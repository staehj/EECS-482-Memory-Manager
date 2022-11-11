#ifndef PAGE_STATE_H
#define PAGE_STATE_H

#include "vm_arena.h"
#include "vm_pager.h"

#include <memory>


enum PAGE_TYPE {SWAP_BACKED, FILE_BACKED};

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
    const char* filename;
    unsigned int file_block;

    PageState(PAGE_TYPE t, unsigned int ppn, unsigned int vpn, bool referenced,
        bool resident, bool dirty, unsigned int swap_block, const char* filename,
        unsigned int file_block)
        : type(t), ppn(ppn), vpn(vpn), referenced(referenced), resident(resident),
        dirty(dirty), swap_block(swap_block), filename(filename), file_block(file_block)
        {}
};

#endif
