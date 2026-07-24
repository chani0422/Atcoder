#include <stdio.h>
#include <stdlib.h>

typedef struct CELL {
    struct CELL *next;
    int unit;
    int price;
} CELL;

int main() {
    CELL *p, *x;
    CELL *header = NULL;
    int total;

    p = (CELL *)malloc(sizeof(CELL));
    p->unit = 20;
    p->price = 100;
    x = p;
    header = p;
    p = (CELL *)malloc(sizeof(CELL));
    p->unit = 19;
    p->price = 50;
    x->next = p;
    x = p;
    p = (CELL *)malloc(sizeof(CELL));
    p->unit = 40;
    p->price = 200;
    p->next = NULL;
    x->next = p;

    total = 0;
    p = header;
    while(p != NULL) {
        printf("%d, %d\n", p->unit, p->price);
        total += p->price;
        p = p->next;
    }
    printf("total = %d\n", total);
    return 0;
}