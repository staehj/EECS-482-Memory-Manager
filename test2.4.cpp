// swap-basked: random test case

#include "vm_app.h"
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include <cstring>

int main() {
    /* 4 pages of physical memory in the system */
    if (fork()) { // parent
        char *page0 = (char *) vm_map(nullptr, 0);
        char *page1 = (char *) vm_map(nullptr, 0);
        char *page2 = (char *) vm_map(nullptr, 0);
        char *page3 = (char *) vm_map(nullptr, 0);
        char *page4 = (char *) vm_map(nullptr, 0);
        char *page5 = (char *) vm_map(nullptr, 0);
        char *page6 = (char *) vm_map(nullptr, 0);
        char *page7 = (char *) vm_map(nullptr, 0);
        char *page8 = (char *) vm_map(nullptr, 0);
        char *page9 = (char *) vm_map(nullptr, 0);
        page0[0] = page1[0] = page2[0] = 'a';
        page3[0] = page4[0] = page5[0] = 'b';
        page6[0] = page7[0] = page8[0] = page9[0] = 'c';
        page0[0] = 'x';
        page9[0] = 'x';
        page2[0] = 'x';
        page5[0] = 'x';
        page8[0] = 'x';
        page4[0] = 'x';
        page1[0] = 'x';
        page6[0] = 'x';
        page7[0] = 'x';

        page0[0] = 'y';
        page1[0] = 'y';
        page2[0] = 'y';
        page3[0] = 'y';
        page4[0] = 'y';
        page5[0] = 'y';
        page6[0] = 'y';
        page7[0] = 'y';
        page8[0] = 'y';

        page0[0] = page9[0];
        page9[0] = page8[0];
        page2[0] = page7[0];
        page5[0] = page6[0];
        page8[0] = page5[0];
        page4[0] = page4[0];
        page1[0] = page3[0];
        page6[0] = page2[0];
        page7[0] = page1[0];
    }
    else { // child
        char *page0 = (char *) vm_map(nullptr, 0);
        strcpy(page0, "Hello, world!");
    }
}
