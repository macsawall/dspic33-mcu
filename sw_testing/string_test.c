//counts characters in a string

#include <stdio.h>

char word[] = "Mackenzie";

int main(void)
{ 
  int i = 0;
  while(word[i] != '\0')
  {
    i++;
  }

  printf("%d\n", i);

  return 0;
}