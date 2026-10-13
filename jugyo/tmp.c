#include <stdio.h>

int main(void) {
    double X, time1, MIPS;
    do {
        scanf("%lf", &X);
    } while ((X < 1.0) || (X > 4.0));

    time1 = (100*2 + 200*1 + 500*1) * (1.0 / X)
          + (100 * (20.0 / 100.0)) *20;

    MIPS = (100 + 200 + 500) / (time1*0.000000001) / 1000000;

    printf("%f nsec, MIPS=%f\n", time1, MIPS);
    return 0;
}