// lab: swap-backed and file-backed random

#include "vm_app.h"
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include <cstring>
#include <iostream>

int main()
{
    // filename wrote on FILE - SWAP
    // second part of filename does not end with null character
    // both swap-backed pages are neither evicted / unreferenced

    // create file-backed page
    char* filename = (char*)vm_map(nullptr, 0);
    strcpy(filename, "lampson83.txt");

    // first part of filename in file-backed page
    char* lampson = (char*)vm_map(filename, 0);
    lampson[VM_PAGESIZE - 3] = 'd';
    lampson[VM_PAGESIZE - 2] = 'a';
    lampson[VM_PAGESIZE - 1] = 't';

    // second part of filename in swap-backed page (not null-character terminated) 
    char* filename_part2 = (char*)vm_map(nullptr, 0);
    strcpy(filename_part2, "hellohellohellohellohellohellohellohello");
    filename_part2[0] = 'a';
    filename_part2[1] = '1';
    filename_part2[2] = '.';
    filename_part2[3] = 'b';
    filename_part2[4] = 'i';
    filename_part2[5] = 'n';

    char* data1 = (char*)vm_map(lampson + VM_PAGESIZE - 3, 0);
    for (int i = 0; i < 10; i++) {
        std::cout << data1[i] << "\n";
    }
}
