// file-back page life cycle (state diagram)
// clock algorithm

#include "vm_app.h"
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>

int main()
{
    std::vector<char*> char_ps;
    char *swap1 = (char *) vm_map(nullptr, 0);
    swap1[0] = '1';
    char *swap2 = (char *) vm_map(nullptr, 0);
    swap2[0] = '2';
    char *swap3 = (char *) vm_map(nullptr, 0);
    swap3[0] = '3';
    char *swap4 = (char *) vm_map(nullptr, 0);
    swap4[0] = '4';
    char_ps.push_back(swap1);
    char_ps.push_back(swap2);
    char_ps.push_back(swap3);
    char_ps.push_back(swap4);
    for (int i = 0; i < 10; i++) {
        std::cout << char_ps[i % 4][0] << "\n";
    }
}
