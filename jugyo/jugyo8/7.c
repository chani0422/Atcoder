#include <stdio.h>
#include <string.h>

typedef struct name_num{
    char name[20];
    int num;
} name_num;

void deisplay(char *str, int num) {
    printf("%s : %d", str, num);
}

int main(void) {
    name_num ME;
    scanf("%s %d", ME.name, &ME.num);
    deisplay(ME.name, ME.num);
    return 0;
}