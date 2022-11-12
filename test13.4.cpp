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

    // map a file-backed page
    char *page2 = (char *) vm_map(filename, 0);
}