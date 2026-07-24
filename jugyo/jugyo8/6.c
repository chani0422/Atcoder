#include <stdio.h>
#include <string.h>
#define L 20

int main(void) {
    char str[L], tmp;
    int len, i;

    printf("文字列を入力せよ： ");
    scanf("%s",str);
    len = strlen(str);
    for(int i = len-1; i >= 0; i--) {
        printf("%c",str[i]);
    }
    return 0;
}