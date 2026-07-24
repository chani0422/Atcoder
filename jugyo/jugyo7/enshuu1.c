#include <stdio.h>
#include <stdlib.h>

int main(void) {
    double *array;
    int num, i;

    //配列のサイズ
    printf("配列のサイズ： ");
    scanf("%d", &num);

    //メモリ確保
    array = (double *)malloc(sizeof(double) * num);
    if(!array) {
        printf("メモリが確保できませんでした。\n");
        exit(1);
    }

    for(i = 0; i < num; i++) {
        printf("%d個目の値： ", i+1);
        scanf("%lf",&array[i]);
    }

    //出力  
    for(i = 0; i < num; i++) {
        printf("%d個目の値： %lf\n", i+1,array[i]);
    }

    //メモリ解放
    free(array);

    return 0;
}