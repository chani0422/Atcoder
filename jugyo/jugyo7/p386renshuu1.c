#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Person{
    int age;
    double weight;
    double height;
} Person;

int main(void) {
    Person p1,p2;
    scanf("%d %lf %lf", &p1.age, &p1.height,&p1.weight);
    scanf("%d %lf %lf", &p2.age, &p2.height,&p2.weight);

    printf("年齢%d体重%lf身長%lfです。\n",p1.age,p1.height,p1.weight);
    printf("年齢%d体重%lf身長%lfです。\n",p2.age,p2.height,p2.weight);
    return 0;
}