// file-back page life cycle (state diagram)
// clock algorithm

#include "vm_app.h"
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include <cstring>
#include <iostream>

int main()
{
    /* Allocate swap-backed page from the arena */
    char *filename = (char *) vm_map(nullptr, 0);

    /* Write the name of the file that will be mapped */
    strcpy(filename, "invalid_file.txt");

    /* Map a page from the specified file */
    char *p0 = (char *) vm_map (filename, 0);

    char *swap1 = (char *) vm_map(nullptr, 0);
    swap1[0] = '1';
    char *swap2 = (char *) vm_map(nullptr, 0);
    swap2[0] = '2';
    std::cout << p0[0];
    char *swap3 = (char *) vm_map(nullptr, 0);
    swap3[0] = '3';
    char *swap4 = (char *) vm_map(nullptr, 0); 
    swap4[0] = '4';
}
