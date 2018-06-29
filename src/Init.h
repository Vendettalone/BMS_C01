/* 
 * File:   Init.h
 * Author: vincewl
 *
 * Created on December 14, 2017, 2:15 PM
 */

#ifndef _XTAL_FREQ
#define	_XTAL_FREQ 20000000

#endif	/* INIT_H */

void Init_IO(void);
void Init_IIC(void);
void Init_UART(void);
void Init_Timer1_100ms(void);
unsigned char ReadEEPROM(unsigned char address);
void WriteEEPROM(unsigned char address, unsigned char datas);
void Init_Timer2_10ms(void);
