// lab: sharing file-backed pages

#include "vm_app.h"
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include <cstring>

int main() {
    /* 4 pages of physical memory in the system */
    char *filename = (char *) vm_map(nullptr, 0);
    strcpy(filename, "lampson83.txt");
    char *p0 = (char *) vm_map(nullptr, 0);
    char *p1 = (char *) vm_map(nullptr, 0);
    char *p2 = (char *) vm_map(nullptr, 0);
    char *p3 = (char *) vm_map(nullptr, 0);
    char *p4 = (char *) vm_map(nullptr, 0);
    char *p5 = (char *) vm_map(nullptr, 0);
    char *p6 = (char *) vm_map(nullptr, 0);
    char *p7 = (char *) vm_map(nullptr, 0);
    char *p8 = (char *) vm_map(nullptr, 0);

    if (fork()) { // parent
        char *fb_page = (char *) vm_map(filename, 0);
        fb_page[0] = 'B';
        vm_yield();
    }
    else { // child
        char *fb_page = (char *) vm_map(filename, 0);
        assert(fb_page[0] == 'B');
        fb_page[0] = 'H';
    }
}
