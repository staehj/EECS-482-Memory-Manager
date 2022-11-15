// lab: swap-backed and file-backed random

#include "vm_app.h"
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include <cstring>
#include <iostream>

int main()
{
    char* s1 = (char *) vm_map(nullptr, 0);
    char* s2 = (char *) vm_map(nullptr, 0);
    char* s3 = (char *) vm_map(nullptr, 0);
    char* s4 = (char *) vm_map(nullptr, 0);

    for (unsigned int i = 0; i < VM_PAGESIZE; ++i) {
        s1[i] = 'a';
        s2[i] = 'a';
        s3[i] = 'a';
    }

    char* file = (char *) vm_map(s1, 0);

    char* invalid = (char *) vm_map(file, 0);

}
