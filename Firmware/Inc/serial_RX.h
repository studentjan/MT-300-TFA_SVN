

//define to prevent recursive inclusion
#ifndef __SERIAL_RX_H
#define __SERIAL_RX_H

#include "defines.h"
#include <stdint.h>
#include <stdbool.h>

//-------------------------------------------------------------------
#define _SER_BUFFER_SIZE            		256  //dolzina bufferja za celoten ukaz
#define _SER_DATA_SIZE               		30  //dolzina bufferja za funkcijo, event in data string

#define MAX_ADDITIONAL_COMMANDS 				5
#define MAX_ADDITIONAL_COMMANDS_LENGTH 	100

#define _SER_START_SIGN             '>' 
#define _SER_DATA_DELIMITER         ':'
#define _SER_DATA_DELIM_STR					":"
#define _SER_CODE_DELIM							'|'
#define _ID_DEBUG					          'T'    //ID terminal debug
#define _ID_PROCESORSKA             'V'    //ID procesorska
#define _ID_MERILNA            			'H'    //ID merilna
#define _ID_MT		            			'M'    //ID Machinnery tester
#define _ID_PAT											'P'
#define _ID_TFA            					'A'    //ID Three phase adapter

#define _UART_DIR_MT	      					0
#define _UART_DIR_HV          				1
#define _UART_DIR_DEBUG       				2
#define _UART_DIR_485		       				3
#define _UART_DIR_USB		       				4

#define _MESSAGE_NACK  				      "NACK"
#define _MESSAGE_ACK      				  "ACK" 
#define _MESSAGE_WAR								"WARNING"
#define _MESSAGE_COM_ERR						"COMMUNICATION ERROR"

#define _ANS_MESSAGE_NACK       			0
#define _ANS_MESSAGE_ACK       				1 
#define _ANS_MESSAGE_ACK_RECIEVED			2
#define _ANS_MESSAGE_NACK_RECIEVED		3

#define _SER_TELEGRAM_RESULT        "RESULT"
#define _SER_TELEGRAM_FIRMWARE      "FIRMWARE" 
#define _SER_TELEGRAM_CALIB         "CALIB" 

uint8_t command_parser(char *cmd,uint8_t dir);
void command_return(uint8_t ans, uint8_t dir);
void send_com_message(char * ans_type, char * ans_code, uint8_t num_valid, char *num, uint8_t dir);
void command_analyze(uint8_t dir);
void transmit_command_handle(void);

#define	TRANSMIT_COMMAND_CHECK		_ON

#if TRANSMIT_COMMAND_CHECK == _ON
#define TRANSMIT_BUFF_SIZE				100
#define TRANSMIT_HANDLE_BUFF_SIZE 10
#define TRANSMIT_SLOT_FREE				0		//mora bit 0
#define NUM_NACK_EVENTS						3
#define TRANSMIT_HANDLE_WAIT			5		//n*10ms - koliko casa cakamo na to da dobimo odgovor pred ponovnim posiljanjem
																			//za program v visual studiu rabmo okol 50ms
uint32_t recieve_ack_func(void);
uint32_t recieve_nack_func(void);
void communication_init(void);
#endif

//--------------------vrsta za sprejemanje komand------------------------------
//USART Mode Selected
#define USART_MODE_TRANSMITTER           0x00
#define USART_MODE_RECEIVER              0x01      
//USART Transaction Type
#define USART_TRANSACTIONTYPE_CMD        0x00
#define USART_TRANSACTIONTYPE_DATA       0x01
#define RxBufferSize_MAX									254
#define UART_COMMAND_CHARACTER_10   0x0a  //termination charater
#define UART_COMMAND_CHARACTER_13   0x0d  //termionation character
#define UART2_QUEUE_SIZE 5
#define UART2_RX_BUFFER_SIZE      256   //UART RX input buffer size
#define UART2_RX_COMMAND_SIZE     200   //UART RX command buffer size

typedef struct
{
 uint32_t command_ID;        	//timer ID Number 
 char command[UART2_RX_COMMAND_SIZE];	
 uint8_t direction;
} USART2_RX_t;

#define UART2_QUEUE_SIZE 5 //5 commands can be in a queue

#define QUEUE_FREE					0 //must be zero
#define QUEUE_FAIL					0
#define QUEUE_PASS					1

void command_do_events(void);
void init_command_queue(void);
void recieved_command_analyze(char *command,uint8_t dir);
void add_command_to_queue(uint8_t* Buf, uint32_t Len,uint8_t dir);
bool CheckCRC(unsigned char isCRC, unsigned char CRCvalue, char *message);
int CalculateCRC(char *message);
void SendComMessage(int send_control,char transmitter, char receiver, char * function, char * command, char * additional_code, char * data, int dirrection);
//-------------------------------------------------------------------------------


//char elvariac_retransmitt_buffer[_SER_DATA_SIZE];
//uint8_t retransmitt_counter;
//uint8_t elvariac_wait_for_ack;
//-------------------------------------------------------------------


#endif // __SERIAL_RX_H
