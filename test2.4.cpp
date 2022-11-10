// piazza@1046: parents forks child forks child's child

#include "vm_app.h"
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include <cstring>

int main(){
    if (fork()) {
        printf("in the parent process\n");
    }
    else {
        printf("in the child process\n");
        if (fork()) {
            printf("fork in child\n");
        }
        else {
            printf("child's child\n");
        }
    }
    return 0;
}
