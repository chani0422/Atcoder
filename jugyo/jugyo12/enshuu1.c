#include <stdio.h>
#include <string.h>

void sub(char *a, char *b, char *c) {
    char d[100] = {""};
    strcat(d,a);
    if(strcmp(a,b) != 0) strcat(d,b);
    // printf("%s1 : \n", d);
    if(strcmp(a,c) != 0 && strcmp(b,c) != 0) strcat(d,c);
    printf("%s\n", d);
    return;
}

int main() {
    sub("Japan", "France", "China");
    sub("Japan", "Japan", "China");
    sub("Japan", "France", "Japan");
    sub("Japan", "Japan", "Japan");
    return 0;
}