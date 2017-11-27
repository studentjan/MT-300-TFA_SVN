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
void send_warning_MSG(void);
void check_connection(void);
void disconnect_function(uint32_t temp_connection_control);

#endif
