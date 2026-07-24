#include <stdio.h>
#include <stdlib.h>

typedef struct Computer {
  int num;
  double freq;
  struct Computer *next;
} Computer; 

int main(void){
  Computer *pcom;
  int num;
  double freq;
  
    pcom = (Computer *) malloc(sizeof(Computer) * 47);
    if(!pcom) {
        printf("メモリが確保できませんでした。\n");
        return 1;
    }

  scanf("%d", &num);
  scanf("%lf", &freq);
  pcom->num = num;
  pcom->freq = freq;
  printf("%d %lf", pcom->num, pcom->freq);
  return 0;
}