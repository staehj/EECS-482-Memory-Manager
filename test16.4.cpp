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
    s1[0] = 'a';
    char *invalid2 = (char *) vm_map(s1, 0);

    if (invalid2 == nullptr) {
        printf("filename was invalid, no null character\n");
    }
    else {
        printf("filename was valid\n");
        if (invalid2[0] == -1) {
            printf("invalid2[0]: %c\n", invalid2[0]);
        }
        else {
            printf("read failed from invalid file\n");
        }
    }
}
