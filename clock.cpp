#include "clock.h"

#include <cassert>
#include <list>

void Clock::enqueue(unsigned int vpn) {
    clock_pages.push_front(vpn);
}

void Clock::remove(/*TODO*/) {

}

unsigned int Clock::evict() {
    assert(is_full);

    // find next unreferenced page vpn to evict using tick

    // use vpn to get ppn

    // set PTE to r:0, w:0

    // if swap-backed
        // if dirty, write to swap disk
            // TODO?: keep track of which swap blocks are free?
        // update PageState in swap_tables

    // if file-backed
        // path: ptbr->vpn->page_states->filename/block->file_table
        // if dirty, write to disk
        // update PageState through page_states

    // change PageState to all 0

    // pop from clock_pages
}

void Clock::tick() {

}
