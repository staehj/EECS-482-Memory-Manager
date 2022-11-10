// lab: forking with non-empty arena

#include "vm_app.h"
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include <cstring>

int main() {
    char *page1 = (char *) vm_map(nullptr, 0);
    if (fork()) {
        printf("This is the parent!");
        vm_yield(); // Let the child run
    } else {
        printf("This is the child!");
        page1[0] = 'a'; // Copy-on-write
    }
}
