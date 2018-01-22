#ifndef __COM_MEAS_TASKS_H
#define __COM_MEAS_TASKS_H

void setNormal(void);
void WeldMachMt310_RelInit(void);
void startMainsMeasurement(void);
void returnMeasuredTask(void);
void stopMainsMeasurement(void);

#define _1_PHASE 1
#define _3_PHASE 3


#define URES_PERIODS_TO_WAIT			10
//cas odklopa kontaktorja je odvisen od trenutne napetosti na kontaktorju in je od cca 17
//rezultati pomerjeni v matlabu file three_phase_sim.slx
#define URES_TIME_TO_DIS_L1			135		//n*100us
#define URES_TIME_TO_DIS_L2			246	//n*100us
#define URES_TIME_TO_DIS_L3			71	//n*100us
#define URES_TIME_TO_DIS_L1_L2	145	
#define URES_TIME_TO_DIS_L1_L3	100		
#define URES_TIME_TO_DIS_L2_L3	226	
#define URES_TIME_L_ON_N_ADD		100//-100;	//dodatek ce je enofazni sistem z fazo na n vodniku (pol periode)


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


//--------------------------------current_URES_measurement-----------------------------
#define __L1_PE		1
#define __L2_PE		2
#define __L3_PE		3
#define __L1_N		4
#define __L2_N		5
#define __L3_N		6
#define __L1_L2		7
#define __L1_L3		8
#define __L2_L3		9
#define __TIMER_INIT	10	//uporabljamo samo ob inicializaciji timerja

#endif
