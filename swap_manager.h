#include <queue>

class SwapManager {
public:
    SwapManager(unsigned int swap_blocks)
    : max_size(swap_blocks), num_reserved(0) {
        for (unsigned int i = 0; i < max_size; ++i) {
            free_blocks.push(i);
        }
    }

    unsigned int num_free() {
        return free_blocks.size() - num_reserved;
    }

    // calls unreserve for you
    unsigned int get_next_free() {
        unsigned int res = free_blocks.front();
        free_blocks.pop();
        unreserve();
        return res;
    }

    void make_free(unsigned int block) {
        free_blocks.push(block);
    }

    void reserve() {
        num_reserved++;
    }


private:
    void unreserve() {
        num_reserved--;
    }


    std::queue<unsigned int> free_blocks;
    unsigned int max_size;
    unsigned int num_reserved;
};

// check free for vm map - eager swap reservation
// eg. map file-backed
//     -> memory is full, so must evict
//     -> swap backed is chosen as eviction target
//     -> no free swap blocks in disk
//     ---> must return nullptr in vm_map
