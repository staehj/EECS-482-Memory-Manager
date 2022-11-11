// lab: swap-backed page lifetime

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
        page0[0] = page1[0] = page2[0] = 'a';
    }
    else { // child
        char *page0 = (char *) vm_map(nullptr, 0);
        char *page0 = (char *) vm_map(nullptr, 0);
        char *page1 = (char *) vm_map(nullptr, 0);
        char *page2 = (char *) vm_map(nullptr, 0);
        page0[0] = page1[0] = page2[0] = 'a';
        strcpy(page0, "Hello, world!");
        /* 4 pages of physical memory in the system */
        if (fork()) { // parent
            char *page0 = (char *) vm_map(nullptr, 0);
            char *page1 = (char *) vm_map(nullptr, 0);
            char *page2 = (char *) vm_map(nullptr, 0);
            page0[0] = page1[0] = page2[0] = 'a';
        }
        else { // child
            char *page0 = (char *) vm_map(nullptr, 0);
            char *page0 = (char *) vm_map(nullptr, 0);
            char *page1 = (char *) vm_map(nullptr, 0);
            char *page2 = (char *) vm_map(nullptr, 0);
            page0[0] = page1[0] = page2[0] = 'a';
            strcpy(page0, "Hello, world!");
        }
    }
}
