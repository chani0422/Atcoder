#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define N 10

int main(void) {
    FILE *fp;
    double tmp,cnt = 0;
    fp = fopen("num.txt", "r");
    if (fp == NULL) {
        printf("ファイルをオープンできません\n");
        exit(1);
    }
    
    int *v;
    //メモリ確保
    v = (int *)malloc(sizeof(int) * N);
    if(!v) {
        printf("メモリが確保できませんでした。\n");
        exit(1);
    }
    int i = 0;
    while(fscanf(fp, "%d",&v[i]) != EOF) {
        i++;
    };
    
    for(int j = 0; j < N; j++) {
        printf("%d ",v[j]);
    }
    printf("\n");
    
    free(v);
    fclose(fp);
    return 0;
}