// lab: swap-backed and file-backed random

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
    strcpy(filename, "lampson83.txt");

    /* Map a page from the specified file */
    char *p0 = (char *) vm_map (filename, 0);
    char *p1 = (char *) vm_map (filename, 1);
    char *p2 = (char *) vm_map (filename, 2);

    p0[0] = 'a';

    /* Print the first part of the paper */
    for (unsigned int i=0; i<1937; i++) {
	    std::cout << p0[i];
	    std::cout << p1[i];
	    std::cout << p2[i];
    }
}
