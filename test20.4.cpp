// lab: swap-backed and file-backed random

#include "vm_app.h"
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include <cstring>
#include <iostream>

int main()
{
    // filename wrote on SWAP - FILE
    // second part of filename does not end with null character
    // both swap-backed pages are neither evicted / unreferenced

    // create file-backed page
    char* filename = (char*)vm_map(nullptr, 0); 
    strcpy(filename, "lampson83.txt");

    // second part of filename in file-backed page (not null-character terminated) 
    char* lampson = (char*)vm_map(filename, 0);
    lampson[0] = 'a';
    lampson[1] = '1';
    lampson[2] = '.';
    lampson[3] = 'b';
    lampson[4] = 'i';
    lampson[5] = 'n';

    // first part of filename in swap-backed page
    filename[VM_PAGESIZE - 3] = 'd';
    filename[VM_PAGESIZE - 2] = 'a';
    filename[VM_PAGESIZE - 1] = 't';

    char* data1 = (char*)vm_map(filename + VM_PAGESIZE - 3, 0);
    for (int i = 0; i < 10; i++) {
        std::cout << data1[i] << "\n";
    }
}
