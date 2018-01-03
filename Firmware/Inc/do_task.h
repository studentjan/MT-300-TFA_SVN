  //******************************************************************************
  //* File Name          : do_task.h
  //* Description        : Tasks
  //* Created			 : Jan Koprivec
  //* Date				 : 11.9.2017
  //******************************************************************************
#ifndef __DO_TASK_H
#define __DO_TASK_H
#include <stdint.h>

void disable_interrupt(void);
void start_measure(void);
void stop_measure(void);
void start_measure_no_THD(void);
void compute_THD_with_FFT(void);
void compute_THD_with_corelation(void);
void power_on_test(void);
void led_blinking_task(void);
void get_phase_seq(void);
void send_mains_status(void);
struct connected_device get_connected_device(void);
void enable_sinchro_interrupt(void);
void disable_sinchro_interrupt(void);

struct connected_device{
	char device_ID;
	uint32_t device_dir;
};
#endif
