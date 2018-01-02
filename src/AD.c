#include <xc.h>
#include "IIC.h"
#include "AD.h"
unsigned char T_Chel[4]={0b10000000,0b10100000,0b11000000,0b11100000};
extern int Reg[32];
extern int Temp[4];
void write_ext_ads1110(unsigned char address, unsigned char config_data)
 {
    unsigned char k;    // Save ACKSTAT
    WCOL = 0;       // Clear Write Conflict Symbol Bit
B:   I2C_Start();
   k = I2C_Write(address);   //Transmit Address of Chip
   if(k == 0)
   {
       I2C_Stop();
       goto B;
   }
   k = I2C_Write(config_data);      //Set Configuration-bit
   if(k == 0)
   {
       I2C_Stop();
       goto B;
   }
   I2C_Stop();
   __delay_ms(1);
}

unsigned int read_ext_ads1110(unsigned char address)
{
   unsigned int data;
   unsigned char k;

   WCOL = 0;       // Clear Write Conflict Symbol Bit
A:   I2C_Start();
   k = I2C_Write((address + 1));
   if(k == 0)
   {
       I2C_Stop();
       goto A;
   }
   __delay_ms(5);     // sampling rate :15sps ==> Each time sampling need 67ms,So setting 100ms
   data = I2C_Read(0);
   data = data << 8;     
   data = data + I2C_Read(0);
   SSPOV = 0;          //  Clear Recieve Overflow Symbol bit
   I2C_Read(0);     // It is OK without this statement !
   I2C_Stop();
   __delay_ms(1);
   return(data);
}
void write_ext_m3424(unsigned char config_data)
 {
    unsigned char k;         // Save ACKSTAT
    WCOL = 0;       // Clear Write Conflict Symbol Bit
B:   I2C_Start();
   k = I2C_Write(0b11010000);        //Transmit Address of Chip
   if( k == 0)
   {
       I2C_Stop();
       goto B;
   }
   k = I2C_Write(config_data);           //Set Configuration-bit
   if( k == 0)
   {
       I2C_Stop();
       goto B;
   }
   I2C_Stop();
   __delay_ms(1);
}

unsigned int read_ext_m3424()
{
   unsigned int data;
   unsigned char k;
   WCOL = 0;       // Clear Write Conflict Symbol Bit
A:   I2C_Start();
   k = I2C_Write(0b11010001);
   if( k == 0)
   {
       I2C_Stop();
       goto A;
   }
   __delay_ms(5);        // sampling rate :240sps
   data = I2C_Read(0);
   data = data << 8;
   data = data + I2C_Read(0);
   SSPOV = 0;          //  Clear Recieve Overflow Symbol bit
   I2C_Read(0);     // It is OK without this statement !
   I2C_Stop();
   __delay_ms(1);
   return(data); 
}

void Sample_Volt(void)
{
     char address = 0b10010010;
     write_ext_ads1110(address ,0b10010000);	// Set Bits of Cofiguration
     Reg[1]= read_ext_ads1110(address);	// Sampling,unit:1mV
     Reg[1] = Reg[1]/2.98;
}
void  Sample_Cur(void)
{
     char address = 0b10010000;
     write_ext_ads1110(address ,0b10010000);	// Set Bits of Cofiguration
     Reg[2] = read_ext_ads1110(address);	// Sampling,unit:1mV
     if(Reg[2]==0xffff)
         Reg[2]=0;
}
void Sample_Temp(void)
{
    write_ext_m3424(T_Chel[0]);
    Reg[4] = read_ext_m3424(); //0.1?
    Temp[0] = Reg[4];
    write_ext_m3424(T_Chel[1]);
    Reg[5] = read_ext_m3424();
    Temp[1] = Reg[5];
    write_ext_m3424(T_Chel[2]);
    Reg[6] = read_ext_m3424();
    Temp[2] = Reg[6];
    write_ext_m3424(T_Chel[3]);
    Reg[27] = read_ext_m3424();
    Temp[3] = Reg[7];
}