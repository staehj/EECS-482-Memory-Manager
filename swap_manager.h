#include <queue>

class SwapManager {
public:
    SwapManager(unsigned int swap_blocks) : max_size(swap_blocks) {
        for (unsigned int i = 0; i < max_size; ++i) {
            free_blocks.push(i);
        }
    }

    unsigned int num_free() {
        return free_blocks.size();
    }

    unsigned int get_next_free() {
        unsigned int res = free_blocks.front();
        free_blocks.pop();
        return res;
    }

    void make_free(unsigned int block) {
        free_blocks.push(block);
    }


private:
    std::queue<unsigned int> free_blocks;
    unsigned int max_size;
};

// check free for vm map - eager swap reservation
// eg. map file-backed
//     -> memory is full, so must evict
//     -> swap backed is chosen as eviction target
//     -> no free swap blocks in disk
//     ---> must return nullptr in vm_map
