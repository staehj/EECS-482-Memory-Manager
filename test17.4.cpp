// lab: swap-backed and file-backed random

#include "vm_app.h"
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include <cstring>
#include <iostream>

int main()
{
    char* s1 = (char *) vm_map(nullptr, 0);
    char *empty = (char *) vm_map(s1, 0);

    if (empty == nullptr) {
        printf("filename was invalid, empty filename\n");
    }
    else {
        printf("filename was valid\n");
        if (empty[0] == -1) {
            printf("empty[0]: %c\n", empty[0]);
        }
        else {
            printf("read failed from invalid file\n");
        }
    }
}
