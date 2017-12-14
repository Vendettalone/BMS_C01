#include<xc.h>
void Init_IO(void);
void Init_IIC(void);
void Init_UART(void);
void Init_timer0(int us);
void Init_timer1(int us);
void Init_timer2(int us);

void Init_timer1_100ms(void)
{
   T1CON=0x30;//1:8,使用4分总线频率，关闭时钟
   TMR1H=0x06;//设置TMR1寄存器值为1536；
   TMR1L=0x00;
}
void Init_IO(void)
{
	TRISB=0xd7;//1101 0111
	RB5=0;
	RB3=1;
	TRISC=0xbf;//1011 1111
	TRISD=0x02;//0000 0010
	PORTD=0x7f;//0111 1111
}
unsigned char readEEPROM(unsigned char address)
{
  EEADR = address; //Address to be read
  EECON1.EEPGD = 0;//Selecting EEPROM Data Memory
  EECON1.RD = 1; //Initialise read cycle
  return EEDATA; //Returning data
}
void writeEEPROM(unsigned char address, unsigned char datas)
{
  unsigned char INTCON_SAVE;//To save INTCON register value
  EEADR = address; //Address to write
  EEDATA = datas; //Data to write
  EECON1.EEPGD = 0; //Selecting EEPROM Data Memory
  EECON1.WREN = 1; //Enable writing of EEPROM
  INTCON_SAVE=INTCON;//Backup INCON interupt register
  INTCON=0; //Diables the interrupt
  EECON2=0x55; //Required sequence for write to internal EEPROM
  EECON2=0xAA; //Required sequence for write to internal EEPROM
  EECON1.WR = 1; //Initialise write cycle
  INTCON = INTCON_SAVE;//Enables Interrupt
  EECON1.WREN = 0; //To disable write
  while(PIR2.EEIF == 0)//Checking for complition of write operation
  {
    asm nop; //do nothing
  }
  PIR2.EEIF = 0; //Clearing EEIF bit
}
