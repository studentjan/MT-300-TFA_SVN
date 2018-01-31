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
void enable_sinchro_interrupt(uint32_t task);
void disable_sinchro_interrupt(uint32_t task);
void synchroSetContactor(uint32_t contactor_name);

struct connected_device{
	char device_ID;
	uint32_t device_dir;
};


//------------------------------------result_transmitt_control---------------------------
#define UL1N_TRANSMITT_READY				0x00000001
#define UL2N_TRANSMITT_READY				0x00000002
#define UL3N_TRANSMITT_READY				0x00000004
#define UL1PE_TRANSMITT_READY				0x00000008
#define UNPE_TRANSMITT_READY				0x00000010
#define UL1PE_TRANSMITT_READY				0x00000020
#define IL1_TRANSMITT_READY					0x00000040
#define IL2_TRANSMITT_READY					0x00000080
#define IL3_TRANSMITT_READY					0x00000100
#define IDIFF_TRANSMITT_READY				0x00000200
#define TIL1_TRANSMITT_READY				0x00000400
#define TIL2_TRANSMITT_READY				0x00000800
#define TIL3_TRANSMITT_READY				0x00001000
#define TUL1N_TRANSMITT_READY				0x00002000
#define TUL2N_TRANSMITT_READY				0x00004000
#define TUL3N_TRANSMITT_READY				0x00008000
#define PL1_TRANSMITT_READY					0x00010000
#define PL2_TRANSMITT_READY					0x00020000
#define PL3_TRANSMITT_READY					0x00040000
#define P3P_TRANSMITT_READY					0x00080000
#define SL1_TRANSMITT_READY					0x00100000
#define SL2_TRANSMITT_READY					0x00200000
#define SL3_TRANSMITT_READY					0x00400000
#define S3P_TRANSMITT_READY					0x00800000
#define PF1_TRANSMITT_READY					0x01000000
#define PF2_TRANSMITT_READY					0x02000000
#define PF3_TRANSMITT_READY					0x04000000
#define PF3P_TRANSMITT_READY				0x08000000

#define TRANSMMITT_VOLTAGE_READY_MASKS 	0x0000003F
#define TRANSMMITT_CURRENT_READY_MASKS 	0x000001C0
#define TRANSMMITT_THD_I_READY_MASKS 		0x00001C00
#define TRANSMMITT_THD_U_READY_MASKS 		0x0000E000
#define TRANSMMITT_POWER_R_READY_MASKS 	0x000F0000
#define TRANSMMITT_POWER_A_READY_MASKS 	0x00F00000
#define TRANSMMITT_PF_READY_MASKS 			0x0F000000

#define L1_CONTACTOR_TIME_TO_ON	99
#define L2_CONTACTOR_TIME_TO_ON	179
#define L3_CONTACTOR_TIME_TO_ON	220

#define TRANSMITT_RESULT_TIME	50	//n*10ms
//#define N_CONTACTOR_TIME_TO_ON	30
//#define PE_CONTACTOR_TIME_TO_ON	30
#endif
