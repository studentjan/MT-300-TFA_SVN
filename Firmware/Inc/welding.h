#ifndef __WELDING_H
#define __WELDING_H

#include "defines.h"
#include <stdbool.h>


#define WELD_AUTO_CONTINUE_MEAS		_OFF
#define WELD_RISO_LIMIT	 					1.0f	//n*Mohm

void set_phase_num_weld(int phase_num);
void WeldingRPEStart(void);
void WeldingRPEStop(void);
void init_weld(void);
void stop_weld(void);
void deinitWelding(void);
void set_RISO_weld_resistance(char* value);
void weld_RISO_phasesToPE(void);
void weld_RISO_onePhaseToPE(void);
void weld_RISO_MainsToClass2(void);
void weld_RISO_MainsToWeld(void);
void weld_RISO_WeldToPE(void);
void weld_RISO_MainsToWeld_Stop(void);
void weld_RISO_WeldToPE_Stop(void);
void weld_RISO_MainsToClass2_Stop(void);
void weld_UnlStart_RMS(void);
void weld_UnlStop_RMS(void);
void weld_UnlStart_peak(void);
void weld_UnlStop_peak(void);


//---------------------------zastavice za weld_task_control------------------------------
#define __WELD_RPE_IN_PROGRESS 									0x00000001
#define __WELD_RPE_MEASURED											0x00000002
#define __WELD_RISO_STARTED											0x00000004
#define __WELD_RISO_RES_REQUESTED								0x00000008
#define __WELD_RISO_PHASES_TO_PE_IN_PROGRESS		0x00000010
#define __WELD_RISO_PHASES_TO_PE_MEASURED				0x00000020
#define __WELD_RISO_ONE_PHASE_TO_PE_IN_PROGRESS 0x00000040
#define __WELD_RISO_ONE_PHASE_TO_PE_MEASURED		0x00000080
#define __WELD_INIT_RECIEVED										0x00000100
#define __WELD_INITIATED												0x00000200
#define __WELD_RISO_MAINS_WELD_IN_PROGRESS			0x00000400
#define	__WELD_RISO_MAINS_CLASS2_IN_PROGRESS		0x00000800
#define	__WELD_RISO_WELD_PE_IN_PROGRESS					0x00001000
#define	__WELD_RISO_CONTINIOUS_MEAS							0x00002000
#define	__WELD_UNL_PEAK_IN_PROGRESS							0x00004000
#define	__WELD_UNL_RMS_IN_PROGRESS							0x00008000
#define __WELD_TEST_RECIEVED										0x00200000
#define __WELD_TIMER_INIT												0x00400000
#define __WELD_MAINS_ANALYZE_IN_PROGRESS				0x00800000

#define WELD_MEAS_MASKS				0x001FFCFF
#define WELD_RISO_PE_MASKS				0x000000FC


//++++++++++++++++++++++++++++++zastavice za weld_insolation_status+++++++++++++++++++++++++++++++
#define L1_PE_FAIL					0x00000001
#define L2_PE_FAIL					0x00000002
#define L3_PE_FAIL					0x00000004
#define N_PE_FAIL						0x00000008

#define WELD_ONE_PHASE_TO_PE_MASK 0x0000000F



#endif