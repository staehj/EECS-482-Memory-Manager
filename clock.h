#include <forward_list>

#include "vm_pager.h"
#include "vm_arena.h"

class Clock {
    Clock (unsigned int memory_pages) : size(0), max_size (memory_pages) {}

    bool is_full() {
        return size == max_size;
    }

    // always enqueues at end of clock_pages
    void enqueue(unsigned int vpn);

    // swap-backed only, can remove any node in O(1)
    void remove(/* some refernce to which node */);

    // evicts properly based on SWAP/FILE type, returns PPN of evicted page
    unsigned int evict();

private:
    // pair of push/pop while checking reference bit
    // return type TODO
    void tick();

    // // returns vpn of next page to evict
    // unsigned int next_eviction_vpn();


    std::forward_list<unsigned int> clock_pages;
    unsigned int size;
    unsigned int max_size;
};
