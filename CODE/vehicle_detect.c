#include<lpc21xx.h>
#include<string.h>
#include "lcd_4bit.h"
#define BUZ 1<<3
#define IR 1<<4
#define DC 1<<5
void UART0_config(void);
void UART1_config(void);
void GSM_config(int);
void UART_int_config(void);
void UART1_tx(unsigned char );
unsigned char UART1_rx(void);
void UART0_tx(unsigned char );
void UART0_rx(void);
void GSM_str(unsigned char *);


void UART_isr(void)__irq
{
 unsigned int i=0,j;
  char a[13];

	 for(i=0;i<12;i++)
	 {
	   while((U1LSR&0x1)==0);
	   a[i]=U1RBR;
	 }
	 a[12]='\0';
  
  if(strcmp(a,"060068232A67")==0)
  {
     IOSET0=DC;
  }
  else
      {
	  	for(j=1;j<=3;j++)
		{
			 if((j==1)|(j==2))
			 {
				  IOSET0=BUZ;
				  delay(500);
				  IOCLR0=BUZ;
			      GSM_config(2);
			 }
			 else if(j==3)
			 {
			 	IOSET0=BUZ;
				delay(500);
				IOCLR0=BUZ;
			   	GSM_config(3);
			 }
		}
	      
	  }
	  VICVectAddr=0;
}
int main()
{
	IODIR0|=IR|BUZ;
    UART0_config();
	UART_int_config();
	UART1_config();
   while(1)
   {
	if((IOPIN0&IR)==0)
	{
		IOSET0=BUZ;
		delay(1000);
		IOCLR0=BUZ;
		GSM_config(1);
	}
	}	
}
void UART0_config(void)
{				
    PINSEL0|=0X5;
	U0LCR=0X83;
	U0DLL=97;
	U0DLM=0;
	U0LCR=0X03;
   
	
}
void UART1_config(void)
{				
    PINSEL0|=0X50000;
	   
	U1LCR=0X83;
	U1DLL=97;
	U1DLM=0;
	U1LCR=0X03;
}
void UART_int_config(void)
{
   VICIntSelect=0;
   VICVectCntl1=(0x20)|7;
   VICVectAddr1=(unsigned long)UART_isr;
   U1IER=((1<<1)|(1<<0));
   VICIntEnable=1<<7;
}
void GSM_config(int n)
{
     int k;
    GSM_str("AT\r\n");
	delay(1000);
	GSM_str("AT+CMGF=1\r\n");
	delay(1000);
	GSM_str("AT+CMGS=\"+919655927006\"\r\n");
	delay(1000);
	if(n==1)
	{
	   GSM_str("motion is detected please check");
	}
	else if(n==2)
	{
	  	GSM_str("ID is wrong");
	}
	else if(n==3)
	{
		for(k=0;k<6;k++)
	  	GSM_str("**WARNING**:someone is trying to access the vehicle");
	}
	UART0_tx(0X1A);
	
}
/*void UART1_tx(unsigned char tx1byte)
{
   while(((U1LSR>>5)&1)==0);
   U1THR=tx1byte;
}
unsigned char UART1_rx(void)
{
   while((U1LSR&0x1)==0);
   return U1RBR;
} */
void UART0_tx(unsigned char txbyte)
{
  while(((U0LSR>>5)&1)==0);
  U0THR=txbyte;
}
void UART0_rx(void)
{
    unsigned char rx[30];
	unsigned int i=0;

	while(U0LSR&1)
	{
	   rx[i++]=U0RBR;

	   if(i>(sizeof(rx)-1))
	   break;
	}
	rx[i]='\0';
	GSM_str(rx);
}
void GSM_str(unsigned char *s)
{
   while(*s)
   {
       UART0_tx(*s++);
   }
}
