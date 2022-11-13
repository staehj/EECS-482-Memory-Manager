// swap-basked: random test case

#include "vm_app.h"
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include <cstring>
#include <iostream>

int main() {
    /* 4 pages of physical memory in the system */
    if (fork()) { // parent
        /* Allocate swap-backed page from the arena */
        char *filename = (char *) vm_map(nullptr, 0);

        /* Write the name of the file that will be mapped */
        strcpy(filename, "lampson83.txt");

        // Map a page from the specified file */
        char *p0 = (char *) vm_map (filename, 0);

        vm_yield();

        char *swap0 = (char *) vm_map (nullptr, 0);
        swap0[0] = 'a';
        char *swap1 = (char *) vm_map (nullptr, 0);
        swap1[0] = 'b';  // p0/c0 becomes unreferenced

        vm_yield();
    }
    else { // child
        char *filename_child = (char *) vm_map(nullptr, 0);
        strcpy(filename_child, "lampson83.txt");

        char* c0 = (char *) vm_map (filename_child, 0);

        std::cout << c0[0];

        vm_yield();

        c0[0] = 'a';

        char *swap_child0 = (char *) vm_map (nullptr, 0);
        swap_child0[0] = 'x';
        char *swap_child1 = (char *) vm_map (nullptr, 0);
        swap_child1[0] = 'y';
        char *swap_child2 = (char *) vm_map (nullptr, 0);
        swap_child2[0] = 'z';
    }
}
