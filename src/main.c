/*
 * File:   main.c
 * Author: vincewl
 *
 * Created on November 15, 2017, 3:38 PM
 */
#define _XTAL_FREQ 20000000
// 2017/12/6
// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

#include <xc.h>
#include "uart.h"
//#include <pic16f877a.h>

//unsigned char *buf;
unsigned char buf[50];
unsigned int Reg[32];
unsigned char len;
void UartAction(unsigned char *buf, unsigned char len);
unsigned int GetCRC16(unsigned char *ptr,  unsigned char len);
const unsigned int wCRCTalbeAbs[] =
{
0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401, 0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400, 
};
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
 void main(void) {
    for(char i=1;i<33;i++)
    {
        Reg[i] = i;
    }
    TRISD=0xfe;
    RD0=0;
    TRISB3=0;
    RB3=1;
    UART_Init(9600);
    RCIF=0;
    GIE=1;
    PEIE=1;
    RCIE=1;
    while(1)
    {
        RD0=~RD0;
       __delay_ms(500);
    }
    return;
}

void interrupt ISR()
{
    if(RCIF)
    {
        RCIE=0;
        UART_Read_Text2(buf,8);
        //UART_Read_Text(buf2,8);
        RB3=0;
        UartAction(buf,8);
        RD0=~RD0;
        RB3=1;
        RCIE=1;
    }
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
                    buf[len++] = 0x00; //register high byte.
                    buf[len++] = Reg[i++]; //register low byte.
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