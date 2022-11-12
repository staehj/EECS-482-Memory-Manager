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

    // Map a page from the specified file */
    char *p0 = (char *) vm_map (filename, 0);
    char *p1 = (char *) vm_map (filename, 1);
    char *p2 = (char *) vm_map (filename, 2);

    // specify new filename within lampson83.txt
    p0[5] = 'd';
    p0[6] = 'a';
    p0[7] = 't';
    p0[8] = 'a';
    p0[9] = '1';
    p0[10] = '.';
    p0[11] = 'b';
    p0[12] = 'i';
    p0[13] = 'n';
    p0[14] = '\0';

    // map new file from new filename
    char *p3 = (char *) vm_map (&p0[5], 0);

    p3[0] = 'h';
    p3[1] = 'e';
    p3[2] = 'l';
    p3[3] = 'l';
    p3[4] = 'o';

    // overwrite p0
    p0 = (char *) vm_map (&p0[5], 0);

    /* Print the first part of the paper */
    for (unsigned int i=0; i<1937; i++) {
	    std::cout << p0[i];
	    std::cout << p1[i];
	    std::cout << p2[i];
    }
}
