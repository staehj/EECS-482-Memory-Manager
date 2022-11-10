#ifndef FILE_BLOCK_H
#define FILE_BLOCK_H

#include <string.h>

struct FileBlock {
    const char* filename;
    unsigned int block;

    FileBlock(const char* filename, unsigned int block)
    : filename(filename), block(block) {}

    bool operator==(const FileBlock &o) const {
        return (strcmp(filename, o.filename) && block == o.block);
    }

    bool operator()() {
        
    }
};  

// // == operator overload? is it default?
// bool operator== (const FileBlock& fb1, const FileBlock& fb2) {
//     return (strcmp(fb1.filename, fb2.filename) && fb1.block == fb2.block);
// }

// // < operator required for std::unordered_map
// bool operator< (const FileBlock& fb1, const FileBlock& fb2) {
//     return (fb1.block < fb2.block || strcmp(fb1.filename, fb2.filename));
// }

#endif
