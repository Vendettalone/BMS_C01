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

	
