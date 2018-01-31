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
#include <stdbool.h>

void MX_USART3_UART_Init(void);
void MX_SPI1_Init(void);
void SPI1_Send_data(uint8_t* data);
void USBConnected_Handler(void);
void Welcome_msg(void);
void Transmit_results_task(void);
void send_warning_MSG(void);
void check_connection(void);
void disconnect_function(uint32_t temp_connection_control);
void checkUSBconnected(void);
void usbCheckTask(void);
bool commandAnalzye(uint8_t dir);
void transmitCommFunc(uint32_t func, uint32_t command, uint32_t add,char* leftover,int  dir);
void transmitFunc(uint32_t func, uint32_t command,char * add,char* leftover, int  dir);
void transmitEvent(uint32_t event,uint32_t std, char * add, int dir);


enum funcEnums
{
	__RPE,
	__RISO,
	__ALL_PE,
	__ONE_PE,
	__PH_PH,
	__C_W,
	__CONT,
	__M_URES,
	__UNL_RMS,
	__UNL_PEAK
};
enum commandEnums
{
	__START,
	__STOP,
	__GET
};
enum addEnums
{
	__LOW,
	__MID,
	__HIGH,
	__RESULT,
	__500V,
	__INITIATED,
	__OPENED,
	__STARTED
};
enum stdEnums
{
	__NONE_EN,
	__CORD_EN,
	__MACHINES_EN,
	__WELDING_EN
};
enum eventEnums
{
	__STOPPED_EN,
	
};
#define USB_CHECK_INTERVAL		10	//n*10ms
#endif
