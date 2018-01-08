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
#define INIT_CORD															21
#define STOP_CORD															22
#define CORD_MEAS_CORRECT_WIRING							23
#define SEND_TFA_MAINS_STATUS									24
#define CHECK_CONNECTION											25
#define CORD_RISO_PHASES_TO_PE								26
#define CORD_RISO_ONE_PHASE_TO_PE							27
#define CORD_RISO_PHASE_TO_PHASE							28
#define CORD_MEAS_CONTINUITY									29
#define MACH_RISO_PHASES_TO_PE								30
#define MACH_RISO_ONE_PHASE_TO_PE							31
#define MACH_RISO_PHASE_TO_PHASE							32
#define MACH_RPE_STOP													33
#define MACH_RPE_START												34
#define INIT_MACH															35
#define STOP_MACH															36
#define MACH_URES															37
#define SINCHRONUS_TRANSMITT									38
#define SYNCHRONUS_PROCESS										39
#define DEINIT_CORD														40
#define START_SET_NORMAL											41
#define DEINIT_MACH														42
#define MACH_URES_STOP												43
#define USB_CHECK_TASK												44
#define INIT_WELD															45
#define WELD_RPE_START												46
#define WELD_RISO_PHASES_TO_PE								47
#define WELD_RISO_ONE_PHASE_TO_PE							48
#define WELD_RISO_PHASE_TO_PHASE							49
#define WELD_RPE_STOP													50
#define DEINIT_WELD														51
#define STOP_WELD															52
#define WELD_RISO_MAINS_TO_WELD								53
#define WELD_RISO_WELD_TO_PE									54
#define WELD_RISO_MAINS_TO_CLASS2							55
#define WELD_RISO_MAINS_TO_WELD_STOP					56
#define WELD_RISO_WELD_TO_PE_STOP							57
#define WELD_RISO_MAINS_TO_CLASS2_STOP				58
#define WELD_UNL_START_RMS										59
#define WELD_UNL_STOP_RMS											60
#define WELD_UNL_START_PEAK										61
#define WELD_UNL_STOP_PEAK										62
#endif // __TASKS_H
