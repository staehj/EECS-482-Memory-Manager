#ifndef PAGE_STATE_H
#define PAGE_STATE_H

#include "vm_arena.h"
#include "vm_pager.h"

#include <memory>
#include <unordered_map>
#include <vector>


enum PAGE_TYPE {SWAP_BACKED, FILE_BACKED};

struct PageState {
    PAGE_TYPE type;

    // general
    unsigned int ppn;
    unsigned int vpn;  // only swap backed
    bool referenced;
    bool resident;
    bool dirty;

    // SWAP-backed
    unsigned int swap_block;
    page_table_t* owner_ptbr;

    // FILE-backed
    const char* filename;
    unsigned int file_block;
    std::unordered_map<page_table_t*, std::vector<page_table_entry_t*>> shared_ptes;

    PageState(PAGE_TYPE t, unsigned int ppn, unsigned int vpn, bool referenced,
        bool resident, bool dirty, unsigned int swap_block, page_table_t* owner_ptbr,
        const char* filename, unsigned int file_block)
        : type(t), ppn(ppn), vpn(vpn), referenced(referenced), resident(resident),
        dirty(dirty), swap_block(swap_block), owner_ptbr(owner_ptbr), filename(filename),
        file_block(file_block) {}

    void add_pte(page_table_t* ptbr, page_table_entry_t* pte) {
        shared_ptes[ptbr].push_back(pte);
    }

    void update_ptes(unsigned int ppage, unsigned int read, unsigned int write) {
        for (auto &itr : shared_ptes) {
            std::vector<page_table_entry_t*> &ptes = itr.second;
            for (unsigned int i = 0; i < ptes.size(); ++i) {
                ptes[i]->ppage = ppage;
                ptes[i]->read_enable = read;
                ptes[i]->write_enable = write;
            }
        }
    }
};

#endif
