#include <xc.h>
#include "IIC.h"
#include "AD.h"
#include "I2c.h"
unsigned char T_Chel[4]={0b10000000,0b10100000,0b11000000,0b11100000};
extern int Reg[32];
extern int Temp[4];
void Sample_Volt(void)
{
        I2CStart();
        I2CSend(0b10010010);
        I2CSend(0b10010000);
        I2CStop();
        __delay_ms(5);
        I2CStart();
        I2CSend(0b10010011);
        Reg[1] = I2CRead();
        I2CAck();
        Reg[1]<<=8;
        //Reg[1]&=0x8fff;
        Reg[1]=Reg[1]+I2CRead(); //1mv
        I2CNak();
        I2CStop();
        __delay_ms(1);
        //Reg[1]=Reg[1]/100*35; //100mV
}
void Sample_Cur(void)
{
        I2CStart();
        I2CSend(0b10010000);
        I2CSend(0b10010000);
        I2CStop();
        __delay_ms(5);
        I2CStart();
        I2CSend(0b10010001);
        Reg[2] = I2CRead();
        I2CAck();
        Reg[2]<<=8;
        //Reg[1]&=0x8fff;
        Reg[2]=Reg[2]+I2CRead(); //1mv
        I2CNak();
        I2CStop();
        __delay_ms(1);
}
void Sample_Temp(void)
{
    for(char i=0;i<4;i++)
    {
        I2CStart();
        I2CSend(0b11010000);
        I2CSend(T_Chel[i]);
        I2CStop();
        __delay_ms(5);
        I2CStart();
        I2CSend(0b11010001);
        Reg[4+i]=I2CRead();
        I2CAck();
        Reg[4+i]<<=8;
        Reg[4+i]=Reg[4+i]+I2CRead();
        I2CNak();
        I2CStop();
        Temp[i]=Reg[4+i];
        __delay_ms(1);
    }
}
