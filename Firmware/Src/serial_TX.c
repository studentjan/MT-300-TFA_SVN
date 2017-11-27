

#include "serial_TX.h"
#include "serial_RX.h"
#include "defines.h"
#include <stdint.h>
#include <string.h>
#include "usbd_cdc_if.h"
#include <math.h>

uint8_t UART_direction;

static void my_round(float *num,int8_t n)
{   
 float x; 
 uint32_t fct;
 
 switch(n)
  {
   case 0: fct=1;break;
   case 1: fct=10;break;
   case 2: fct=100;break;
   case 3: fct=1000;break;
   case 4: fct=10000;break;
   case 5: fct=100000;break;
   case 6: fct=1000000;break;
   default:;
  } 
 
 x=*num;  
 x*=fct;
 x+=0.5f;
 x=floor(x);
 x/=fct;
 *num=x; 
}


//oddaj znak na UART
//oddaja 1 znaka pri 115200 baud traja ca. 80us
void TxChr(uint8_t ch)
{           
	//while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); 
	if(UART_direction==_UART_DIR_USB)
	{
		CDC_Transmit_FS(&ch, 1);		 
	}
}

//oddaj charater %2X format
void TxHex(unsigned char ch)
{  
 char strv[10];   
 sprintf(strv,"%02X",ch);
 TxStr("0x");
 TxStr(strv);
 TxChr(' ');
}

//oddaj OK
void Tx_OK(void)
{
 TxChr('O');
 TxChr('K');
 TxChr(13);
}

//oddaj ER (error)
void Tx_ER(void)
{
 TxChr('E');
 TxChr('R');
 TxChr(13);
}

//oddaj crlf
void Crlf(void)
{   
 TxChr(13);
 TxChr(10);
}

//oddaj space
void TxSpc(void)
{
 TxChr(' '); 
}

//oddaj signed short(16bit) na RS232
void TxSDec(signed short n)
{
 signed char i;
 char t[6];
 t[0]=0;
 i=0;
 
 if(n<0) {n=-n;TxChr('-');}
 
 if (n)
 {
  while(n>0)
   {
    t[i]=(n%10)+0x30;
    n/=10;
    i++;
   }
 if(i)i--;
 if (!n)
  for (;i>=0;i--) TxChr(t[i]);
 else
   TxChr('0');
 }
 else
  TxChr('0');   
}

//oddaj unsigned short(16bit) na RS232
void TxDec(unsigned short n)
{
 signed char i;
 char t[6];
 t[0]=0;
 i=0;
 if (n)
 {
  while(n>0)
   {
    t[i]=(n%10)+0x30;
    n/=10;
    i++;
   }
 if(i)i--;
 if (!n)
  for (;i>=0;i--) TxChr(t[i]);
 else
   TxChr('0');
 }
 else
  TxChr('0');   
}

//oddaj unsigned long(32 bit) na RS232
void TxLong(unsigned long n)
{
 signed char i;
 char t[11];
 t[0]=0;
 i=0;
 if(n)
 {
  while(n>0)
   {
    t[i]=(n%10)+0x30;
    n/=10;
    i++;
   }
 if(i) i--;
 if (!n)
  for (;i>=0;i--) TxChr(t[i]);
 else
   TxChr('0');
 }
 else
  TxChr('0');
}

//oddaj signed long(32 bit) na RS232
void TxSLong(signed long n)
{
 signed char i;
 char t[11];
 t[0]=0;
 i=0;
 
 if(n<0) {n=-n;TxChr('-');}
 
 if(n)
 {
  while(n>0)
   {
    t[i]=(n%10)+0x30;
    n/=10;
    i++;
   }
 if(i) i--;
 if (!n)
  for (;i>=0;i--) TxChr(t[i]);
 else
   TxChr('0');
 }
 else
  TxChr('0');
}

//oddaj signed long long (64 bit) na RS232
void TxSLongLong(signed long long n)

{
 signed char i;
 char t[25];
 t[0]=0;
 i=0; 
 if(n<0) {n=-n;TxChr('-');}  
 if(n)
 {
  while(n>0)
   {
    t[i]=(n%10)+0x30;
    n/=10;
    i++;
   }
 if(i) i--;
 if (!n)
  for (;i>=0;i--) TxChr(t[i]);
 else
   TxChr('0');
 }
 else
  TxChr('0');
}

void TxStr(char *str)
{
 unsigned int len;
 unsigned char i,c;
 i=0;
	if(UART_direction==_UART_DIR_USB)
//	{
//		len = strlen((const char *)&str);
//		CDC_Transmit_FS((uint8_t *)&str,(uint16_t)len);
//	}
//	else
	{
			while((c=str[i])>0)
			 {
				TxChr(c);
				i++;
			 }
	}
}

//oddaj delimiter ", "
void TxDel(void)
{
 TxChr(',');
 TxChr(' ');
}

//float2string
//printout floating point value using precision and prefix
//trajanje code ca. 86us, brez koncnega izpisa
void Xprintf(float f)
{    
  float round=ROUND_RES; //float2sting round variable precision
  
  unsigned char float_ptr=0;    
  signed char pow10;
  signed char i,n,cc;
  signed char dp;
  float temp;
  
  unsigned char float_buff[PRECISION+5];  //float2string buffer    
   
  //za pravilen izpis na terminal
  //init buffer
  for(cc=0;cc<(PRECISION+5);cc++) float_buff[cc]=' ';
    
  // check sign and make positive
  if (f<0)
   {                                                  
    f=-f;
    float_buff[0]='-';
    float_ptr++;
   }
  
  // normalise between +-1.0000 and +-9.9999 
  // first get f below 10 when rounded       
  // if we don't allow for rounding 9.9999 could change to 10.00! 
  pow10=0;
  
  temp=10.0f-round; 
  while (f>=temp)
   {
    f/=10;               
    pow10++;
   }
  f+=round;                                                     
      
  //we leave number between 0 and 9.999 - so we can give a FIXED result
  dp=pow10;	//handle exponent by moving decimal point
  pow10=0;  
  
  if (dp>PRECISION) return; //can't print smaller than precision 
                    
  //dp is now equal to exponent
  //f is in range 1.0<=f<10
  //PRINT MANTISSA. Include decimal point and print until precision runs out
  for (i=0;i<PRECISION;i++) 
  {  
   f-=(n=(signed char)f);          
   float_buff[float_ptr++]=n+'0'; //zapis cifre                  
   if (i==dp) float_buff[float_ptr++]='.'; //zapis decimalne pike       
   f=ldexp(f+ldexp(f,2),1);  //f*=10 but faster
  }              
 float_buff[float_ptr++]=0; //termination character           
  
 //izpis 
 i=0;
 while((float_buff[i]))
  {
   TxChr(float_buff[i]);
   i++;
  }  
}

//debug print float(rounded to rnd digits) with header
void Debug_float(char *header,float value,uint8_t rnd)
{
 char tmpstr[30],formatstr[5];
 sprintf(formatstr,"%%.%df",rnd);  
 my_round(&value,rnd);
 TxStr(header);TxStr(": ");
 sprintf(tmpstr,"%.3f",value);
 TxStr(tmpstr);Crlf();                   
}

//debug print string
void Debug_string(char *strng)
{
 TxStr(strng);Crlf();
}

//tole funkcijo najbols da si prilagodi vsak sam
//vse kar nardi je to da poslje sporocilo po pravem portu, ki ga izbere glede na dir
void serial_send_handler(char * send_buff, uint16_t buffer_size,uint8_t dir)
{
	
	if((dir==_UART_DIR_USB)||(dir==_UART_DIR_DEBUG))
	{
		test3_on;
		while(HAL_OK!=CDC_Transmit_FS((uint8_t *)send_buff, buffer_size));
		test3_off;
	}
	
}

 
