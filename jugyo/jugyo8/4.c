#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define N 5

int main(void) {
    int data[N] = {0,1,2,3,4};
    int *p;
    p = data;

    for(int i = 0; i < N; i++) {
        printf("%d ",*p);
        p++;
    }
    return 0;
}