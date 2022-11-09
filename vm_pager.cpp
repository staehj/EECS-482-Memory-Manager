#include "vm_pager.h"

#include <cassert>
#include <cstring>
#include <unordered_set>

#include "clock.h"
#include "file_block.h"
#include "page_state.h"
#include "shared.h"
#include "swap_manager.h"


void vm_init(unsigned int memory_pages, unsigned int swap_blocks) {
    // Initialize 0 page
    memset(vm_physmem, 0, VM_PAGESIZE);

    // Initialize Clock (empty)
    Clock clock(memory_pages);

    // Initialize SwapManager
    SwapManager swap_manager(swap_blocks);

    // Initialize file_table (empty)
    std::unordered_map<FileBlock, std::shared_ptr<PageState>> file_table;

    // Initialize page_states (empty)
    std::unordered_map<page_table_t*, std::vector<std::shared_ptr<PageState>>> arenas;

    // Initialize page_tables (empty)
    std::unordered_map<pid_t, page_table_t*> page_tables;

    // Initialize phys_mem_pages
    std::vector<std::shared_ptr<PageState>> phys_mem_pages(memory_pages, nullptr);

    // Initialize lowest_invalid_vpns (empty)
    std::unordered_map<page_table_t*, unsigned int> lowest_invalid_vpns;

    // Initialize buffer
    char buffer[VM_PAGESIZE];
}

int vm_create(pid_t parent_pid, pid_t child_pid) {
    // create deep copy of parent page table
    page_table_t* child_ptbr = new page_table_t;
    page_table_t* parent_ptbr = page_tables[parent_pid];
    size_t ptes_length = sizeof(parent_ptbr->ptes) / sizeof(page_table_entry_t);
    for (size_t i = 0; i < ptes_length; ++i) {
        update_pte(i, parent_ptbr->ptes[i].ppage, parent_ptbr->ptes[i].read_enable,
            parent_ptbr->ptes[i].write_enable, child_ptbr);
    }

    // add to page_tables
    page_tables[child_pid] = child_ptbr;

    // create deep copy of parent arena
    std::vector<std::shared_ptr<PageState>> parent_arena = arenas[parent_ptbr];
    std::vector<std::shared_ptr<PageState>> child_arena;
    child_arena.reserve(parent_arena.size());
    for (size_t i = 0; i < parent_arena.size(); ++i) {
        std::shared_ptr<PageState> child_page = PageState_deep_copy(parent_arena[i]);
        child_arena.push_back(child_page);
    }

    // set child 0 page
    update_pte(0, 0, 1, 0, child_ptbr);

    // Initialize vpn tracker
    lowest_invalid_vpns[page_table_base_register] = 1;
}

void vm_switch(pid_t pid) {
    page_table_base_register = page_tables[pid];
}

// Called when current process has a fault at virtual address addr.  write_flag
// is true if the access that caused the fault is a write.
int vm_fault(const void* addr, bool write_flag) {
    // get PageState and pte using addr
    unsigned int vpn = va_to_vpn(addr);

    // check if vpn is out of bound
    if (addr < VM_ARENA_BASEADDR 
        || vpn >= lowest_invalid_vpns[page_table_base_register]) {
        return -1;
    }

    // if write to zero page
    if (vpn == 0 && write_flag == 1) {
        return -1;
    }

    std::vector<std::shared_ptr<PageState>> arena = arenas[page_table_base_register];
    std::shared_ptr<PageState> page_state = arena[vpn];
    page_table_entry_t pte = page_table_base_register->ptes[vpn];

    // Note: r:1, w:1 would never fault (you can read and write to it legally)
    assert(!(pte.read_enable == 1 && pte.write_enable == 1));
    // r:0, w:1 is impossible (stae-fficial)
    assert(!(pte.read_enable == 0 && pte.write_enable == 1));

    // SWAP-backed
    if (page_state->type == PAGE_TYPE::SWAP) {
        // r:0, w:0
        if (pte.read_enable == 0 && pte.write_enable == 0) {
            // non-resident
            unsigned int ppn = disk_to_mem(nullptr, page_state->swap_block);

            // update phys_mem_pages
            phys_mem_pages[ppn] = page_state;

            // for eager swap reservation
            if (!swap_manager.reserve()) {
                return -1;
            }
            swap_manager.make_free(page_state->swap_block);

            // update page state
            page_state->ppn = ppn;
            page_state->referenced = true;
            page_state->resident = true;
            if (write_flag == 1) {
                page_state->dirty = true;
            }
            page_state->swap_block = 0;

            // update pte
            if (write_flag == 0) {
                update_pte(vpn, ppn, 1, 0, page_table_base_register);
            }
            else {
                update_pte(vpn, ppn, 1, 1, page_table_base_register);
            }
        }
        // r:1, w:0
        else if (pte.read_enable == 1 && pte.write_enable == 0) {
            assert(write_flag == 1);

            // if fresh swap-backed page (zero)
            if (page_state->ppn == 0) {
                unsigned int free_ppn = evict_or_get_free_ppn();

                // update page state
                page_state->ppn = free_ppn;
                page_state->referenced = true;
                page_state->resident = true;
                page_state->dirty = true;
                page_state->swap_block = 0;

                // update swap manager (eager swap reservation)
                if (!swap_manager.reserve()) {
                    return -1;
                }

                // update pte
                update_pte(vpn, free_ppn, 1, 1, page_table_base_register);
            }
            // if existing clean page
            else {
                assert(page_state->swap_block == 0);
                make_page_dirty(vpn, page_state);
            }
        }
    }
    // FILE-backed
    else {
        // r:0, w:0
        if (pte.read_enable == 0 && pte.write_enable == 0) {
            // non-resident
            unsigned int ppn = disk_to_mem(page_state->filename, page_state->file_block);

            // update phys_mem_pages
            phys_mem_pages[ppn] = page_state;

            // insert into file table
            assert(file_table.find(FileBlock(page_state->filename,
                   page_state->file_block)) == file_table.end());
            file_table[FileBlock(page_state->filename, page_state->file_block)]
                = page_state;

            // update page state
            page_state->ppn = ppn;
            page_state->referenced = true;
            page_state->resident = true;
            if (write_flag == 1) {
                page_state->dirty = true;
            }

            // update pte
            if (write_flag == 0) {
                update_pte(vpn, ppn, 1, 0, page_table_base_register);
            }
            else {
                update_pte(vpn, ppn, 1, 1, page_table_base_register);
            }
        }
        // r:1, w:0
        else if (pte.read_enable == 1 && pte.write_enable == 0) {
            make_page_dirty(vpn, page_state);
        }
    }
}

void vm_destroy() {
    std::vector<std::shared_ptr<PageState>> arena = arenas[page_table_base_register];
    std::unordered_set<unsigned int> ppns;
    for (unsigned int i = 0; i < arena.size(); ++i) {
        std::shared_ptr<PageState> page = arena[i];
        if (page->type == PAGE_TYPE::SWAP) {
            // in MEMORY
            if (page->resident) {
                unsigned int ppn = vpn_to_ppn(i);
                ppns.insert(ppn);
            }
            // in DISK
            else {
                // SwapManager: mark process swap blocks in disk as free
                swap_manager.make_free(page->swap_block);
            }
        }
    }

    // Clock: remove all swap pages being used by process, and mark as free
    clock.make_free(ppns);

    // delete swap block PageStates
    arenas.erase(page_table_base_register);

    // delete page table
    delete page_table_base_register;

    // TODO: delete from page_tables??
}

void *vm_map(const char *filename, unsigned int block) {
    // check that arena is not full
    unsigned int lowest_invalid_vpn = lowest_invalid_vpns[page_table_base_register];
    if (lowest_invalid_vpn == VM_ARENA_SIZE) {
        assert(lowest_invalid_vpn <= VM_ARENA_SIZE);
        return nullptr;
    }

    std::vector<std::shared_ptr<PageState>> arena = arenas[page_table_base_register];
    // SWAP-backed
    if (filename == nullptr) {
        // add pte to 0 page to page table
        update_pte(lowest_invalid_vpn, 0, 1, 0, page_table_base_register);

        // initialize PageState and put into arena
        arena[lowest_invalid_vpn] = std::make_shared<PageState>(
            PAGE_TYPE::SWAP, 0, lowest_invalid_vpn, 1, 1, 0, 0, nullptr, 0);
    }
    // FILE-backed
    else {
        // check filename is completely in valid part of arena
        if (!filename_valid_in_arena(filename, lowest_invalid_vpn)) {
            return nullptr;
        }

        // page in file_table
        FileBlock file_block(filename, block);
        if (file_table.find(file_block) != file_table.end()) {
            std::shared_ptr<PageState> page_state_ptr = file_table[file_block];

            // set arena PageState to point to file_table PageState
            arena[lowest_invalid_vpn] = page_state_ptr;

            // add pte, check PageState.dirty for write_enable
            if (page_state_ptr->dirty) {
                update_pte(lowest_invalid_vpn, page_state_ptr->ppn,
                    1, 1, page_table_base_register);
            }
            else {
                update_pte(lowest_invalid_vpn, page_state_ptr->ppn,
                    1, 0, page_table_base_register);
            }
        }
        // file not in file_table
        else {
            // // read in file into buffer
            // file_read(filename, block, buffer);

            // // if memory is full
            // unsigned int free_ppn;
            // if (clock.is_full()) {
            //     // evict using clock algorithm
            //     free_ppn = clock.evict();
            // }
            // else {
            //     free_ppn = clock.get_free_ppn();
            // }

            // // write contents of buffer into memory
            // std::memcpy(ppn_to_mem_addr(free_ppn) , buffer, VM_PAGESIZE);

            unsigned int ppn = disk_to_mem(filename, block);

            // create new PageState
            std::shared_ptr<PageState> new_page_state
                = std::make_shared<PageState>(
                    PAGE_TYPE::FILE, ppn, lowest_invalid_vpn, 1, 1, 0, 0, filename,
                    block);

            // update phys_mem_pages
            phys_mem_pages[ppn] = new_page_state;

            // add file page to file table
            file_table[file_block] = new_page_state;

            // add PageState to arena
            arena[lowest_invalid_vpn] = new_page_state;

            // add pte to page table
            update_pte(lowest_invalid_vpn, ppn, 1, 0, page_table_base_register);
        }
    }

    // update lowest invalid vpn
    lowest_invalid_vpn++;
}
