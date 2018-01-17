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
        I2CSend(0b10010001);
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
void Set_ADS1110(void)
{
    I2CStart();
    I2CSend(0b10010010);//ED1
    I2CSend(0b10000000);
    I2CStop();
    I2CStart();
    I2CSend(0b10010000);//ED0
    I2CSend(0b10000000);//
    I2CStop();
}
void Read_ADS1110(void)
{
    I2CStart();
    I2CSend(0b10010011);
    Reg[1] = I2CRead();
    I2CAck();
    Reg[1]<<=8;
    //Reg[1]&=0x8fff;
    Reg[1]=Reg[1]+I2CRead(); //1mv
    I2CNak();
    I2CStop();
    Reg[1]=Reg[1]/2.52; //实际电压，单位0.1V。计算by 电阻分压
    
    I2CStart();
    I2CSend(0b10010001);
    Reg[2] = I2CRead();
    I2CAck();
    Reg[2]<<=8;
    Reg[2]=Reg[2]+I2CRead(); //1mv
    I2CNak();
    I2CStop();
    Reg[2]=Reg[2]/3.95; //实际电流，单位0.1A。计算by AD602增益只与RG相关，OP07仅做电压跟随器.
}

void Set_MCP3424(char ch)
{
     I2CStart();
     I2CSend(0b11010000);
     I2CSend(T_Chel[ch]);
     I2CStop();
}
void Read_MCP3424(char ch)
{
    I2CStart();
    I2CSend(0b11010001);
    Reg[4+ch]=I2CRead();  //10mV/℃，AD值正好与0.1℃单位温度值相同
    I2CAck();
    Reg[4+ch]<<=8;
    Reg[4+ch]=Reg[4+ch]+I2CRead();
    I2CNak();
    I2CStop();
    Temp[ch]=Reg[4+ch];
}
