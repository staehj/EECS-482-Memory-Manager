// test from lab 8 (1)
#include "vm_app.h"
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include <cstring>
#include <iostream>

int main() {
    if (fork()) { // parent
        char *filename = (char *) vm_map(nullptr, 0);
        strcpy(filename, "lampson83.txt");
        char* file = (char*) vm_map(filename, 0);

        // read on file
        std::cout << file[5] << '\n';

        char *swap1 = (char *) vm_map(nullptr, 0);
        swap1[0] = '1';
        char *swap2 = (char *) vm_map(nullptr, 0);  // first swap evicted
        swap2[0] = '2';
    }
    else {
        char *swap1_child = (char *) vm_map(nullptr, 0);
        swap1_child[0] = '1';
        char *swap2_child = (char *) vm_map(nullptr, 0);
        swap2_child[0] = '2';
        char *swap3_child = (char *) vm_map(nullptr, 0);
        swap3_child[0] = '3';
        char *swap4_child = (char *) vm_map(nullptr, 0);
        swap4_child[0] = '4';
    }
}
