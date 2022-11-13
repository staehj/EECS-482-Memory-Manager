#ifndef SWAP_MANAGER_H
#define SWAP_MANAGER_H

#include <cassert>
#include <queue>

class SwapManager {
public:
    // SwapManager(unsigned int swap_blocks)
    // : max_size(swap_blocks), num_reserved(0) {
    //     for (unsigned int i = 0; i < max_size; ++i) {
    //         free_blocks.push(i);
    //     }
    // }

    void fill_swap_blocks(unsigned int swap_blocks) {
        max_size = swap_blocks;
        for (unsigned int i = 0; i < max_size; ++i) {
            free_blocks.push(i);
        }
    }

    unsigned int num_free() {
        return free_blocks.size();
    }

    // returns 0 if no free blocks
    unsigned int get_next_free() {
        assert(!free_blocks.empty());
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

#endif
