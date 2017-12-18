#ifndef __MACHINES_H
#define __MACHINES_H


#include "defines.h"
#include <stdbool.h>

#define _1_PHASE 1
#define _3_PHASE 3


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
void init_mach(void);
void stop_mach(void);

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

#define MACH_RISO_MASKS				0x00000000FC
//++++++++++++++++++++++++++++++zastavice za mach_insolation_status+++++++++++++++++++++++++++++++
#define L1_PE_FAIL					0x00000001
#define L2_PE_FAIL					0x00000002
#define L3_PE_FAIL					0x00000004
#define N_PE_FAIL						0x00000008

#define MACH_ONE_PHASE_TO_PE_MASK 0x0000000F


//--------------------------------current_URES_measurement-----------------------------
#define __L1_PE		1
#define __L2_PE		2
#define __L3_PE		3
#define __L1_N		4
#define __L2_N		5
#define __L3_N		6



#define CON_L1_A	set_REL(10)
#define DIS_L1_A	rst_REL(10)
#define CON_L2_A	set_REL(11)
#define DIS_L2_A	rst_REL(11)
#define CON_L3_A	set_REL(12)
#define DIS_L3_A	rst_REL(12)
#define CON_N_A		set_REL(13)
#define DIS_N_A		rst_REL(13)
#define CON_PE_A	set_REL(8)
#define DIS_PE_A	rst_REL(8)
#define CON_L1_B	set_REL(33)
#define DIS_L1_B	rst_REL(33)
#define CON_L2_B	set_REL(34)
#define DIS_L2_B	rst_REL(34)
#define CON_L3_B	set_REL(35)
#define DIS_L3_B	rst_REL(35)
#define CON_N_B		set_REL(36)
#define DIS_N_B		rst_REL(36)
#define CON_PE_B	set_REL(30)
#define DIS_PE_B	rst_REL(30)

#endif