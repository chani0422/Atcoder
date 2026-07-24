#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double cal(double t) {
    double c = 331.5 + 0.6*t;
    return c;
}

double hatyou(double c, double f) {
    double r = c/f;
    return r;
}

int main(void) {
    double t,f;
    scanf("%lf %lf",&t, &f);
    printf("%lf\n",hatyou(cal(t),f));
    return 0;
}