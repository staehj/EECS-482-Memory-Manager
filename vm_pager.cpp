#include "vm_pager.h"

#include <cassert>
#include <cstring>
#include <cstdio>
#include <string>
#include <unordered_set>

#include "clock.h"
#include "page_state.h"
#include "shared.h"
#include "swap_manager.h"


void vm_init(unsigned int memory_pages, unsigned int swap_blocks) {
    // Initialize 0 page
    std::memset(vm_physmem, 0, VM_PAGESIZE);

    // Initialize Clock (empty)
    clock_.fill_memory_pages(memory_pages);

    // Initialize SwapManager
    swap_manager.fill_swap_blocks(swap_blocks);

    // Initialize phys_mem_pages
    phys_mem_pages.resize(memory_pages, nullptr);
}

int vm_create(pid_t parent_pid, pid_t child_pid) {
    // create deep copy of parent page table
    page_table_t* child_ptbr = new page_table_t();

    // just to be safe, make sure all entries are sanitized to 0
    sanitize_page_table(child_ptbr);

    // add to page_tables
    page_tables[child_pid] = child_ptbr;

    // create 0 page in arena
    std::vector<std::shared_ptr<PageState>> child_arena;

    // add arena to arenas
    arenas[child_ptbr] = child_arena;

    return 0;
}

void vm_switch(pid_t pid) {
    page_table_base_register = page_tables[pid];
}

// Called when current process has a fault at virtual address addr.  write_flag
// is true if the access that caused the fault is a write.
int vm_fault(const void* addr, bool write_flag) {
    if (addr < VM_ARENA_BASEADDR) {
        return -1;
    }

    // get PageState and pte using addr
    unsigned int vpn = va_to_vpn(addr);

    // INVALID CASE: vpn is out of bounds (not in valid arena)
    // if (addr < VM_ARENA_BASEADDR 
    if (vpn >= lowest_invalid_vpn()) {
        return -1;
    }

    std::vector<std::shared_ptr<PageState>> &arena = arenas[page_table_base_register];
    std::shared_ptr<PageState> &page_state = arena[vpn];
    page_table_entry_t &pte = page_table_base_register->ptes[vpn];

    // Note: r:1, w:1 would never fault (you can read and write to it legally)
    assert(!(pte.read_enable == 1 && pte.write_enable == 1));  // 26.4
    // r:0, w:1 is impossible (stae-fficial)
    assert(!(pte.read_enable == 0 && pte.write_enable == 1));

    // SWAP-backed
    if (page_state->type == PAGE_TYPE::SWAP_BACKED) {
        // r:0, w:0
        if (pte.read_enable == 0 && pte.write_enable == 0) {
            // CASE: resident but unreferenced
            if (page_state->resident) {
                assert(!(page_state->referenced));
                // mark page as referenced
                page_state->referenced = true;

                // update pte, setting writable if dirty or attempting to write
                if (page_state->dirty || write_flag == 1) {
                    update_pte(page_state->vpn, page_state->ppn, 1, 1,
                        page_state->owner_ptbr);
                    page_state->dirty = true;
                }
                else {
                    update_pte(page_state->vpn, page_state->ppn, 1, 0,
                        page_state->owner_ptbr);
                }
            }
            // CASE: non-resident
            else {
                unsigned int ppn = disk_to_mem(nullptr, page_state->swap_block);

                // file_read fails (swap block not in disk)
                // Note: should SWAP-backed should never fail file_read?
                if (ppn == 0) {
                    return -1;
                }

                // update phys_mem_pages
                phys_mem_pages[ppn] = page_state;

                // update page state
                page_state->ppn = ppn;
                page_state->referenced = true;
                page_state->resident = true;
                if (write_flag == 1) {
                    page_state->dirty = true;
                }

                // update pte
                if (write_flag == 0) {
                    update_pte(vpn, ppn, 1, 0, page_state->owner_ptbr);
                }
                else {
                    update_pte(vpn, ppn, 1, 1, page_state->owner_ptbr);
                }
            }
        }
        // r:1, w:0
        else if (pte.read_enable == 1 && pte.write_enable == 0) {
            assert(write_flag == 1);

            // CASE: fresh swap-backed page (zero)
            if (page_state->ppn == 0) {
                unsigned int free_ppn = evict_or_get_free_ppn();

                // file_write failed in evicted case
                if (free_ppn == 0) {
                    return -1;
                }

                // copy zero page to new ppn
                std::memcpy((void*)ppn_to_mem_addr(free_ppn) , vm_physmem, VM_PAGESIZE);

                // update phys_mem_pages
                phys_mem_pages[free_ppn] = page_state;

                // update page state
                page_state->ppn = free_ppn;
                page_state->referenced = true;
                page_state->resident = true;
                page_state->dirty = true;

                // update pte
                update_pte(vpn, free_ppn, 1, 1, page_state->owner_ptbr);
            }
            // CASE: existing clean page
            else {
                make_page_dirty(vpn, page_state);
            }
        }
    }
    // FILE-backed
    else {
        // r:0, w:0
        if (pte.read_enable == 0 && pte.write_enable == 0) {
            assert(file_in_file_table(page_state->filename, page_state->file_block));
            // CASE: resident but unreferenced
            if (page_state->resident) {
                assert(!(page_state->referenced));

                // mark page as referenced
                page_state->referenced = true;

                // update pte, setting writable if dirty or attempting to write
                if (page_state->dirty || write_flag == 1) {
                    page_state->update_ptes(page_state->ppn, 1, 1);
                    page_state->dirty = true;
                }
                else {
                    page_state->update_ptes(page_state->ppn, 1, 0);
                }
            }
            // CASE: non-resident
            else {
                unsigned int ppn = disk_to_mem(page_state->filename,
                    page_state->file_block);

                // file_read failed (file not in disk)
                if (ppn == 0) {
                    return -1;
                }

                // update phys_mem_pages
                phys_mem_pages[ppn] = page_state;

                // update page state
                unsigned int read_enable = 1;
                unsigned int write_enable = 0;
                page_state->ppn = ppn;
                page_state->referenced = true;
                page_state->resident = true;
                if (write_flag == 1) {
                    page_state->dirty = true;
                    write_enable = 1;
                }
                // update shared_ptes
                page_state->update_ptes(ppn, read_enable, write_enable);
            }
        }
        // r:1, w:0
        // CASE: existing clean page
        // Note: writing to 0 page is already considered invalid in earlier check
        else if (pte.read_enable == 1 && pte.write_enable == 0) {
            make_page_dirty(vpn, page_state);
        }
    }
    return 0;
}

void vm_destroy() {
    std::vector<std::shared_ptr<PageState>> &arena = arenas[page_table_base_register];
    std::unordered_set<unsigned int> ppns;
    for (unsigned int i = 0; i < arena.size(); ++i) {
        std::shared_ptr<PageState> &page = arena[i];
        // SWAP
        if (page->type == PAGE_TYPE::SWAP_BACKED) {
            // in MEMORY
            if (page->resident) {
                unsigned int ppn = vpn_to_ppn(i);
                ppns.insert(ppn);

                phys_mem_pages[ppn] = nullptr;
            }
            // nothing special about disk

            // SwapManager: mark process swap blocks in disk as free
            swap_manager.make_free(page->swap_block);
        }
        // FILE
        else {
            if (page->shared_ptes.find(page_table_base_register)
                != page->shared_ptes.end()) {
                page->shared_ptes.erase(page_table_base_register);
            }
        }
    }

    // Clock: remove all swap pages being used by process, and mark as free
    clock_.make_free(ppns);

    // delete swap block PageStates
    arenas.erase(page_table_base_register);

    // delete page table from page_tables
    for (auto itr = page_tables.begin(); itr != page_tables.end(); itr++) {
        if (itr->second == page_table_base_register) {
            page_tables.erase(itr);
            break;
        }
    }

    // just to be safe, make sure all entries are sanitized to 0
    sanitize_page_table(page_table_base_register);

    // delete page table
    delete page_table_base_register;
}

void *vm_map(const char *filename, unsigned int block) {
    // check that arena is not full
    unsigned int new_vpn = lowest_invalid_vpn();
    if (new_vpn >= (unsigned int) VM_ARENA_SIZE/VM_PAGESIZE) {
        return nullptr;
    }

    std::vector<std::shared_ptr<PageState>> &arena = arenas[page_table_base_register];
    // SWAP-backed
    if (filename == nullptr) {
        // update swap manager (eager swap reservation)
        unsigned int swap_block;
        if (swap_manager.num_free() == 0) {
            return nullptr;
        }
        else {
            swap_block = swap_manager.get_next_free();
        }

        // add pte to 0 page to page table
        update_pte(new_vpn, 0, 1, 0, page_table_base_register);

        // initialize PageState and put into arena
        arena.push_back(std::make_shared<PageState>(
            PAGE_TYPE::SWAP_BACKED, 0, new_vpn, true, true, false, swap_block,
            page_table_base_register, nullptr, 0));
    }
    // FILE-backed
    else {
        // check filename is completely in valid part of arena
        const char* page_filename = get_filename(filename);
        if (page_filename == nullptr) {
            return nullptr;
        }

        // page in file_table
        if (file_in_file_table(page_filename, block)) {
            std::shared_ptr<PageState> &page_state
                = file_table[std::string(page_filename)][block];

            // set arena PageState to point to file_table PageState
            arena.push_back(page_state);

            // add pte* to shared_ptes
            page_table_entry_t* pte = &(page_table_base_register->ptes[new_vpn]);
            page_state->add_pte(page_table_base_register, pte);

            // referenced
            if (page_state->referenced) {
                // referenced and resident
                if (page_state->resident) {
                    // dirty
                    if (page_state->dirty) {
                        update_pte(new_vpn, page_state->ppn, 1, 1,
                            page_table_base_register);
                    }
                    // clean
                    else {
                        update_pte(new_vpn, page_state->ppn, 1, 0,
                            page_table_base_register);
                    }
                }
                // referenced, not resident
                else {
                    update_pte(new_vpn, page_state->ppn, 0, 0, page_table_base_register);
                }
            }
            // unreferenced
            else {
                update_pte(new_vpn, page_state->ppn, 0, 0, page_table_base_register);
            }
        }
        // file not in file_table
        else {
            // arbitrary, doesn't actually matter
            unsigned int ppn = 0;

            // create new PageState
            std::shared_ptr<PageState> new_page_state
                = std::make_shared<PageState>(
                    PAGE_TYPE::FILE_BACKED, ppn, new_vpn, false, false, false, 0,
                    page_table_base_register, page_filename, block);
            page_table_entry_t* pte = &(page_table_base_register->ptes[new_vpn]);
            new_page_state->add_pte(page_table_base_register, pte);

            // add PageState to arena
            arena.push_back(new_page_state);

            // add pte to page table
            update_pte(new_vpn, ppn, 0, 0, page_table_base_register);

            // add page_State to file_table
            file_table[std::string(new_page_state->filename)][new_page_state->file_block]
                = new_page_state;
        }
    }

    return (void*)(((intptr_t)VM_ARENA_BASEADDR) + new_vpn*VM_PAGESIZE);
}
