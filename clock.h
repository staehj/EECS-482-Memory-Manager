#ifndef CLOCK_H
#define CLOCK_H

#include <list>
#include <unordered_set>
#include <queue>

#include "vm_pager.h"
#include "vm_arena.h"

class Clock {
public:
    Clock (unsigned int memory_pages) : max_size(memory_pages) {
        for (unsigned int i = 1; i < memory_pages; ++i) {
            free_pages.push(i);
        }
    }

    bool is_full() {
        return size() == max_size;
    }

    // always enqueues at end of active_pages
    void make_active(unsigned int ppn);

    // marks physical page as free and updates phys_mem_pages
    void make_free(std::unordered_set<unsigned int> ppns);
    void make_free(unsigned int ppn);

    // evicts properly based on SWAP/FILE type, returns PPN of evicted page
    unsigned int evict();

    unsigned int get_free_ppn();

private:
    unsigned int size() {
        // 1+ for 0 page
        return 1 + active_pages.size();
    }

    // returns vpn of next page to evict
    unsigned int next_eviction_ppn();

    // pair of push/pop while checking reference bit
    // return type TODO
    unsigned int tick();

    void make_free_itr(std::list<unsigned int>::iterator itr);


    std::queue<unsigned int> free_pages;
    std::list<unsigned int> active_pages;
    unsigned int max_size;
};

#endif
