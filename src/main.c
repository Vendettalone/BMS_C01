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
//#include <pic16f877a.h>

//unsigned char *buf;
//this is a test
unsigned char buf[50];
int Reg[32];
unsigned char len;


 void main(void) {
     
    Init_IO();
    for(char i=0;i<7;i++)
    {
        Reg[i+11]=ReadEEPROM(i);
    }
    for(char i=7;i<14;i++)
    {
        Reg[i+13]=ReadEEPROM(i);
    }
    Init_I2C();
    Sample_Volt();
    Sample_Volt();
    Sample_Cur();
    Sample_Cur();
    Sample_Temp();
    Sample_Temp();
    
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
        UART_Read_Text(buf,8);
        //UART_Read_Text(buf2,8);
        RB3=0;
        UartAction(buf,8);
        RD0=~RD0;
        RB3=1;
        RCIE=1;
    }
}


