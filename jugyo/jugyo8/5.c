#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void desc(int *a, int *b) {
    int tmp = *a;
    if(*a < *b) {
        *a = *b;
        *b = tmp;
    }
}

int main(void) {
    int a,b;

    scanf("%d %d", &a, &b);
    desc(&a, &b);
    printf("%d %d\n", a, b);
    return 0;
}