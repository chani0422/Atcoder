#include <stdio.h>

typedef struct Project {
    int human;
    int money;
} Project;

int main() {
    Project alist[3] = {
        {50,20},
        {30,60},
        {40,30},
    };
    int i, maxv, maxi;

    maxv = 0;
    for(i = 0; i < 3; i++) {
        if(maxv < alist[i].human * alist[i].money) {
            maxv = alist[i].human * alist[i].money;
            maxi = i;
        }
    }
    printf("%d = %d man-yen x %d \n",alist[maxi].human*alist[maxi].money, alist[maxi].human,alist[maxi].money);
    return 0;
}