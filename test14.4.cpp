// lab: swap-backed and file-backed random

#include "vm_app.h"
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include <cstring>
#include <iostream>

int main()
{
    /* Allocate swap-backed page from the arena */
    char *filename = (char *) vm_map(nullptr, 0);

    /* Write the name of the file that will be mapped */
    filename[VM_PAGESIZE-1] = 'a';

    char *invalid = (char *) vm_map(filename+VM_PAGESIZE-1, 0);

    if (invalid == nullptr) {
        printf("filename was invalid, reached end\n");
    }
    else {
        printf("filename was valid\n");
        printf("invalid[0]: %c\n", invalid[0]);
    }

}
