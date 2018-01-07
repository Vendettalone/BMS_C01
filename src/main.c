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
#include "I2C.h"
#include "AD.h"
#include "control.h"
//#include <pic16f877a.h>

//unsigned char *buf;
//this is a test
unsigned char buf[50];
int Reg[32];
int Temp[4]={0,0,0,0};
char ch=0;
unsigned char len;
unsigned char KJ_Flag=0;
unsigned char ErrorFlag1=0x00;
unsigned char ErrorFlag2=0x00;
unsigned char Timer2_Counter=0,Timer2_Counter_Set=50;
unsigned char Timer1_Counter=0,Timer1_Counter_Set=10;
void Get_Error(void);
void Set_Standard(void);
void Init_I2C(void);

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


   /* while(!RB5)
    {
     
     I2C_Master_Init(10000);
     //Init_I2C();
     Sample_Volt();
     // Sample_Volt();
     // Sample_Cur();
     // Sample_Cur();
     // Sample_Temp();
     // Sample_Temp();
     // Get_Error();
    }
    */
    I2CInit(10000);
    Set_ADS1110();
    GIE=1;
    PEIE=1;
    //Init_Timer1_100ms();
    //TMR1IE=1;
    Init_Timer2_10ms();
    TMR2IE=1;
    
    UART_Init(9600);
    RB3=1;
    RCIF=0;
    RCIE=1;
    while(1)
    {
        if(Timer2_Counter==Timer2_Counter_Set)
        {
            Timer2_Counter=0;
            RD0=~RD0;
           // Get_Error();
        }
            
    }
    return;
}

void interrupt Modbus()
{
    if(TMR1IF)
    {
        TMR2IE=0;
        TMR1H=0x06;
        TMR1IF=0;
        Timer1_Counter++;
        TMR2IE=1;
    }
    
    if(TMR2IF)
    {
        TMR2IF=0;
       // RCIE=0;
        Read_ADS1110();
        Read_MCP3424(ch);
        if(ch==3)
        {
            ch=0;
            Set_MCP3424(ch);
        }
        else
        {
            ch++;
            Set_MCP3424(ch);
        }
        Timer2_Counter++;
        //RCIE=1;
      
    }
    if(RCIF)
    {
        RCIE=0;
        UART_Read_Text(buf,8);
        //UART_Read_Text(buf2,8);
        RB3=0;
        UartAction(buf,8);
        RB3=1;
        RCIE=1;
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
void Set_Standard()
{
  Reg[11]=590;
  Reg[12]=620;
  Reg[13]=450;
  Reg[14]=550;
  Reg[15]=10000;
  Reg[16]=370;
  Reg[17]=350;
  Reg[20]=480;
  Reg[21]=400;
}
void Init_I2C()
{
  TRISC3=1;        
  TRISC4=1;

  SSPCON = 0x28;      
  SSPCON2 = 0x00;
  SSPADD = 0x0D;  

  CKE=1;   
  SMP=1;     

  SSPIF=0;      
  BCLIF=0;     
}
