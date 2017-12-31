#include <xc.h>
#include "control.h"

void Relay_Fan(char s)
{
  if(s==1)
    RD7=1;
  else
    RD7=0;
}
void Relay_Gy(char s)
{
  if (s==1)
    RB5=1;
  else
    RB5=0;
}
void LED2(char s)
{
  if (s==1)
    RD0=0;
  else
    RD1=1;
}
void PCD(char channel,char s)
{
  if(channel<1||channel>5)
    return;
  switch (channel)
  {
    case 1:
      if (s==1)
      RD6=0;
      else RD6=1;
      break;
    case 2:
      if (s==1)
      RD5=0;
      else RD5=1;
      break;
    case 3:
      if (s==1)
      RD4=0;
      else RD4=0;
      break;
    case 4:
      if (s==1)
      RD3=0;
      else RD3=0;
      break;
    case 5:
      if (s==1)
      RD2=0;
      else RD2=0;
      break;      
  }
  
}
