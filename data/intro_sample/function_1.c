#include <stdio.h>
void my_hello(char *name);

void my_hello(char *name){
  printf("Hello %s.\n",name);
}

int main(void){
  char name[] = "koarashi";
  my_hello(name);
  return 0;
}
