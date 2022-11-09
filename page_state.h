#ifndef PAGE_STATE_H
#define PAGE_STATE_H

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

#endif
