
char UART_Init(const long int baudrate)
{
	unsigned int x;
	x = (_XTAL_FREQ - baudrate*64)/(baudrate*64);
	if(x>255)
	{
		x = (_XTAL_FREQ - baudrate*16)/(baudrate*16);
		BRGH = 1;
	}
	if(x<256)
	{
	  SPBRG = x;
	  SYNC = 0;
	  SPEN = 1;
      TRISC7 = 1;
      TRISC6 = 0;
      CREN = 1;
      TXEN = 1;
	  return 1;
	}
	return 0;
}

char UART_TX_Empty()
{
  return TRMT; 
}

char UART_Data_Ready()
{
   return RCIF;
}
char UART_Read()
{
  if(OERR) // check for Error 
    {
        CREN = 0; //If error -> Reset 
        CREN = 1; //If error -> Reset 
    }
  while(!RCIF);
  
  return RCREG;
}

void UART_Read_Text(char *Output, unsigned int length)
{
	unsigned int i;
	for(i=0;i<length;i++)
		*(Output+i) = UART_Read();
}
void UART_Read_Text2(char buf[],unsigned int length)
{
    unsigned int i;
    for(i=0;i<length;i++)
    {
        buf[i]=UART_Read();
    }
}
void UART_Write(char data)
{
  while(!TRMT);
  TXREG = data;
}

void UART_Write_Text(char *text)
{
  int i;
  for(i=0;text[i]!='\0';i++)
	  UART_Write(text[i]);
}
void UartWrite1(unsigned char *buf, unsigned char len)
{
    for(char i=0;i<len;i++)
        UART_Write(*buf++);
    
}

void UartWrite(unsigned char buf[],unsigned char len)
{
    for(char i=0;i<len;i++)
        UART_Write(buf[i]);
}