#include <stdio.h>
#include <stdlib.h>
#define N 6

typedef struct Computer {
  int num;
  struct Computer *next ,*prev;
} Computer; 

int main(void) {
  Computer *p, *header = NULL, *tmp, *pre;
  int num;
  double freq;
  
  p=(Computer *)malloc(sizeof(Computer) * N); 
  p=NULL;  

int max = -10000;
  for(int i = 0; i < N; i++) {
    int value; scanf("%d", &value);
    if(max < value) max = value;
    tmp->num = value;
    tmp->prev = NULL;
    tmp->next = NULL;
    if(header == NULL) {
        header = tmp;
    }else {
        p->next = tmp;
  }
  pre = tmp;
}

p = header;
while(p->num != max) p = p->next;

  if(p->prev != NULL) p-> prev->next = p->next;
  if(p->next != NULL) p->next->prev = p->prev;
  if(header == p) header = p->next;
    p->num = num;
    p->next = NULL;

    

  return 0;
}