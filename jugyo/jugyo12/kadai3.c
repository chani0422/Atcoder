#include <stdio.h>
#include <string.h>

void change(char *s) {
    int len = strlen(s);
    char ns[25];
    char ans[25] = "";
    if(len < 2) printf("error\n");
    if(s[len-1] == 's' || (s[len-1] == 'h' && s[len-2] == 'c') || s[len-1] == 'x' || (s[len-1] == 'h' && s[len-2] == 's')) {
        strcat(s, "es");
        strcpy(ans, s);
    }else if(!(s[len-2] == 'a' || s[len-2] == 'i' || s[len-2] == 'u' || s[len-2] == 'e' || s[len-2] == 'o')) {
        if(s[len-1] == 'o') {
            strcat(s, "es");
            strcpy(ans, s);
        }else if(s[len-1] == 'y') {
            char tmp[25] = "";
            strcpy(tmp, s);
            tmp[len-1] = 'i';
            tmp[len] = 'e';
            tmp[len+1] = 's';
            strcpy(ans, tmp);
        }
    }
     if(s[len-1] == 'f' || (s[len-2] == 'f' && s[len-1] == 'e')) {
        char tmp[25] = "";
        strcpy(tmp, s);
        tmp[len-2] = 'v';
        tmp[len-1] = 'e';
        tmp[len] = 's';
        strcpy(ans, tmp);
    }else {
        strcpy(ans, s);
        ans[len] = 's';
    }
    printf("%s\n", ans);
    return;
};

int main() {
    for(int i = 0; i < 8; i++) {
    char ss[20] = "";
    scan
}
