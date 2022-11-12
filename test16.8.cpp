// test from lab 8 (1)
#include "vm_app.h"
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include <cstring>

int main() {
    /* 4 pages of physical memory in the system */
    char* page1 = (char*) vm_map(nullptr, 0);
    if (fork()) {
        vm_yield();
    }
    else {
        page1[0] = 'a';
    }
}
