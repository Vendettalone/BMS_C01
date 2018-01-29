#include "soc.h"
#include "Init.h"
void Get_soc(void)
{
  Quantity1=Quantity1-Reg[3];
  soc=Quantity1/Quantity0*100;
  Reg[2]=soc;
  WriteEEPROM(0x0f,soc);
}
		
