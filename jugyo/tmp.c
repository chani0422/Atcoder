#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 10 //配列の数

void print_a(int a[]){
    int i;
    for (i=0;i<N;i++)
        printf("%d ",a[i]);
    printf("\n");
}  

void swap(int a[],int i,int j){
    int tmp;
    tmp = a[i];
    a[i] = a[j];
    a[j] = tmp;
    print_a(a);
}


void sort(int a[]){
    //この関数を編集(レポートにもこの関数のみ掲載し説明)
    for(int i = 0; i < N-1; i++) {
        int min_index = i;
        for(int j = i+1; j < N; j++) {
            if(a[j] < a[min_index]) {
                min_index = j;
            }
        }
        swap(a,i,min_index);
    }
}

void main(void){
    int a[N]={0},i,tmp;
    srand((unsigned int)time(NULL));
    for (i=0;i<N;i++){
        a[i]=rand()%100;
    }
    printf ("Start:\n");
    print_a(a);

    sort(a);
    
    printf ("Finish:\n");
    print_a(a);
}