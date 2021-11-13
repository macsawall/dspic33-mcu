
#include <stdio.h>

unsigned char lcd_memory[6][20];

//0 = history not displayed, about to be deleted
//1 = history not displayed
//2 = top
//3 = second from top
//4 = third from top
//5 = fourth from top (bottom)

//unsigned char lcd_character_count = 0;
//unsigned char space_count = 0;
//unsigned char line_count = 0;
//unsigned char next_line_now = 0;
unsigned int x = 2;
unsigned int y = 0;

unsigned char* string1 = "Mackenzie Lee Sawall hates the douche faguet who lives upstairs. ";
unsigned char* string2 = "The second floor bass bastard can literally go ahead and suck his own little pee pee.";

void store_string(unsigned char* string);
void print_LCD(void);

int main(void)
{ 

  store_string(string1);
  //store_string(string2);
  
  print_LCD();
  
  

  
  

  return 0;
}

void store_string(unsigned char* string)
{
  int a;
  int b;
  int k;
  int i=0;
  while(string[i] != '\0')
  {
    lcd_memory[x][y] = string[i];
    i++;
    y++;
    //lcd_character_count++;
    
    if(y == 20)
    {
      y=0;
      x++;
      
      if(x == 6)
      {
        x=5;
        
        //shift rows up once a fifth row is started
        for(k=0; k<20; k++)
        {
          lcd_memory[0][k] = lcd_memory[1][k];
        }
        
        for(k=0; k<20; k++)
        {
          lcd_memory[1][k] = lcd_memory[2][k];
        }
        
        for(k=0; k<20; k++)
        {
          lcd_memory[2][k] = lcd_memory[3][k];
        }  
        
        for(k=0; k<20; k++)
        {
          lcd_memory[3][k] = lcd_memory[4][k];
        }          

        for(k=0; k<20; k++)
        {
          lcd_memory[4][k] = lcd_memory[5][k];
        }  
        
        for(k=0; k<20; k++)
        {
          lcd_memory[5][k] = '\0';
        }          
      }
    }
  }
  
  //post-storage processing to remove unnecessary spaces and fix split-up words
  for(a=0; a<6; a++)
  {
    for(b=0; b<20; b++)
    {
      if(lcd_memory[a][b] != ' ')
    }
  }
}

void print_LCD(void)
{
  int a;
  int b;
  
  for(a=2; a<6; a++)
  {
    for(b=0; b<20; b++)
    {
      printf("%c", lcd_memory[a][b]);
    }
    printf("\n");
  }
}