// lab: swap-backed and file-backed random

#include "vm_app.h"
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include <cstring>
#include <iostream>

int main()
{
    
    if (fork()) { // parent
        char* p1 = (char *) vm_map(nullptr, 0);
        strcpy(p1, "lampson83.txt");

        vm_yield();

        char* p2 = (char *) vm_map(nullptr, 0);
        strcpy(p2, "data1.bin");
        char* p3 = (char *) vm_map(nullptr, 0);
        strcpy(p3, "data2.bin");

        vm_yield();

        char* p4 = (char *) vm_map(p3, 0); // data1.bin

        vm_yield();

        char* p5 = (char *) vm_map(p2, 0); // data1.bin
        strcpy(p5+100, "hello world");
    }
    else { // child
        char* c1 = (char *) vm_map(nullptr, 0);
        strcpy(c1, "data1.bin");
        char* c2 = (char *) vm_map(nullptr, 0);
        c2[0] = 'a';

        vm_yield();

        char* c3 = (char *) vm_map(nullptr, 0);
        std::cout << c3[0];

        vm_yield();

        std::cout << c1[0];

        // child destroyed
    }

    if (fork()) {
        vm_yield();
        char* z1 = (char *) vm_map(nullptr, 0);  // data1
        char* z2 = (char *) vm_map(z1, 0);  // data2
        char* z3 = (char *) vm_map(z2, 0);  // data3

        vm_yield();
    }
    else {
        char* x1 = (char *) vm_map(nullptr, 0);
        strcpy(x1, "data1.bin");
        char* x2 = (char *) vm_map(x1, 0);
        strcpy(x2, "data2.bin");
        char* x3 = (char *) vm_map(x1, 0);
        strcpy(x3, "data3.bin");

        vm_yield();

        x1[0] = 'a';

        // child destroyed
    }

    char* v1 = (char *) vm_map(nullptr, 0);
    char* v2 = (char *) vm_map(nullptr, 0);
    char* v3 = (char *) vm_map(nullptr, 0);
    char* v4 = (char *) vm_map(nullptr, 0);
    char* v5 = (char *) vm_map(nullptr, 0);
    v3[0] = 'v';
    v5[0] = 'v';

    // parent destroyed
}
