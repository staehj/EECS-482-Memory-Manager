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

    // Initialize lowest_invalid_vpns (empty)
    std::unordered_map<page_table_t*, unsigned int> lowest_invalid_vpns;

    // Set 0 page pte - TODO: check if we have to do this for kernel pt
    page_table_entry_t zero_pte;
    zero_pte.ppage = 0;
    zero_pte.read_enable = 0;
    zero_pte.write_enable = 0;
    page_table_base_register->ptes[0] = zero_pte;
}

int vm_create(pid_t parent_pid, pid_t child_pid) {
    // create deep copy of parent page table
    page_table_t* child_ptbr = new page_table_t;
    page_table_t* parent_ptbr = page_tables[parent_pid];
    size_t ptes_length = sizeof(parent_ptbr->ptes) / sizeof(page_table_entry_t);
    for (size_t i = 0; i < ptes_length; ++i) {
        child_ptbr->ptes[i].ppage = parent_ptbr->ptes[i].ppage;
        child_ptbr->ptes[i].read_enable = parent_ptbr->ptes[i].read_enable;
        child_ptbr->ptes[i].write_enable = parent_ptbr->ptes[i].write_enable;
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

    // Initialize vpn tracker
    lowest_invalid_vpns[page_table_base_register] = 1;
}

void vm_switch(pid_t pid) {
    page_table_base_register = page_tables[pid];
}

// Called when current process has a fault at virtual address addr.  write_flag
// is true if the access that caused the fault is a write.
int vm_fault(const void* addr, bool write_flag) {
    // addr->vpn->page_states->PageState

    // 
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

    // Clock: remove all swap pages being used by process
    clock.remove(ppns);

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

    // SWAP-backed
    // TODO: is eager swap reservation relevant for 4 credit?
    if (filename == nullptr) {
        // add pte to 0 page to page table
        page_table_base_register->ptes[lowest_invalid_vpn].ppage = 0;
        page_table_base_register->ptes[lowest_invalid_vpn].read_enable = 1;
        page_table_base_register->ptes[lowest_invalid_vpn].write_enable = 0;

        // initialize PageState and put into arena
        std::vector<std::shared_ptr<PageState>> arena = arenas[page_table_base_register];
        arena[lowest_invalid_vpn] = std::make_shared<PageState>(
            PAGE_TYPE::SWAP, 1, 1, 0, 0, nullptr, 0);
    }
    // FILE-backed
    else {
        // check filename is completely in valid part of arena
        if (!filename_valid_in_arena(filename, lowest_invalid_vpn)) {
            return nullptr;
        }

        // check if page in file_table
        if (file_table.find(FileBlock(filename, block)) != file_table.end()) {
            // set arena PageState to point to file_table PageState
               // add pte, check PageState.dirty for write_enable
            
            
        
        }
        else {
        // if file not in file_table
            // check for space in memory through clock
            // if memory is full
                // evict using clock algorithm
            
            // file read into free page 
            // add file page to file table
            // add pte to page table
            // add PageState to arena

        }
    }

    // update lowest invalid vpn
    lowest_invalid_vpn++;
}
