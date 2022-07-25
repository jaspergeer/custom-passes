#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int a = atoi(argv[1]), b = atoi(argv[2]);
    a = a * 128;
    b = 256 * b;
    int c = 2048 * (a + b);
    printf("the result is: %d\n", c);
    return 0;
}

int func() {
    return 1;
}