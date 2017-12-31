/*
 * File:   main.c
 * Author: vincewl
 *
 * Created on November 15, 2017, 3:38 PM
 */
#define _XTAL_FREQ 20000000
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
#include "Init.h"
#include "uart.h"
#include "IIC.h"
#include "AD.h"
#include "control.h"
//#include <pic16f877a.h>

//unsigned char *buf;
//this is a test
unsigned char buf[50];
int Reg[32];
int Temp[4]={0,0,0,0};
unsigned char len;
unsigned char KJ_Flag=0;
unsigned char ErrorFlag1=0x00;
unsigned char ErrorFlag2=0x00;
void Get_Error(void);


 void main(void) {
     
    Init_IO();
    RB5=0;
    for(char i=0;i<7;i++)
    {
        Reg[i+11]=ReadEEPROM(i);
    }
    for(char i=7;i<14;i++)
    {
        Reg[i+13]=ReadEEPROM(i);
    }
    Init_I2C();
    while(!RB5)
    {
      Sample_Volt();
      Sample_Volt();
      Sample_Cur();
      Sample_Cur();
      Sample_Temp();
      Sample_Temp();
      Get_Error();
    }
    GIE=1;
    PEIE=1;
    Init_Timer1_100ms();
    TMR1IE=1;
    
    UART_Init(9600);
    RB3=1;
    RCIF=0;
    RCIE=1;
    while(1)
    {
        Get_Error();
    }
    return;
}

void interrupt Modbus()
{
    if(RCIF)
    {
        RCIE=0;
        UART_Read_Text(buf,8);
        //UART_Read_Text(buf2,8);
        RB3=0;
        UartAction(buf,8);
        RD0=~RD0;
        RB3=1;
        RCIE=1;
    }
}
void interrupt Sample()
{
    if(TMR1IF)
    {
        TMR1H=0x06;
        TMR1IF=0;
        Sample_Volt();
        Sample_Cur();
        Sample_Temp();
    }
}
void Get_Error(void)
{   
  //voltage error
  if (Reg[1]>Reg[12])
  {
      Relay_Gy(0);
      ErrorFlag1|=0b00001100;
  }
  else if(Reg[1]>Reg[11])
      ErrorFlag1|=0b00000100;
  else if(Reg[1]<Reg[21])
  {
      Relay_Gy(0);
      ErrorFlag1|=0b00000011;
  }
  else if (Reg[1]<Reg[20])
      ErrorFlag1|=0b00000001;
  else
      ErrorFlag1&=0b11110000;
  //Temperature error
  int tmp;
  for(char i=0;i<3;i++)
      for(char j=0;j<i;j++)
      {
          if(Temp[j]>Temp[j+1])
          {
              Temp[j] = tmp;
              Temp[j] = Temp[j+1];
              Temp[j+1] = tmp;
          }
              
      }
  if (Temp[3]>Reg[14])
  {
      Relay_Gy(0);
      ErrorFlag1 |= 0b11000000;
  }
  else if (Temp[3]>Reg[13])
      ErrorFlag1 |= 0b01000000;
  if (Temp[3]>Reg[16])
      Relay_Fan(1);
  else if (Temp[3]<Reg[17])
      Relay_Fan(0); 
  //power on
  if (ErrorFlag1 && 0b10101010)
      Relay_Gy(0);
  else
      Relay_Gy(1);
}
