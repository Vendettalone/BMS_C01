#ifndef _XTAL_FREQ
#define	_XTAL_FREQ 20000000

#endif	/* INIT_H */


void I2CInit(const unsigned long c);
void I2CStart();
void I2CStop();
void I2CRestart();
void I2CAck();
void I2CNak();
void I2CWait();
void I2CSend(unsigned char dat);
unsigned char I2CRead(void);