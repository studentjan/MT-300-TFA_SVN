#ifndef __MACHINES_H
#define __MACHINES_H


#include "defines.h"
#include <stdbool.h>


#define __MACH_RPE
#define MACH_AUTO_CONTINUE_MEAS		_OFF
#define MACH_RISO_LIMIT	 					1.0f	//n*Mohm

void set_RISO_mach_resistance(char* value);
bool mach_check_RISO_resistance(void);
void mach_RISO_onePhaseToPE(void);
void mach_RISO_phasesToPE(void);
void set_phase_num_mach(int phase_num);

void MachinesRPEStart(void);
void MachinesRPEStop(void);
void mach_URES(void);
void mach_URES_Stop(void);
void machOPENcontactors(void);
void disconnectURESContactors(void);
void init_mach(void);
void deinitMachines(void);
void stop_mach(void);
void SetMachLimit(char * lim_str);

//---------------------------zastavice za mach_task_control------------------------------
#define __MACH_RPE_IN_PROGRESS 									0x00000001
#define __MACH_RPE_MEASURED											0x00000002
#define __MACH_RISO_STARTED											0x00000004
#define __MACH_RISO_RES_REQUESTED								0x00000008
#define __MACH_RISO_PHASES_TO_PE_IN_PROGRESS		0x00000010
#define __MACH_RISO_PHASES_TO_PE_MEASURED				0x00000020
#define __MACH_RISO_ONE_PHASE_TO_PE_IN_PROGRESS 0x00000040
#define __MACH_RISO_ONE_PHASE_TO_PE_MEASURED		0x00000080
#define __MACH_INIT_RECIEVED										0x00000100
#define __MACH_INITIATED												0x00000200
#define __MACH_URES_IN_PROGRESS									0x00000400
#define	__MACH_URES_L1_PE_MEASURED							0x00000800
#define	__MACH_URES_L2_PE_MEASURED							0x00001000
#define	__MACH_URES_L3_PE_MEASURED							0x00002000
#define	__MACH_URES_L1_N_MEASURED								0x00004000
#define	__MACH_URES_L2_N_MEASURED								0x00008000
#define	__MACH_URES_L3_N_MEASURED								0x00010000
#define	__MACH_URES_L1_L2_MEASURED							0x00020000
#define	__MACH_URES_L1_L3_MEASURED							0x00040000
#define	__MACH_URES_L2_L3_MEASURED							0x00080000
#define __MACH_URES_DISCONNECT_PS								0x00100000
#define __MACH_TEST_RECIEVED										0x00200000
#define __MACH_TIMER_INIT												0x00400000
#define __MACH_MAINS_ANALYZE_IN_PROGRESS				0x00800000
#define __MACH_REINIT														0x01000000
#define __MACH_POWER_START_REQUESTED						0x02000000

#define MACH_MEAS_MASKS				0x001FFCFF
#define MACH_RISO_MASKS				0x000000FC
#define MACH_URES_MASKS				0x000FFC00
#define MACH_IN_PROG_MASKS		0x00000451
//++++++++++++++++++++++++++++++zastavice za mach_insolation_status+++++++++++++++++++++++++++++++
#define L1_PE_FAIL					0x00000001
#define L2_PE_FAIL					0x00000002
#define L3_PE_FAIL					0x00000004
#define N_PE_FAIL						0x00000008

#define MACH_ONE_PHASE_TO_PE_MASK 0x0000000F

#endif