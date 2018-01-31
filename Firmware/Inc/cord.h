#ifndef __CORD_H
#define __CORD_H

#include "defines.h"
#include <stdbool.h>


#define CORD_RPE_LIMIT				1.0f		//n*ohm
#define CORD_RISO_LIMIT				1.0f	//n*Mohm
#define RPE_CONTINUITY_FAIL_LIMIT	20.0f 		//n*ohm


enum CordLimits
{
	SET_CORD_RPE_LIM,
	SET_CORD_RISO_LIM,
	SET_CORD_CONT_LIM,
};
//funkcije
void stop_cord_and_transmitt(uint32_t dir, char device_ID);
void cord_global_init_set(void);
void cord_correct_wiring_init(void);
void cord_continuity_init(void);
void cord_RISO_init(void);
void set_RPE_cord_resistance(char* value);
void set_RISO_cord_resistance(char* value);
void set_phase_num(int phase_num);
void deinitCord(void);
void SetCordLimit(uint32_t limit, char * lim_str);
//taski
void init_cord(void);
void deinitCord(void);
void stop_cord(void);
void cord_RISO_onePhaseToPE(void);
void cord_meas_correct_wiring(void);
void cord_RISO_phasesToPE(void);
void cord_RISO_phaseToPhase(void);
void cord_continuity_test(void);
void CordRPEStart(void);
void CordRPEStop(void);



//ne spreminjaj makrojev!!!
//----------------------------NASTAVITVE--------------------------------------
//avtomatsko detektiranje za kolk polni kabl gre
#define AUTO_DETECT_P_NUM	false				//avtomatska detekcija stevila faz
#define MULTI_FAULT_CONTINUE  _OFF		//ce je tole vklopljeno se meritev izvede do konca tudi ce ze ugotovi multifault
#define MAX_ERROR_ADD_COMMAND	7				//stevilo oddanih errorjev locenih z ',' v koncnem sporocilu
#define RISO_TEST_ON_FLAG				_OFF	//ali gremo na RISO test ali ne
#define CORD_AUTO_CONTINUE_MEAS	_OFF	//dolocimo ali zelimo avtomatsko nadaljevati cord meritve po vrstnem redu, ce je nastavljeno na _OFF potem to pomeni, da nadaljevanje meritve zahteva zunanja naprava npr. MT-310
//----------------------------------------------------------------------------

//
#define _1_PHASE 1
#define _3_PHASE 3

//+++++++++++++++++++++++++++zastavice za connection_reg+++++++++++++++++++++++++++++
#define L1_L1		0x00000001
#define L1_L2		0x00000002
#define L1_L3		0x00000004
#define L1_N		0x00000008
#define L1_PE		0x00000010
#define L2_L3		0x00000020
#define L2_N		0x00000040
#define L2_PE		0x00000080
#define L3_N		0x00000100
#define L3_PE		0x00000200
#define N_PE		0x00000400
#define L2_L2		0x00000800
#define L3_L3		0x00001000
#define N_N			0x00002000
#define PE_PE		0x00004000
//obrnjeno
#define L2_L1		0x00008000
#define L3_L1		0x00010000
#define N_L1		0x00020000
#define PE_L1		0x00040000
#define L3_L2		0x00080000
#define N_L2		0x00100000
#define PE_L2		0x00200000
#define N_L3		0x00400000
#define PE_L3		0x00800000
#define PE_N		0x01000000
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


#define PASS_MASK 					0x00007801
#define L1_OPEN_MASK 				0x00007800
#define L2_OPEN_MASK 				0x00007001
#define L3_OPEN_MASK 				0x00006801
#define N_OPEN_MASK 				0x00005801
#define PE_OPEN_MASK 				0x00003801
#define L1_OPEN_1P_MASK			0x00006000
#define N_OPEN_1P_MASK			0x00004001
#define PE_OPEN_1P_MASK			0x00002001
#define PASS_1P_MASK 				0x00006001
#define L1_CROSSED_MASK 		0x0000001E
#define L1_CROSSED_MASK2 		0x00078000
#define L1_L2_CROSSED_MASK 	0x00000002
#define L1_L3_CROSSED_MASK 	0x00000004
#define L1_N_CROSSED_MASK 	0x00000008
#define L1_PE_CROSSED_MASK 	0x00000010
#define L2_CROSSED_MASK 		0x000000E0
#define L2_CROSSED_MASK2 		0x00388000
#define L2_L3_CROSSED_MASK 	0x00000020
#define L2_N_CROSSED_MASK 	0x00000040
#define L2_PE_CROSSED_MASK 	0x00000080
#define L3_CROSSED_MASK 		0x00000300
#define L3_CROSSED_MASK2 		0x00C90000
#define L3_N_CROSSED_MASK 	0x00000100
#define L3_PE_CROSSED_MASK 	0x00000200
#define N_CROSSED_MASK			0x00000548
#define N_CROSSED_MASK2			0x01520000
#define N_PE_CROSSED_MASK 	0x00000400
#define PE_CROSSED_MASK			0x00000690
#define PE_CROSSED_MASK2		0x01A40000


//++++++++++++++++++++zastavice za connection_status++++++++++++++++++++++++++++++++++
#define L1_OPEN 				0x00000001
#define L1_L2_CROSSED		0x00000002
#define L1_L2_SHORTED		0x00000004
#define L1_L3_CROSSED		0x00000008
#define L1_L3_SHORTED		0x00000010
#define L1_N_CROSSED		0x00000020
#define L1_N_SHORTED		0x00000040
#define L1_PE_CROSSED		0x00000080
#define L1_PE_SHORTED		0x00000100
#define L2_L3_CROSSED		0x00000200
#define L2_L3_SHORTED		0x00000400
#define L2_N_CROSSED		0x00000800
#define L2_N_SHORTED		0x00001000
#define L2_PE_CROSSED		0x00002000
#define L2_PE_SHORTED		0x00004000
#define L3_N_CROSSED		0x00008000
#define L3_N_SHORTED		0x00010000
#define L3_PE_CROSSED		0x00020000
#define L3_PE_SHORTED		0x00040000
#define L2_OPEN					0x00080000
#define L3_OPEN					0x00100000
#define N_OPEN					0x00200000
#define PE_OPEN					0x00400000
#define N_PE_CROSSED		0x00800000
#define L1_OK						0x01000000
#define L2_OK						0x02000000
#define L3_OK						0x04000000
#define N_OK						0x08000000
#define PE_OK						0x10000000
#define N_PE_SHORTED		0x20000000
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define L1_SHORTED_MASK	0x00000154
#define L2_SHORTED_MASK	0x00005404
#define L3_SHORTED_MASK	0x00050410
#define N_SHORTED_MASK	0x20011040
#define PE_SHORTED_MASK	0x20044180
#define ALL_OPEN_MASKS	0x00780001
#define ALL_SHORTED_MASKS	0x20055554
#define ALL_CROSSED_MASKS	0x0082AAAA
#define STATUS_FAULT_MASK	0x20FFFFFF
#define STATUS2_FAULT_CROSSED_MASK	0x00003FF0
#define STATUS_FAULT_SHORTED_MASK	0x20055554
#define STATUS_FAULT_OPEN_MASK		0x00780001
#define STATUS2_FAULT_1P_CROSSED_MASK	0x00002480
#define STATUS_FAULT_1P_SHORTED_MASK	0x20000140
#define STATUS_FAULT_1P_OPEN_MASK		0x00600001
#define STATUS_OK_1P_MASK	0x19000000
#define STATUS_OK_3P_MASK	0x1F000000

#define L1_CROSS_CON_MASK	0x000000AA
#define L2_CROSS_CON_MASK	0x00002A02
#define L3_CROSS_CON_MASK	0x00280208
#define N_CROSS_CON_MASK	0x00808820
#define PE_CROSS_CON_MASK	0x00822080

#define CABLE_1P_MASK					0x39E001E1	//vse kar se tice enofaznega kabla pazi L2_open in L3_OPEN nista upostevana
#define CABLE_1P_FAULT_MASK_S1	0x20E001E1
#define CABLE_1P_FAULT_MASK_S2	0x00832480
#define CABLE_3P_FAULT_MASK_S1	0x20FFFFFF
#define CABLE_3P_FAULT_MASK_S2	0x00FFC000
#define CABLE_1P_CROSSED_LOW_MASKS 	0x008000A0
#define CABLE_1P_CROSSED_HIGH_MASKS 0x00830000
#define CABLE_3P_CROSSED_LOW_MASKS 	0x0082AAAA
#define CABLE_3P_CROSSED_HIGH_MASKS 0x00FFC000

//++++++++++++++++++++++++++++++zastavice za connection_status2+++++++++++++++++++++++++++++++
#define MULTI_FAULT					0x00000001
#define CROSSED_AND_SHORTED			0x00000002
#define MULTI_SHORTED				0x00000004
#define CROSSED_AND_OPEN			0x00000008
#define L1_L2_CRIS_CROSSED			0x00000010
#define L1_L3_CRIS_CROSSED			0x00000020
#define L2_L3_CRIS_CROSSED			0x00000040
#define L1_N_CRIS_CROSSED			0x00000080
#define L2_N_CRIS_CROSSED			0x00000100
#define L3_N_CRIS_CROSSED			0x00000200
#define L1_PE_CRIS_CROSSED			0x00000400
#define L2_PE_CRIS_CROSSED			0x00000800
#define L3_PE_CRIS_CROSSED			0x00001000
#define N_PE_CRIS_CROSSED			0x00002000
#define L2_L1_CROSSED				0x00004000
#define L3_L1_CROSSED				0x00008000
#define N_L1_CROSSED				0x00010000
#define PE_L1_CROSSED				0x00020000
#define L3_L2_CROSSED				0x00040000
#define N_L2_CROSSED				0x00080000
#define PE_L2_CROSSED				0x00100000
#define N_L3_CROSSED				0x00200000
#define PE_L3_CROSSED				0x00400000
#define PE_N_CROSSED				0x00800000
#define SHORTED_AND_OPEN			0x01000000
#define MULTI_CROSSED				0x02000000
#define MULTI_OPENED				0x04000000
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//++++++++++++++++++++++++++++++zastavice za insolation_status+++++++++++++++++++++++++++++++
#define L1_PE_FAIL					0x00000001
#define L2_PE_FAIL					0x00000002
#define L3_PE_FAIL					0x00000004
#define N_PE_FAIL						0x00000008
#define L1_N_FAIL						0x00000010
#define L2_N_FAIL						0x00000020
#define L3_N_FAIL						0x00000040
#define L1_L2_FAIL					0x00000080
#define L2_L3_FAIL					0x00000100
#define L1_L3_FAIL					0x00000200
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define CORD_ONE_PHASE_TO_PE_MASK		0x0000000F
#define CORD_PHASE_TO_PHASE_MASK		0x000003F0

//++++++++++++++++++++++++++++++zastavice za cord_task_control+++++++++++++++++++++++++++++++
#define	__CORD_RPE_RES_REQUESTED									0x00000002
#define	__CORD_RISO_RES_REQUESTED									0x00000004
#define	__CORD_CORECT_WIRING_MEASURED							0x00000008
#define	__CORD_CONTINUITY_MEASURED								0x00000010
#define	__CORD_RISO_PHASES_TO_PE_MEASURED					0x00000020
#define	__CORD_RISO_ONE_PHASE_TO_PE_MEASURED			0x00000040
#define	__CORD_RISO_PHASE_TO_PHASE_MEASURED				0x00000080
#define	__CORD_INITIATED													0x00000100
#define	__CORD_INIT_RECIEVED											0x00000200
#define	__CORD_CORRECT_WIRING_IN_PROGRESS					0x00000400
#define	__CORD_RISO_PHASES_TO_PE_IN_PROGRESS			0x00000800
#define	__CORD_RISO_ONE_PHASE_TO_PE_IN_PROGRESS		0x00001000
#define	__CORD_RISO_PHASE_TO_PHASE_IN_PROGRESS		0x00002000
#define	__CORD_CONTINUITY_IN_PROGRESS							0x00004000
#define __CORD_RISO_STARTED												0x00008000
#define __CORD_RPE_L_STARTED											0x00010000
#define __CORD_RPE_H_STARTED											0x00020000
#define __CORD_REINIT															0x00040000
#define __CORD_RPE_M_STARTED											0x00080000
#define __CORD_RPE_IN_PROGRESS										0x00100000
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


#define CORD_MEAS_IN_PROG_MASK			0x00107C00				
#define CORD_RISO_MASKS							0x000038E4
#define CORD_MEAS_MASKS							0x0003FCFE




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
