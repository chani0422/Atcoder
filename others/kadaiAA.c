#include <stdio.h>

int main(void) {
    int a,b,c,d;
    scanf("%d %d", &a,&b);
    if(a > b) {
        c = a;
        d = b;
    } else {
        c = b;
        d = a;
    }
    if(d != 0) {
        int r = c % d;
        while(r != 0) {
            c = d;
            d = r;
            r = c % d;
        }
        printf("%dと%dの最大公約数は：%dです。\n",a,b,d);
    }else {
        printf("エラーです。\n");
    }
    return 0;
}