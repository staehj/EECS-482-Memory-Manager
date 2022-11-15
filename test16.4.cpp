// stae

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

    strcpy(lamp1+10, "data1.bin");  // lamp1: ppn 2 (faults) (file_read lamp)

    char *data1 = (char *) vm_map (lamp1+10, 0);

    strcpy(data1+50, "data2.bin");  // lamp2: ppn 2 (no fault)

    char *data2 = (char *) vm_map (data1+50, 0);

    strcpy(data2+100, "data3.bin");  // data1: ppn 3 (faults) (file_read data1)

    char *data3 = (char *) vm_map (data2+100, 0);

    strcpy(data3+100, "data4.bin");  // data1: ppn 3 (faults) (file_read data1)

    char *data4 = (char *) vm_map (data3+100, 0);

    data4[0] = 'L';  // no fault, write succeeds

    std::cout<<data1[1]<<std::endl; // evict data2.bin and write to disk
    data1[1] = 'a'; // write fault on data1.bin

    std::cout<<data3[0]<<std::endl; // reference bits set from 0 to 1
    std::cout<<data4[0]<<std::endl; // reference bits set from 0 to 1

    std::cout<<lamp_name[0]<<std::endl; // evict data1.bin write to disk

    strcpy(lamp_name, "data4.bin"); 
    char *data44 = (char *) vm_map (lamp_name, 0);

    std::cout<<data2[0]<<std::endl; // evict data1

    strcpy(lamp_name, "data2.bin"); 
    char *data22 = (char *) vm_map (lamp_name, 0); // vm_map to file in file_table and in physmem
    data22[1] = 'a'; // set data2.bin ref=true, dirty=true, w=1

    std::cout<<data3[0]<<std::endl; // evict data1.bin
    std::cout<<data4[0]<<std::endl; // evict lampname and write to disk
    std::cout<<data1[1]<<std::endl; // evict data2.bin and write to disk

    data44[0] = 'a'; // set data4.bin dirty = true w = true
    data22[1] = 'a'; // evict data3.bin and don't write to disk
    std::cout<<lamp_name[0]<<std::endl; // evict data4.bin and write to disk
}
