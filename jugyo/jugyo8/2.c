#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double cal(double t) {
    double c = 331.5 + 0.6*t;
    return c;
}

int main(void) {
    double t;
    scanf("%lf %lf",&t);
    printf("%lf\n",cal(t));
    return 0;
}