#include <stdio.h>
#include <stdlib.h>

int main(void){
  printf("RAND_MAX=%d\n",RAND_MAX);
  int data[7]={0,0,0,0,0,0,0};
  for (int i=0;i<10;i++){
    int tmp = rand()%6+1;
    printf("%d:%d\n",i,tmp);
    data[tmp]+=1;
  }
  printf("\n");
  for (int i=1;i<7;i++){
    printf("%d:%d\n",i,data[i]);
  }
}
