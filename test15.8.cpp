// more swap pages than blocks
#include "vm_app.h"
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include <cstring>

int main() {
    /* 4 pages of physical memory in the system */
    if (fork()) { // parent
        for (int i = 0; i < 257; ++i) {
            printf("i: %i\n", i);
            char *temp = (char *) vm_map(nullptr, 0);
            temp[0] = 'a';
        }
        vm_yield();
    }
    else { // child
        char *fb_page = (char *) vm_map(nullptr, 0);
        fb_page[0] = 'H';
    }
}
