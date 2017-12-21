/* 
 * File:   IIC.h
 * Author: zql
 *
 * Created on December 14, 2017, 4:49 PM
 */
#include <xc.h>
#include "IIC.h"

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

void I2C_Start()
{
 SEN=1;  
 while(SEN);  
}

//void I2C_RptStart()   // Repeat Start
//{
//  RSEN=1;  
//  while(RSEN);  
//}

void I2C_Stop()
{
  PEN=1;  
  while(PEN); 
}

unsigned short I2C_Read(unsigned char ack)
{
 unsigned char I2CReadData;

 RCEN=1;  
 while(RCEN);  

 I2CReadData = SSPBUF; 
 if ( ack )
 {
  ACKDT=0;
 }
 else
 {
  ACKDT=1;
 }
 ACKEN=1;           
 while(ACKEN);  
 return( I2CReadData );
}

unsigned char I2C_Write( unsigned char I2CWriteData )
{
 SSPBUF = I2CWriteData;
 while(SSPSTAT & 0b00000100);  
 return ( !ACKSTAT  ); 
}
