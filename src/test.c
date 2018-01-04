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
#pragma config BOREN = OFF       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

#include <xc.h>
#include "Init.h"
#include "uart.h"
#include "IIC.h"
#include "I2C.h"
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
unsigned char Timer2_Counter=0,Timer2_Counter_Set=100;
unsigned char Timer1_Counter=0,Timer1_Counter_Set=10;
void Get_Error(void);


 void main(void) {
     
    //Init_IO();
    I2CInit(100000);
    while(1)
    {
        /*
        I2CStart();
        I2CSend(0b10010010);
        I2CSend(0b10010000);
        I2CStop();
        __delay_ms(5);
        I2CStart();
        I2CSend(0b10010011);
        buf[1] = I2CRead();
        I2CAck();
        buf[2]=I2CRead(); //1mv
        I2CAck();
        buf[3]=I2CRead();
        I2CAck();
        I2CStop();
        Reg[1]=buf[1];
        Reg[1]<<=8;
        Reg[1]=Reg[1]+buf[2];
        //Reg[1]=Reg[1]/29.8; //100mV
        */
        TRISD0=0;
        RD0=~RD0;
        Sample_Volt();
        Sample_Cur();
        __delay_ms(500);
        Sample_Temp();    
    }
    return;
}