#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int n;
    scanf("%d", &n);
    int *array;

        //メモリ確保
    array = (double *)malloc(sizeof(double) * n);
    if(!array) {
        printf("メモリが確保できませんでした。\n");
        exit(1);
    }

    for(int i = 0; i < n; i++) {
        scanf("%d", &array[i]);
    }

    int *arr = array;
    int gu = 0, ki = 0;
    for(int i = 0; i < n; i++) {
        if(*arr %2) ki++;
        else gu++;
        arr++;
    }

    printf("偶数：%d, 奇数：%d\n", gu, ki);

    //メモリ解放
    free(array);
    
    return 0;
}