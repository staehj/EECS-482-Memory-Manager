#include "vm_app.h"
#include "vm_arena.h"
#include "vm_pager.h"

enum PAGE_TYPE {SWAP, FILE};

struct PageState {
    PAGE_TYPE type;

    // general
    bool referenced;
    bool resident;
    bool dirty;

    // SWAP-backed
    unsigned int swap_block;

    // FILE-backed
    char* filename;
    unsigned int file_block;

    PageState (PAGE_TYPE t, unsigned int swap_block = 0, char* filename='\0', 
        unsigned int file_block = 0) : referenced(0), resident(0), dirty(0) {
        // ?? TODO ??
    };
};



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
