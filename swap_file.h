#include <list>

class SwapFile {
public:
    SwapFile(unsigned int swap_blocks) : max_size(swap_blocks) {
        for (unsigned int i = 0; i < max_size; ++i) {
            free_blocks.push_back(i);
        }
    }

    unsigned int num_free() {
        return free_blocks.size();
    }

    unsigned int get_next_free() {
        unsigned int res = free_blocks.front();
        free_blocks.pop_front();
        return res;
    }

    void make_free(unsigned int block) {
        free_blocks.push_back(block);
    }


private:
    std::list<unsigned int> free_blocks;
    unsigned int max_size;
};
