

//define to prevent recursive inclusion
#ifndef __SERIAL_RX_H
#define __SERIAL_RX_H

#include "defines.h"
#include <stdint.h>

//-------------------------------------------------------------------

#define _SER_START_SIGN             '>' 
#define _SER_DATA_DELIMITER         ':'
#define _SER_CODE_DELIM							'|'
#define _ID_DEBUG					          'T'    //ID terminal debug
#define _ID_PROCESORSKA             'V'    //ID procesorska
#define _ID_MERILNA            			'H'    //ID merilna
#define _ID_MT		            			'M'    //ID Machinnery tester
#define _ID_PAT											'P'
#define _ID_TFA            					'A'    //ID Three phase adapter

#define _MESSAGE_NACK  				      "NACK"
#define _MESSAGE_ACK      				  "ACK" 

#define _SER_TELEGRAM_RESULT        "RESULT"
#define _SER_TELEGRAM_FIRMWARE      "FIRMWARE" 
#define _SER_TELEGRAM_CALIB         "CALIB" 

uint8_t command_parser(char *cmd,uint8_t dir);
void command_return(uint8_t ans, uint8_t dir);
void send_com_message(char * ans_type, char * ans_code, uint8_t num_valid, char *num, uint8_t dir);
void command_analyze(uint8_t dir);
void transmit_command_handle(void);



//char elvariac_retransmitt_buffer[_SER_DATA_SIZE];
//uint8_t retransmitt_counter;
//uint8_t elvariac_wait_for_ack;
//-------------------------------------------------------------------


#endif // __SERIAL_RX_H
