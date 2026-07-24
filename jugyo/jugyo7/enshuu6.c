#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Person{
    int age;
    double weight;
    double height;
} Person;

double BMI(Person p) {
    double sintyou = p.height;
    double ans = p.weight/sintyou/sintyou;
    return ans;
}

int main(void) {
    Person p1,p2;
    scanf("%d %lf %lf", &p1.age, &p1.height,&p1.weight);
    scanf("%d %lf %lf", &p2.age, &p2.height,&p2.weight);

    printf("年齢%d体重%lf身長%lf BMI%lf\n",p1.age,p1.weight,p1.height,BMI(p1));
    printf("年齢%d体重%lf身長%lf BMI%lf\n",p2.age,p2.weight,p2.height,BMI(p2));
    return 0;
}