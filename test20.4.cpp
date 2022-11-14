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

    strcpy(s1+VM_PAGESIZE-3, "he");
}
