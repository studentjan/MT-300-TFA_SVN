  //******************************************************************************
  //* File Name          : comunication.h
  //* Description        : Comunication initialization
  //* Created			 : Jan Koprivec
  //* Date				 : 13.7.2017
  //******************************************************************************
  
#ifndef __COMUNICATION_H
#define __COMUNICATION_H
#include <stdint.h>
#include "defines.h"

void MX_USART3_UART_Init(void);
void MX_SPI1_Init(void);
void SPI1_Send_data(uint8_t* data);
void USBConnected_Handler(void);
void Welcome_msg(void);
void Transmit_results_task(void);
void command_do_events(void);
void init_command_queue(void);
void recieved_command_analyze(char *command);
void send_warning_MSG(void);

typedef struct
{
 uint32_t command_ID;        	//timer ID Number 
 char command[UART2_RX_COMMAND_SIZE];	
} USART2_RX_t;

#define UART2_QUEUE_SIZE 5 //5 commands can be in a queue

#define QUEUE_FREE					0 //must be zero
#define QUEUE_FAIL					0
#define QUEUE_PASS					1
#endif
