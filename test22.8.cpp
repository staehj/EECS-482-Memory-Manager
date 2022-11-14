// lab: swap-backed and file-backed random

#include "vm_app.h"
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include <cstring>
#include <iostream>

int main()
{
    char* file_names = (char *) vm_map(nullptr, 0);
    strcpy(file_names, "lampson83.txt");
    strcpy(file_names+100, "data1.bin");

    char* lamp = (char *) vm_map(file_names, 0);

    char* data1 = (char *) vm_map(file_names+100, 0);

    if (fork()) { // parent
        char* p1 = (char *) vm_map(file_names, 0);
        strcpy(p1, "lampson83.txt");

        vm_yield();

        char* p2 = (char *) vm_map(nullptr, 0);
        strcpy(p2, "data1.bin");
        char* p3 = (char *) vm_map(nullptr, 0);
        strcpy(p3, "data2.bin");

        char* p4 = (char *) vm_map(p3, 0); // data1.bin
        strcpy(file_names+100, p4);  // data1.bin written to data.bin

        vm_yield();

        if (fork()) {
            vm_yield();
            char* g1 = (char *) vm_map(nullptr, 0);
            char* g2 = (char *) vm_map(nullptr, 0);

        }
        else {
            char* f1 = (char *) vm_map(nullptr, 0);
            char* f2 = (char *) vm_map(file_names, 0);
            char* f3 = (char *) vm_map(file_names+100, 0);
            strcpy(f1, file_names);

            vm_yield();
        }

        vm_yield();
    }
    else { // child
        char* c1 = (char *) vm_map(nullptr, 0);
        strcpy(c1, "data1.bin");
        char* c2 = (char *) vm_map(c1, 0);
        c2[0] = 'a';

        vm_yield();

        char* c3 = (char *) vm_map(nullptr, 0);
        std::cout << c3[0];

        vm_yield();

        c2[0] = 'b';
        char* c4 = (char *) vm_map(c1, 0);
        char* c5 = (char *) vm_map(nullptr, 0);
        strcpy(c5, "data4.bin");

        char* c6 = (char *) vm_map(c5, 1);
    }
}
