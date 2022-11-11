// lab: swap-backed page lifetime

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

    p0[123] = p1[234];
    p1[123] = p1[657];
    p0[123] = p1[243];
    p1[123] = p0[28];

    /* Print the first part of the paper */
    for (unsigned int i=0; i<1937; i++) {
	    std::cout << p0[i];
	    std::cout << p1[i];
    }
}
