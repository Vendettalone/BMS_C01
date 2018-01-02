/* 
 * File:   IIC.h
 * Author: vincewl
 *
 * Created on December 14, 2017, 4:49 PM
 */

#ifndef _XTAL_FREQ
#define	_XTAL_FREQ 20000000

#endif	/* INIT_H */

void I2C_Master_Init(const unsigned long c);
void I2C_Master_Wait();
void I2C_Master_Start();
void I2C_Master_RepeatedStart();
void I2C_Master_Stop();
void I2C_Master_Write(unsigned d);
unsigned short I2C_Master_Read(unsigned short a);