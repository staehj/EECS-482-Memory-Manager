#include "clock.h"

#include <cassert>
#include <list>
#include <memory>
#include <queue>

#include "page_state.h"
#include "shared.h"


void Clock::enqueue(unsigned int vpn) {
    active_pages.push_front(vpn);
}

void Clock::make_free(std::unordered_set<unsigned int> ppns) {
    for (auto itr = active_pages.begin(); itr != active_pages.end(); ++itr) {
        if (ppns.find(*itr) != ppns.end()) {
            make_free_itr(itr);
        }
    }
}

void Clock::make_free(unsigned int ppn) {
    for (auto itr = active_pages.begin(); itr != active_pages.end(); ++itr) {
        if (ppn == *itr) {
            make_free_itr(itr);
            return;
        }
    }
}

unsigned int Clock::evict() {
    assert(is_full());
    assert(free_pages.empty());

    // find next unreferenced page ppn to evict using tick
    unsigned int target_ppn = next_eviction_ppn();

    // mark page as free in clock and phy_mem_pages
    make_free(target_ppn);

    // get corresponding PageState
    std::shared_ptr<PageState> page_state = phys_mem_pages[target_ppn];

    // set PTE to ppn:0 (arbitrary), r:0, w:0
    update_pte(page_state->vpn, 0, 0, 0, page_table_base_register);

    // if swap-backed
    if (page_state->type == PAGE_TYPE::SWAP) {
        // if dirty, write to swap disk
        if (page_state->dirty) {
            // determine free swap block + unreserve a swap block
            unsigned int target_swap_block = swap_manager.get_next_free();

            // write page to swap file (disk)
            file_write(nullptr, target_swap_block, ppn_to_mem(target_ppn));

            // update PageState swap_block
            page_state->swap_block = target_swap_block;
        }
    }
    // if file-backed
    else {
        // remove entry from file_table
        file_table.erase(FileBlock(page_state->filename, page_state->file_block));

        // if dirty, write to disk
        if (page_state->dirty) {
            file_write(page_state->filename, page_state->file_block,
                ppn_to_mem(target_ppn));
        }
    }

    // change common PageState vars
    page_state->ppn = 0;
    page_state->dirty = false;
    page_state->referenced = false;
    page_state->resident = false;

    // return evicted ppn
    return target_ppn;
}

unsigned int Clock::get_free_ppn() {
    assert(free_pages.size() > 0);
    unsigned int ppn = free_pages.front();
    free_pages.pop();
    return ppn;
}

// returns vpn of next page to evict
unsigned int Clock::next_eviction_ppn() {
    unsigned int target;
    while (true) {
        target = tick();
        if (target != 0) {
            return target;
        }
    }
}

// returns 0 if ticked page does not need to be evicted
// returns ppn if ticked page requires eviction
unsigned int Clock::tick() {
    // get "front" of clock queue
    unsigned int cur = active_pages.front();
    active_pages.pop_front();

    // get corresponding PageState
    std::shared_ptr<PageState> page_state =  phys_mem_pages[cur];
    assert(page_state->ppn == cur);

    // check referenced bit
    if (page_state->referenced) {
        page_state->referenced = false;
        active_pages.push_back(cur);
        return 0;
    }

    // found eviction target
    return cur;
}

void Clock::make_free_itr(std::list<unsigned int>::iterator itr) {
    // update Clock vars
    active_pages.erase(itr);
    free_pages.push(*itr);

    // mark phys_mem_pages
    phys_mem_pages[*itr] = nullptr;
}
