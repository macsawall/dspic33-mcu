//counts characters in a string

#include <stdio.h>

void tx1(volatile unsigned char string[]);
void tx2(volatile unsigned char* string);
char first[] = "Mackenzie";
char* last = "Sawall";
char middle = 'L';
char byte;

int main(void)
{ 
  tx1(first);
  tx2(last);
  tx2((char*)middle);

  return 0;
}

void tx1(volatile unsigned char string[])
{
  printf("\n");	
  int i = 0;
  while(string[i] != '\0') //loop though string until all characters written
  {
    byte = string[i];
	printf("%c", byte);
    i++;
  }
  printf("\n");
}

void tx2(volatile unsigned char* string)
{
  printf("\n");	
  int i = 0;
  while(string[i] != '\0') //loop though string until all characters written
  {
    byte = string[i];
	printf("%c", byte);
    i++;
  }
  printf("\n");
}