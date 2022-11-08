#include "vm_app.h"
#include "vm_arena.h"
#include "vm_pager.h"

#include <memory>


enum PAGE_TYPE {SWAP, FILE};

struct PageState {
    PAGE_TYPE type;

    // general
    unsigned int ppn;
    unsigned int vpn;
    bool referenced;
    bool resident;
    bool dirty;

    // SWAP-backed
    unsigned int swap_block;

    // FILE-backed
    char* filename;
    unsigned int file_block;

    PageState(PAGE_TYPE t, unsigned int ppn, unsigned int vpn, bool referenced,
        bool resident, bool dirty, unsigned int swap_block, char* filename,
        unsigned int file_block)
        : type(t), ppn(ppn), vpn(vpn), referenced(referenced), resident(resident),
        dirty(dirty), swap_block(swap_block), filename(filename), file_block(file_block)
        {}
};

std::shared_ptr<PageState> PageState_deep_copy(std::shared_ptr<PageState> parent) {
    std::shared_ptr<PageState> child = std::make_shared<PageState>(
        parent->type, parent->ppn, parent->vpn, parent->referenced, parent->resident,
        parent->dirty, parent->swap_block, parent->filename, parent->file_block);
    return child;
}


// arr = array of PageState
// index: virtual page number - VIRTUAL_ARENA_BASE

// Process asks for (virtual) page N
// MMU looks up page_table[N] == n'th PTE
// if PTE is accessible (read != 0 or write != 0)
//     access ...
// else (vm_fault get's called with virtual address)
//     virtual address: extract VPN
//     arr[VPN - VIRTUAL_ARENA_BASE] == get relevant PageState
//     if resident:
//         go to physical page in page table and do stuff
//     else:
//         1. figure out which page to evict from phymem (clock)
//             - call this page page X
//         2. file_read from swap_block'th block in swap file
//             eg. file_read(nullptr, swap_block, vm_physmem+(X*VM_PAGESIZE))
