// test from lab 8 (1)
#include "vm_app.h"
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include <cstring>

int main() { // main process
    if (fork()) { // parent process
        char *filename = (char *) vm_map(nullptr, 0);
        strcpy(filename, "lampson83.txt");
        char* file = (char*) vm_map(filename, 0);
        char *swap1 = (char *) vm_map(nullptr, 0);
        swap1[0] = '1';
        char *swap2 = (char *) vm_map(nullptr, 0);  // first swap evicted
        swap2[0] = '2';
    }
    else { // child process
        char *filename_child = (char *) vm_map(nullptr, 0);
        strcpy(filename_child, "lampson83.txt");
        char* file_child = (char*) vm_map(filename_child, 0);
        char *swap1_child = (char *) vm_map(nullptr, 0);
        swap1_child[0] = '1';
        char *swap2_child = (char *) vm_map(nullptr, 0);  // first swap evicted
        swap2_child[0] = '2';
    }
}
