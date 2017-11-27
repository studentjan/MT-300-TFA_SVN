  //******************************************************************************
  //* File Name          : includes.h
  //* Description        : file with includes
  //* Created			 : Jan Koprivec
  //* Date				 : 13.7.2017
  //******************************************************************************
  #ifndef __INCLUDES_H
  #define __INCLUDES_H
	
	  #include "stm32f3xx_hal.h"
		#include "stm32f3xx.h"
		#include <stdint.h>
		#include <stdio.h>
		#include <math.h>
		#include <string.h>
		#include <stdlib.h>
		#include "defines.h"
		#include "tasks.h"
		#include <stdlib.h>
		#include <stdio.h>
		#include <string.h>
		#include <assert.h>
		#include <ctype.h>
		#include <errno.h>
		#include <limits.h>

		
	//MAIN
	//-------------------------------------------------------------------------------
	extern uint32_t glob_test1;
	extern uint32_t glob_test2;
	extern uint32_t glob_test3;
	extern uint32_t glob_test4;
	extern uint32_t glob_test5;
	//-------------------------------------------------------------------------------
  //TIMER
  //-------------------------------------------------------------------------------	
	extern void MX_TIM3_Init(void);
	//-------------------------------------------------------------------------------
	//CLOCK
  //-------------------------------------------------------------------------------	
  void SystemClock_Config(void);
	//-------------------------------------------------------------------------------
	//SDADC
  //-------------------------------------------------------------------------------	
	extern void MX_SDADC1_Init(void);
	extern void MX_SDADC2_Init(void); 
	extern void MX_SDADC3_Init(void); 
	//-------------------------------------------------------------------------------
	//GPIO
  //-------------------------------------------------------------------------------	
	extern void MX_GPIO_Init(void);
	//-------------------------------------------------------------------------------
	//COMUNICATION
  //-------------------------------------------------------------------------------	
  extern void MX_USART3_UART_Init(void);
  extern void MX_SPI1_Init(void);
	//-------------------------------------------------------------------------------
	//OS
  //-------------------------------------------------------------------------------	
	extern uint32_t TimerTick;
	extern void init_OS(void);
	extern void init_event_Q(void);
	extern void init_timers(void);
	extern void do_events(void);
	extern void service_timers(void);
	extern void run_OS(void);
	extern char set_event(unsigned int e_ID,void (*e_fp)(void));
	extern char set_timer(unsigned int t_ID,unsigned int t_cnt,void (* t_fp)(void));
	extern char restart_timer(unsigned int t_ID,unsigned int t_cnt,void (* t_fp)(void));
	extern char end_task(unsigned int t_ID);
	//-------------------------------------------------------------------------------
	//USB
  //-------------------------------------------------------------------------------	
  extern void MX_USB_DEVICE_Init(void);
	extern uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);
	//-------------------------------------------------------------------------------
	//TEST
	//-------------------------------------------------------------------------------	
	extern uint32_t led_blink_time;
  extern void test_task(void);
	extern void Led_flash_task(void);
	//-------------------------------------------------------------------------------
	


  #endif
	
