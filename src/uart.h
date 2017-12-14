#ifndef _XTAL_FREQ
#define	_XTAL_FREQ 20000000
#endif	

extern unsigned int Reg[32];
const unsigned int wCRCTalbeAbs[] =
{
0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401, 0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400, 
};

char UART_Init(const long int baudrate);
void UART_Read_Text(char buf[],unsigned int len);
void UART_Write_text(char *text);
void UARTWrite(unsigned char buf[],unsigned char len);
unsigned int CRC16_1(char *buf,char len);
unsigned int CRC16_2(char* pchMsg, char wDataLen);
