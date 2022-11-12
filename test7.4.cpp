// lab: sharing file-backed pages

#include "vm_app.h"
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include <cstring>
#include <iostream>

int main() {
    /* Allocate swap-backed page from the arena */
    char *filename = (char *) vm_map(nullptr, 0);
    /* Write the name of the file that will be mapped */
    strcpy(filename, "lampson83.txt");

    /* Map a page from the specified file */
    char *p0 = (char *) vm_map (filename, 0);

    char *swap1 = (char *) vm_map(nullptr, 0);
    swap1[0] = '1';
    char *swap2 = (char *) vm_map(nullptr, 0);  // first swap evicted
    swap2[0] = '2';

    std::cout << p0[5] << '\n';
    std::cout << p0[3] << '\n';
    p0[0] = 'x';
    strcpy(p0+5, "hey jude, don't make it bad");

    std::cout << p0[5] << '\n';
    std::cout << filename[0] << '\n';
    p0[0] = 'y';
    std::cout << swap1[0] << '\n';

    std::cout << filename[0] << '\n';
    std::cout << swap1[0] << '\n';
    std::cout << swap2[0] << '\n';  // p0 evicted

    strcpy(p0+5, "nananananananananananananana");

    if (fork()) { // parent
        char *parent = (char *) vm_map (filename, 0);
        std::cout << parent[5] << '\n';
        char *swap_parent0 = (char *) vm_map(nullptr, 0);
        std::cout << swap_parent0[0];
        char *swap_parent1 = (char *) vm_map(nullptr, 0);
        std::cout << swap_parent1[0];
        char *swap_parent2 = (char *) vm_map(nullptr, 0);
        std::cout << swap_parent2[0];
        char *swap_parent3 = (char *) vm_map(nullptr, 0);
        std::cout << swap_parent3[0];
        std::cout << parent[3] << '\n';
        strcpy(swap_parent0, "data1.bin");
        /* Map a page from the specified file */
        char *second_file = (char *) vm_map (swap_parent0, 0);
        second_file[5] = 'i';
    }
    else { // child
        char *fb_page = (char *) vm_map(filename, 0);
        assert(fb_page[0] == 'B');
        fb_page[0] = 'H';

        char *swap_child0 = (char *) vm_map(nullptr, 0);
        std::cout << swap_child0[0];
        char *swap_child1 = (char *) vm_map(nullptr, 0);
        std::cout << swap_child1[0];
        char *swap_child2 = (char *) vm_map(nullptr, 0);
        std::cout << swap_child2[0];
        strcpy(swap_child2, "data1.bin");

        /* Map a page from the specified file */
        char *another_file = (char *) vm_map (swap_child2, 0);
        std::cout << another_file[5];
        another_file[5] = 'o';
    }
}
