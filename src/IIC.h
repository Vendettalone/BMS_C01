/* 
 * File:   IIC.h
 * Author: zql
 *
 * Created on December 14, 2017, 4:49 PM
 */

void Init_I2C(void);
void I2C_Start(void);
void I2C_RptStart(void);
void I2C_Stop(void);
unsigned short I2C_Read( unsigned char ack );
unsigned char I2C_Write( unsigned char I2CWriteData );