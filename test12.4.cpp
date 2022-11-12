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
    char *swap1 = (char *) vm_map(nullptr, 0);
    swap1[0] = '1';
    char *swap2 = (char *) vm_map(nullptr, 0);
    swap2[0] = '2';
    char *swap3 = (char *) vm_map(nullptr, 0);
    swap3[0] = '3';
    char *swap4 = (char *) vm_map(nullptr, 0);
    swap4[0] = '4';

    std::cout << swap1[0] << '\n';
    swap1[2] = '1';

    swap2[1] = '2';
    swap3[1] = '3';
    swap4[1] = '4';

    std::cout << swap1[2];

    swap4[2] = '4';

    swap1[3] = '1';

    swap2[1] = '2';
    swap3[1] = '3';
    swap4[1] = '4';

    std::cout << swap1[2];

    swap4[2] = '4';

    swap2[1] = '2';
    swap3[1] = '3';
    swap4[1] = '4';
}
