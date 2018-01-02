#include <xc.h>
#include "IIC.h"
#include "AD.h"
unsigned char T_Chel[4]={0b10000000,0b10100000,0b11000000,0b11100000};
extern int Reg[32];
extern int Temp[4];
void Sample_Volt(void)
{
        I2C_Master_Start();
        I2C_Master_Write(0b10010010);
        I2C_Master_Write(0b10010000);
        I2C_Master_Stop();
        __delay_ms(5);
        I2C_Master_Start();
        I2C_Master_Write(0b10010011);
        Reg[1] = I2C_Master_Read(0);
        Reg[1]<<8;
        Reg[1]=Reg[1]+I2C_Master_Read(0); //1mv
        Reg[1]=Reg[1]/29.8; //100mV
        I2C_Master_Stop();
        __delay_ms(1);
}
void Sample_Cur(void)
{
        I2C_Master_Start();
        I2C_Master_Write(0b10010000);
        I2C_Master_Write(0b10010000);
        I2C_Master_Stop();
        __delay_ms(5);
        I2C_Master_Start();
        I2C_Master_Write(0b10010001);
        Reg[1] = I2C_Master_Read(0);
        Reg[1]<<8;
        Reg[1]=Reg[1]+I2C_Master_Read(0); //1mv
        I2C_Master_Stop();
        __delay_ms(1);
}
void Sample_Temp(void)
{
    for(char i=0;i<4;i++)
    {
        I2C_Master_Start();
        I2C_Master_Write(0b11010000);
        I2C_Master_Write(T_Chel[i]);
        I2C_Master_Stop();
        __delay_ms(5);
        I2C_Master_Start();
        I2C_Master_Write(0b11010001);
        I2C_Master_Read(0);
        Reg[4+i]=I2C_Master_Read(0);
        Reg[4+i]<<8;
        Reg[4+i]=I2C_Master_Read(0);
        Temp[i]=Reg[4+i];
        I2C_Master_Stop();
        __delay_ms(1);
    }
}
