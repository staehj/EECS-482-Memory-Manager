// lab: swap-backed and file-backed random

#include "vm_app.h"
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include <cstring>
#include <iostream>

int main()
{
    // filename wrote on two swap-backed pages
    // second part of filename does not end with null character
    // both swap-backed pages are neither evicted / unreferenced

    char* filename_part1 = (char*)vm_map(nullptr, 0); // filename first part
    filename_part1[VM_PAGESIZE - 2] = 'l';
    filename_part1[VM_PAGESIZE - 1] = 'a';

    char* filename_part2 = (char*)vm_map(nullptr, 0); // filename second part
    strcpy(filename_part2, "hellohellohellohellohellohellohellohello");
    filename_part2[0] = 'm';
    filename_part2[1] = 'p';
    filename_part2[2] = 's';
    filename_part2[3] = 'o';
    filename_part2[4] = 'n';
    filename_part2[5] = '8';
    filename_part2[6] = '3';
    filename_part2[7] = '.';
    filename_part2[8] = 't';
    filename_part2[9] = 'x';
    filename_part2[10] = 't';

    char* lampson = (char*)vm_map(filename_part1 + VM_PAGESIZE - 2, 0); // file back page
    for (int i = 0; i < 10; i++) {
        std::cout << lampson[i] << "\n";
    }
}
