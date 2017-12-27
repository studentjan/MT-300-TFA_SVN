

//define to prevent recursive inclusion
#ifndef __SERIAL_COM_H
#define __SERIAL_COM_H

#include "defines.h"
#include <stdint.h>
#include <stdbool.h>




#define _ON 	1
#define _OFF 	0
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&//
//																NASTAVITVE																	 //
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&//
#define	TRANSMIT_COMMAND_CHECK		_ON				//s tem makrojem vklopimo ali izklopimo preverjanje posiljanja
																						//ce je preverjanje izklopljeno, se tudi koda ne nalozi
#define SYNCHRONUS_TRANSMITT			_ON				//vklopima ali izklopimo sinhrono oddajanje	(na 50 ms se izmenjujeta transmitter in receiver)

#define SERIAL_COMMAND_SIZE     				200   		//maksimalna velikost komande
//Nastavitve bufferja za prejemanje
#define _SER_BUFFER_SIZE            		256  			//dolzina bufferja za celoten ukaz
#define _SER_DATA_SIZE               		30  			//dolzina bufferja za funkcijo, event in data string
#define RxBufferSize_MAX					 			254
#define SERIAL_IN_QUEUE_SIZE 						5					//velikost vhodnega bufferja za sprejem komand
#define SERIAL_BUFFER_SIZE      				256   		//velikost vhodnega bufferja
#define MAX_ADDITIONAL_COMMANDS 				5
#define MAX_ADDITIONAL_COMMANDS_LENGTH 	100

//Nastavitve bufferja za oddajanje

#if TRANSMIT_COMMAND_CHECK == _ON
#define TRANSMIT_BUFF_SIZE				100
#define TRANSMIT_HANDLE_BUFF_SIZE 10
#define TRANSMIT_OUT_BUFF_SIZE		10
#define TRANSMIT_SLOT_FREE				0		//mora bit 0
#define NUM_NACK_EVENTS						3		//kolikokrat ponovi posiljanje ce dobi nack
#define TRANSMIT_HANDLE_WAIT			10		//n*10ms - koliko casa cakamo na to da dobimo odgovor pred ponovnim posiljanjem
																			//za program v visual studiu rabmo okol 50ms
#define WAIT_FOR_MSG_ACK					3		//koliko casa cakamo na ack TRANSMIT_HANDLE_WAIT * n
static uint32_t recieve_ack_func(void);
static uint32_t recieve_nack_func(void);
static void communication_init(void);
#endif

//Makroji smeri (pomembno za serial_send_handler)
#define _UART_DIR_MT	      					0
#define _UART_DIR_HV          				1
#define _UART_DIR_DEBUG       				2
#define _UART_DIR_485		       				3
#define _UART_DIR_USB		       				4
#define _EMPTY_DIR										10	//smer rezervirana za idntifikacijo posiljanja praznega stringa
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&//


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&//
//												MAKROJI IN KONSTANTE																 //
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&//
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
#define _ID_NONE										'N'

#define _MESSAGE_NACK  				      "NACK"
#define _MESSAGE_ACK      				  "ACK" 
#define _MESSAGE_WAR								"WARNING"
#define _MESSAGE_COM_ERR						"COMMUNICATION ERROR"

#define _ANS_MESSAGE_NACK       			1
#define _ANS_MESSAGE_ACK       				0 
#define _ANS_MESSAGE_ACK_RECIEVED			2
#define _ANS_MESSAGE_NACK_RECIEVED		3

#define _SER_TELEGRAM_RESULT        "RESULT"
#define _SER_TELEGRAM_FIRMWARE      "FIRMWARE" 
#define _SER_TELEGRAM_CALIB         "CALIB" 

#define SERIAL_TERM_CHARACTER_10   0x0a  	//termination charater
#define SERIAL_TERM_CHARACTER_13   0x0d  	//termionation character

#define QUEUE_FREE					0 //must be zero
#define QUEUE_FAIL					0
#define QUEUE_PASS					1
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&//

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&//
//												DOSTOPNE FUNKCIJE																		 //
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&//
//funkcije dostopne tudi od zunaj
//ne pozabi dodati kodo v serial send handler
void serial_com_init(void);
void add_command_to_queue(uint8_t* Buf, uint32_t Len,uint8_t dir);	//doda komando v cakalno vrsto za analiziranje. Poklici ko dobis podatke iz serijskega vodila
uint32_t SendComMessage(int send_control,char transmitter, char receiver, char * function, char * command, char * additional_code, char * data, int dirrection);	//ko zelis posilati komando poklici to funkcijo in ji dodaj potrebne parametre
bool CheckIfACK(uint32_t msg_id);
__weak void SendTimerInit(void);
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&//

//struktura za shranjevanje prejete komande
typedef struct
{
 uint32_t command_ID;        	//timer ID Number 
 char command[SERIAL_COMMAND_SIZE];	
 uint8_t direction;
} SERIAL_RX_t;

#endif // __SERIAL_RX_H
