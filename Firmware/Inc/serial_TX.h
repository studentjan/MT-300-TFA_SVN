

//define to prevent recursive inclusion
#ifndef __SERIAL_TX_H
#define __SERIAL_TX_H
#include <stdint.h>

#define PRECISION 8
#define ROUND_RES 5e-9 //float2sting round variable precision
void TxChr(unsigned char ch);
void Tx_OK(void);
void Tx_ER(void);
void Crlf(void);
void TxDec(unsigned short n);
void TxHex(unsigned char ch);
void TxSDec(signed short n);
void TxLong(unsigned long n);
void TxSLong(signed long n);
void TxSLongLong(signed long long n);
void TxStr(char *str);
void TxSpc(void);
void TxDel(void);
void Xprintf(float f);
void Debug_string(char *strng);
void Debug_float(char *header,float value,uint8_t rnd);
void serial_send_handler(char * send_buff, uint16_t buffer_size,uint8_t dir);

#endif // __SERIAL_TX_H
