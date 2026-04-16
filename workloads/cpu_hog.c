#include <stdio.h>
#include <unistd.h>

int main() {
    long long i = 0;
    while (1) {
        i++;
        if (i % 100000000 == 0) {
            printf("cpu_hog alive: %lld\n", i);
            fflush(stdout);
        }
    }
    return 0;
}
