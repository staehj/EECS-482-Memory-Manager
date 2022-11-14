// more swap pages than blocks
#include "vm_app.h"
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>

int main() {
    /* 4 pages of physical memory in the system */
    if (fork()) { // parent
        for (int i = 0; i < 257; ++i) {
            printf("i: %i\n", i);
            char *temp = (char *) vm_map(nullptr, 0);
            if (temp != nullptr) {
                temp[0] = 'a';
            }
        }
        vm_yield();
    }
    else { // child
        char *fb_page = (char *) vm_map(nullptr, 0);
        fb_page[0] = 'H';
        if (fb_page == nullptr) {
            printf("fb_page is nullptr\n");
        }
        printf("fb_page: %p\n", fb_page);
        fb_page[1] = 'H'; ///

        if (true) {
            printf("what is happening\n");
        }
        // std::cout << fb_page[0] << std::endl;
        // std::cout << "after" << std::endl;
        printf("fb_page[0]: %c\n", fb_page[0]);


    }
}
