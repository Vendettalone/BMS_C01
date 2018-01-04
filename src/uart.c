#include <xc.h>
#include "uart.h"

unsigned int CRC16_1(char *buf,char len)
{
    unsigned int CRC16=0xFFFF;
    char chChar;
    for(char i=0;i<len;i++)
    {
        chChar=*buf++;
        CRC16^=chChar;
        for(char j=0;j<8;j++)
        {
            if(CRC16&0x0001)
            {
                CRC16=CRC16>>1;
                CRC16^=0xA001;
            }
            else
                CRC16=CRC16>>1;
        }
    }
    return CRC16;
}
unsigned int CRC16_2(char* pchMsg, char wDataLen)
{
        unsigned int wCRC = 0xFFFF;
        char i;
        char chChar;
        for (i = 0; i < wDataLen; i++)
        {
                chChar = *pchMsg++;
                wCRC = wCRCTalbeAbs[(chChar ^ wCRC) & 15] ^ (wCRC >> 4);
                wCRC = wCRCTalbeAbs[((chChar >> 4) ^ wCRC) & 15] ^ (wCRC >> 4);
        }

        return wCRC;
}
char UART_Init(const long int baudrate)
{
	unsigned int x;
	x = (_XTAL_FREQ - baudrate*64)/(baudrate*64);
	if(x>255)
	{
		x = (_XTAL_FREQ - baudrate*16)/(baudrate*16);
		BRGH = 1;
	}
	if(x<256)
	{
	  SPBRG = x;
	  SYNC = 0;
	  SPEN = 1;
      TRISC7 = 1;
      TRISC6 = 0;
      CREN = 1;
      TXEN = 1;
	  return 1;
	}
	return 0;
}

char UART_Read()
{
  if(OERR) // check for Error 
    {
        CREN = 0; //If error -> Reset 
        CREN = 1; //If error -> Reset 
    }
  while(!RCIF);
  
  return RCREG;
}

void UART_Read_Text(char buf[],unsigned int len)
{
    unsigned int i;
    for(i=0;i<len;i++)
    {
        buf[i]=UART_Read();
    }
}
void UART_Write(char data)
{
  while(!TRMT);
  TXREG = data;
}

void UART_Write_Text(char *text)
{
  int i;
  for(i=0;text[i]!='\0';i++)
	  UART_Write(text[i]);
}

void UartWrite(unsigned char buf[],unsigned char len)
{
    for(char i=0;i<len;i++)
        UART_Write(buf[i]);
}

void UartAction(unsigned char buf[], unsigned char len)
{
	unsigned char i;
	unsigned char cnt;
	unsigned char str[4];
	unsigned int crc;
	unsigned char crch, crcl;
	if (buf[0] != 0x01) //Device number check.
	{ 
		return; 
	}
	crc = CRC16_2(&buf[0], len-2); //CRC check.
	crch = crc >> 8;
	crcl = crc & 0xFF;
	if ((buf[len-2]!=crcl) || (buf[len-1]!=crch))
	{
		return; 
	}
	switch (buf[1])
	{
		case 0x03: //function number check
			if ((buf[2]==0x00)&&(buf[3]<=0x20)) //Register Address check.
			{
				i = buf[3]; //Register start address
				cnt = buf[5]; //Register number
				buf[2] = cnt*2; //Data byte number.
				len = 3; //Frame head byte length.
                while (cnt--)
                {
                    buf[len++] = Reg[i]>>8; //register high byte.
                    buf[len++] = Reg[i]; //register low byte.
                    i++;
                }
                break;
            }		
			else //Register Address error return.
			{
			    	buf[1] = 0x83; //high bit set to 1
			    	buf[2] = 0x02; //set address error code 02
			    	len = 3;
			    	break;
			}
		case 0x06: //function number check
			if ((buf[2]==0x00) && (buf[3]<=0x20)) //resister address check
			{
				i = buf[3]; //get Register Address
				Reg[i] = buf[5]; //Save Resiger Value
				len-=2;
                break;
			}
			else //Register Address error.
			{
				buf[1] = 0x86; //set high bit to 1=
                buf[2] = 0x02; //set register address error code 02
				len = 3;
				break;
			}
		default: //Other fuction code error.
            buf[1] |= 0x80; //set high bit to 1
			buf[2] = 0x01; //function error code 1
			len = 3;
			break;
	}
	crc = CRC16_2(&buf[0], len); //Get crc cdoe 
	buf[len++] = crc; //set crc low byte
	buf[len++] = crc >>8; //set crc high byte
	UartWrite(buf, len+1); //send
}