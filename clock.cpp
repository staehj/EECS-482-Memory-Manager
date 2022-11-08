#include "clock.h"

#include <cassert>
#include <list>
#include <queue>


void Clock::enqueue(unsigned int vpn) {
    active_pages.push_front(vpn);
}

void Clock::make_free(std::unordered_set<unsigned int> ppns) {
    for (auto itr = active_pages.begin(); itr != active_pages.end(); ++itr) {
        if (ppns.find(*itr) != ppns.end()) {
            active_pages.erase(itr);
            free_pages.push(*itr);
        }
    }
}

// NOTE: must perform file_write here
unsigned int Clock::evict() {
    assert(is_full());

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

    // pop from active_pages
}

unsigned int Clock::get_free_ppn() {
    assert(free_pages.size() > 0);
    unsigned int ppn = free_pages.front();
    free_pages.pop();
    return ppn;
}

void Clock::tick() {

}
