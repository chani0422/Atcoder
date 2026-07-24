#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    double n = 5, sum = 0,cnt = 0, ave;
    for(int i = 0; i < n; i++) {
        double a;
        scanf("%lf",&a);
        if(a < 0) break;
        sum += a;
        cnt++;
    }
    ave = sum/cnt;
    printf("%lf\n", ave);
    return 0;
}