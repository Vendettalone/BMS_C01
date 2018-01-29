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
#include "soc.h"
//#include <pic16f877a.h>

//unsigned char *buf;
//this is a test
unsigned char buf[50];
int Reg[32];
const float Quantity0=150696;
float Quantity1=0;
char soc;
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
char string[]="hello world\n";
void main(void) {
    Init_IO();
    RB5=0;
    Reg[2]=ReadEEPROM(0x10);
    for(char i=1;i<8;i++)     //控制阈值读取
    {
        Reg[i+10]=ReadEEPROM(2*i-1)<<8;
        Reg[i+10]+=ReadEEPROM(2*i);
    }
    for(char i=8;i<14;i++)
    {
        Reg[i+12]=ReadEEPROM(2*i-1)<<8;
        Reg[i+12]+=ReadEEPROM(2*i);
    }
    I2CInit(100000);
    Set_ADS1110();
    while(!RB5)   //开机自检
    {
     Sample_Volt();
     Sample_Cur();
     //Sample_Volt();
     // Sample_Cur();
     // Sample_Cur();
     // Sample_Temp();
     // Sample_Temp();
     // Get_Error();
    }

    GIE=1;
    PEIE=1;
    //Init_Timer1_100ms();
    //TMR1IE=1;
    Init_Timer2_10ms();
    TMR2IE=1;
    
    UART_Init(19200);   
    RB3=1;
    RCIF=0;
    RCIE=1;
    RB3=0;
    while(1)
    {
        if(Timer2_Counter==Timer2_Counter_Set)
        {
            Timer2_Counter=0;
            RD0=~RD0;
           // HMI_Send();  //HMI测试
          //  HMI_New();   //HMI测试
           // Get_Error(); //错误处理
        }
       // if(Timer2_Counter%5==0)
        //{
        //   UART_SEND_PC();
        //}
            
    }
    return;                                                                                                                                                                                                                                                                                   
}

void interrupt ISR()
{
    if(TMR1IF)  
    {
        RCIE=0;
        TMR2IE=0;
        TMR1H=0x06;
        TMR1IF=0;
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
        Timer1_Counter++;
        TMR2IE=1;
        RCIE=1;
        RB3=1;
    }
    
    if(TMR2IF)   //10ms采样策略 ADS1110连续采样，每10ms采样一次；
    {           //MCP3424单次采样，每10ms转换并采集一次
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
        Get_soc();
        Reg[2]=Reg[2]/4.095;
        //UART_SEND_PC();     
        //RCIE=1;
    }
    if(RCIF)
    {
        RCIE=0;
        UART_Read_Text(buf,8);
        //CREN=0;
        //UART_Read_Text(buf2,8);
        RB3=0;
        UartAction(buf,8);
        RB3=1;
        RCIE=1;
       // CREN=1;
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
  //charge control
  if (Reg[2]>204)//current > 5A
    Relay(1);
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
