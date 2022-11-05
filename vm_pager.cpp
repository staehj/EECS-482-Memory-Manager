#include "vm_pager.h"

#include <cstring>

#include "clock.h"
#include "page_state.h"
#include "shared.h"
#include "swap_file.h"


void vm_init(unsigned int memory_pages, unsigned int swap_blocks) {
    // Initialize 0 page
    memset(vm_physmem, 0, VM_PAGESIZE);

    // Initialize Clock (empty)
    Clock clock(memory_pages);

    // Initialize SwapFile
    SwapFile swap_manager(swap_blocks);

    // Initialize file_table (empty)
    std::unordered_map<FileBlock, std::shared_ptr<PageState>> file_table;

    // Initialize page_states (empty)
    std::unordered_map<page_table_t*, std::vector<std::shared_ptr<PageState>>> arenas;

    // Initialize page_tables (empty)
    std::unordered_map<pid_t, page_table_t*> page_tables;
}

int vm_create(pid_t parent_pid, pid_t child_pid {
    // create page table for process and insert in page_tables

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
    // mark relevant swap blocks as dead

    // delete swap block PageStates

    // delete page table

}

void *vm_map(const char *filename, unsigned int block) {
    // if filename == nullptr -> swap backed
        //initialize page states

    //else -> file backed
        //if filename not in arena return nullptr
        //else initialize page states and file table(?)

}
