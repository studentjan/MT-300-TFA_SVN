  //******************************************************************************
  //* File Name          : tasks.h
  //* Description        : Task definitions for OS
  //* Created			 : Jan Koprivec
  //* Date				 : 13.7.2017
  //******************************************************************************

//define to prevent recursive inclusion
#ifndef __TASKS_H
#define __TASKS_H


#define	OS_LED_FLASH                  				1
#define	OS_TEST			                   				2
#define TEST_TASK															3
#define TEST_LED															4
#define TRANSMIT_RESULTS											5
#define WELCOME_MSG														6
#define COMPUTE_RMS														7
#define DISABLE_INTERRUPT											8
#define START_MEASURE													9
#define MEASURING_TASK												10
#define COMMAND_DO_EVENTS											11
#define START_MEASURE_NO_THD									12
#define MEASURING_TASK_NO_THD									13
#define COMPUTE_THD_WITH_CORELATION						14
#define COMPUTE_THD_WITH_FFT									15
#define MEASURING_TASK_FFT										16
#define POWER_ON_TEST													17
#define SEND_WARNING_MSG											18
#define LED_BLINKING_TASK											19
#define TRANSMIT_COMMAND_HANDLE								20
#define START_CORD_NORMAL											21
#define STOP_CORD															22
#define CORD_MEAS_NORMAL											23
#define SEND_TFA_MAINS_STATUS									24
#define CHECK_CONNECTION											25
#define CORD_MEAS_RISO												26

#endif // __TASKS_H
