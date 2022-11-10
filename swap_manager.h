#ifndef SWAP_MANAGER_H
#define SWAP_MANAGER_H

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
        num_reserved = 0;
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

    bool reserve() {
        if (num_free() == 0) {
            return false;
        }
        num_reserved++;

        return true;
    }


private:
    void unreserve() {
        num_reserved--;
    }


    std::queue<unsigned int> free_blocks;
    unsigned int max_size;
    unsigned int num_reserved;
};

#endif
