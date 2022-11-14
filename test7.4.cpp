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
    char *lamp_name = (char *) vm_map(nullptr, 0);

    /* Write the name of the file that will be mapped */
    strcpy(lamp_name, "lampson83.txt");  // lamp_name: ppn 1 (faults)

    // Map a page from the specified file */
    char *lamp1 = (char *) vm_map (lamp_name, 0);  // succeeds

    strcpy(lamp1+10, "lampson83.txt");  // lamp1: ppn 2 (faults) (file_read lamp)

    char *lamp2 = (char *) vm_map (lamp1+10, 0);

    strcpy(lamp2+50, "data1.bin");  // lamp2: ppn 2 (no fault)

    char *data1 = (char *) vm_map (lamp2+50, 0);

    strcpy(data1+100, "lampson83.txt");  // data1: ppn 3 (faults) (file_read data1)

    char *lamp3 = (char *) vm_map (data1+100, 0);

    lamp3[0] = 'L';  // no fault, write succeeds

    char* temp = (char *) vm_map(nullptr, 0);
    temp[0] = 't';  // evicts lamp_name, unreferences all lamp and data1 pages
                    // temp: ppn 1 (faults)

    char* data1b = (char *) vm_map(lamp2+50, 0);
                // lamp pages are unreferenced, so should fault to reference them
                // marks ppn3 (data1/b) as referenced

    char* temp2 = (char *) vm_map(nullptr, 0);
    temp2[0] = '2';  // evicts lamps (ppn2), everything else unreferenced
}
