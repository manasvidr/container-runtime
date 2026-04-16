#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int size = 10 * 1024 * 1024; // 10MB
    char *mem;

    while (1) {
        mem = malloc(size);
        if (mem == NULL) {
            printf("Memory allocation failed\n");
            break;
        }

        printf("Allocated 10MB\n");
        sleep(1);
    }

    return 0;
}