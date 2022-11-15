// lab: swap-backed and file-backed random

#include "vm_app.h"
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include <cstring>
#include <iostream>

int main()
{
    // filename wrote on FILE - FILE
    // second part of filename does not end with null character
    // both swap-backed pages are neither evicted / unreferenced

    // create swap-backed page1
    char* filename1 = (char*)vm_map(nullptr, 0);
    strcpy(filename1, "lampson83.txt");

    // create swap-backed page2
    char* filename2 = (char*)vm_map(nullptr, 0);
    strcpy(filename2, "data1.bin");

    // first part of filename in file-backed page
    char* lampson = (char*)vm_map(filename1, 0);
    lampson[VM_PAGESIZE - 3] = 'd';
    lampson[VM_PAGESIZE - 2] = 'a';
    lampson[VM_PAGESIZE - 1] = 't';

    // second part of filename in file-backed page (null terminated)
    char* data1 = (char*)vm_map(filename1, 0);
    data1[0] = 'a';
    data1[1] = '2';
    data1[2] = '.';
    data1[3] = 'b';
    data1[4] = 'i';
    data1[5] = 'n';
    data1[6] = '\0';

    // read from data2
    char* data2 = (char*)vm_map(lampson + VM_PAGESIZE - 3, 0);
    for (int i = 0; i < 10; i++) {
        std::cout << data2[i] << "\n";
    }
}
