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
    int n;
    scanf("%d",&n);

    Person *people;

    //メモリ確保
    people = (Person *)malloc(sizeof(Person) * n);
    if(!people) {
        printf("メモリが確保できませんでした。\n");
        exit(1);
    }

    for(int i = 0; i < n; i++) {
        scanf("%d %lf %lf", &people[i].age, &people[i].height,&people[i].weight);
        printf("年齢%d体重%lf身長%lf BMI%lf\n",people[i].age,people[i].weight,people[i].height,BMI(people[i]));
    }

    free(people);
    return 0;
}