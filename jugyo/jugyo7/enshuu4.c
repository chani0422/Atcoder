#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char buffer[100], *str;

    scanf("%s", buffer);
    int len = strlen(buffer);

        //メモリ確保
    str = (char *)malloc(sizeof(char) * len);
    if(!str) {
        printf("メモリが確保できませんでした。\n");
        exit(1);
    }

    strcopy(str, buffer);
    // for(int i = 0; i < len; i++) {
    //     str[i] = buffer[i];
    // }

    printf("%s\n",str);

    //メモリ解放
    free(str);
    
    return 0;
}