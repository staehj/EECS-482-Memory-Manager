// lab: swap-backed and file-backed random

#include "vm_app.h"
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include <cstring>
#include <iostream>

int main() {
    // map two swap-backed pages
    char *page0 = (char *) vm_map(nullptr, 0);
    char *page1 = (char *) vm_map(nullptr, 0);

    // write the filename into virtual memory
    char *filename = page0 + VM_PAGESIZE - 4;
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

    std::cout << p0[5] << '\n';

    std::cout << filename[0] << '\n';
    std::cout << swap1[0] << '\n';
    std::cout << swap2[0] << '\n';  // p0 evicted
}
