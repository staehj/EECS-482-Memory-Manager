#include <string.h>

struct FileBlock {
    char* filename;
    unsigned int block;

    bool operator==(const FileBlock  &) const;
};  

// == operator overload? is it default?
bool operator== (const FileBlock& fb1, const FileBlock& fb2)
{
    return (strcmp(fb1.filename, fb2.filename)
            && fb1.block == fb2.block);
}
