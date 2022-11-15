// lab: swap-backed and file-backed random

#include "vm_app.h"
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include <cstring>
#include <iostream>

int main()
{
    char* name = (char *) vm_map(nullptr, 0);
    strcpy(name, "lampson83.txt");

    char* file = (char *) vm_map(name, 0);
    char* file2 = (char *) vm_map(name, 0);
    file[VM_PAGESIZE-9] = 'l';
    file[VM_PAGESIZE-8] = 'a';
    file[VM_PAGESIZE-7] = 'm';
    file[VM_PAGESIZE-6] = 'p';
    file[VM_PAGESIZE-5] = 's';
    file[VM_PAGESIZE-4] = 'o';
    file[VM_PAGESIZE-3] = 'n';
    file[VM_PAGESIZE-2] = '8';
    file[VM_PAGESIZE-1] = '3';
    file[0] = '.';
    file[1] = 't';
    file[2] = 'x';
    file[3] = 't';
    file[4] = '\0';

    char* temp1 = (char *) vm_map(nullptr, 0);
    temp1[0] = 'a';
    char* temp2 = (char *) vm_map(nullptr, 0);
    temp2[0] = 'b';
    char* temp3 = (char *) vm_map(nullptr, 0);
    temp3[0] = 'c';
    char* file3 = (char *) vm_map(file+VM_PAGESIZE-9, 0);
    std::cout << file[0];
    file2[0] = 'l';
}
