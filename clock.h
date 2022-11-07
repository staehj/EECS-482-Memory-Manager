#include <list>
#include <unordered_set>

#include "vm_pager.h"
#include "vm_arena.h"

class Clock {
public:
    Clock (unsigned int memory_pages) : max_size(memory_pages) {}

    unsigned int size() {
        return clock_pages.size();
    }

    bool is_full() {
        return size() == max_size;
    }

    // always enqueues at end of clock_pages
    void enqueue(unsigned int ppn);

    // swap-backed only
    void remove(std::unordered_set<unsigned int> ppns);

    // evicts properly based on SWAP/FILE type, returns PPN of evicted page
    unsigned int evict();
    

private:
    // pair of push/pop while checking reference bit
    // return type TODO
    void tick();

    // // returns vpn of next page to evict
    // unsigned int next_eviction_vpn();

    std::list<unsigned int> clock_pages;
    unsigned int max_size;
};
