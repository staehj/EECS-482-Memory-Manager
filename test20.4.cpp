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

        /* Write the name of the file that will be mapped */
        strcpy(filename, "data1.bin");

        char *p1 = (char *) vm_map (filename, 0);
        char *p2 = (char *) vm_map (filename, 0);
        std::cout << p2[0];
        std::cout << p1[0];
        char *p3 = (char *) vm_map (filename, 0);
        std::cout << p3[0];

        vm_yield();

        p3[0] =  'b';

        char* junk1 = (char *) vm_map(nullptr, 0);
        char* junk2 = (char *) vm_map(nullptr, 0);
        char* junk3 = (char *) vm_map(nullptr, 0);

        junk1[0] = 'a';
        junk2[0] = 'b';
        junk3[0] = 'c';

        std::cout << p3[0];

        vm_yield();
    }
    else { // child
        char *filename_child = (char *) vm_map(nullptr, 0);
        strcpy(filename_child, "data1.bin");

        char* c0 = (char *) vm_map (filename_child, 0);
        char* c1 = (char *) vm_map (filename_child, 1);

        std::cout << c0[0];
        c0[0] = 'a';

        vm_yield();

        c1[0] = 'z';
    }
}
