
#include "machines.h"
#include "defines.h"
#include "rel_driver.h"
#include "stdint.h"
#include "defines.h"
#include "serial_com.h"
#include "do_task.h"
#include "tasks.h"
#include "os.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "do_task.h"


extern uint32_t connection_control;
extern uint32_t meas_task_control;
extern uint32_t global_control;
uint32_t mach_task_control;
uint32_t mach_insolation_status;
uint32_t current_URES_measurement=0;

float mach_riso_resistance=0;


void MachunesRPEInit(void);
void mach_RISO_onePhaseToPE(void);
void mach_RISO_phasesToPE(void);
void mach_RISO_init(void);
void mach_transmittOnePhaseToPE(void);
void mach_transmittPhasesToPE(bool pass);
void MachinesInit(void);
void mach_URES_init(void);

uint32_t MACH_PHASE_NUM_SETTING = _3_PHASE;

uint32_t mach_RISO_count=0;
uint32_t start_mach_count=0;
uint32_t mach_URES_count=0;

static struct connected_device device;


void set_phase_num_mach(int phase_num)
{
	if(phase_num == 1)
		MACH_PHASE_NUM_SETTING = _1_PHASE;
	else
		MACH_PHASE_NUM_SETTING = _3_PHASE;
}

void init_mach(void)
{
	if(start_mach_count == 0)
	{
		global_control &= (~__INIT_TEST_PASS);
		mach_task_control = 0;
		mach_insolation_status=0;
		mach_RISO_count=0;
		mach_RISO_count=0;
		set_event(POWER_ON_TEST,power_on_test);
		start_mach_count++;
		set_timer(INIT_MACH,5,init_mach);
		device = get_connected_device();
	}
	else if(start_mach_count == 1)
	{
		if(global_control & __INIT_TEST_PASS)
		{
			start_mach_count++;
			set_event(INIT_CORD,init_mach);
		}
		else if(global_control & __INIT_TEST_FAIL)	//ce pride do napake skocimo tle not
		{
			set_event(SEND_TFA_MAINS_STATUS,send_mains_status);
			start_mach_count = 0;
		}
		else 
			restart_timer(INIT_MACH,5,init_mach);
	}
	else if(start_mach_count == 2)
	{
		start_mach_count=0;
		MachinesInit();
		set_event(SEND_TFA_MAINS_STATUS,send_mains_status);
		mach_task_control |= __MACH_INITIATED;
		SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__MACHINES__,__MACH_INITIATED__,"",device.device_dir);
		//ce ne dobimo inita in hocemo direktno zagnat correct wiring se najprej izvede init nato pa gre na correct wiring
		if((MACH_AUTO_CONTINUE_MEAS==_ON)||(!(mach_task_control & __MACH_INIT_RECIEVED)))
			restart_timer(MACH_RPE_START,2,MachinesRPEStart);
	}
}
void MachinesInit(void)
{
	mach_task_control = 0;
	mach_insolation_status=0;
	mach_RISO_count=0;
	if(connection_control & __CON_TO_MT310)
	{
		//postavimo vse releje v nevtralno stanje, tako da vemo kaksno situacijo imamo
		rst_REL(2);
		rst_REL(3);
		rst_REL(4);
		rst_REL(5);
		rst_REL(8);
		rst_REL(30);
		rst_REL(10);
		rst_REL(11);
		rst_REL(12);
		rst_REL(13);
		rst_REL(33);
		rst_REL(34);
		rst_REL(35);
		rst_REL(36);
		
		rst_REL(17);
		rst_REL(18);
		rst_REL(19);
		rst_REL(20);
		rst_REL(22);
		
		rst_REL(39);
		rst_REL(40);
		rst_REL(41);
		rst_REL(42);
		rst_REL(29);
		
		rst_REL(26);
		rst_REL(25);
		rst_REL(24);
		rst_REL(23);
		rst_REL(15);
		
		rst_REL(16);
		rst_REL(14);
		rst_REL(7);
		rst_REL(42);
		rst_REL(29);
		
		rst_REL(9);
		rst_REL(27);
		rst_REL(28);
		rst_REL(6);		//PE kontaktor ima NC kontakt, zato ga izklopimo
	}
	
}

void MachinesRPEStart(void)
{
	set_REL(8);
	mach_task_control |= __MACH_RPE_IN_PROGRESS;
	SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__MACHINES__,__MECH_RPE_STARTED__,"",device.device_dir);
}

void MachinesRPEStop(void)
{
	rst_REL(8);
	mach_task_control &= ~__MACH_RPE_IN_PROGRESS;
	mach_task_control |= __MACH_RPE_MEASURED;
	SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__MACHINES__,__MECH_RPE_STOPPED__,"",device.device_dir);
}

void mach_RISO_init(void)
{
	mach_RISO_count=0;
	mach_insolation_status=0;
	mach_task_control &= (~MACH_RISO_MASKS);
	if(connection_control & __CON_TO_MT310)
	{
		rst_REL(2);
		rst_REL(3);
		rst_REL(4);
		rst_REL(5);
		rst_REL(8);
		rst_REL(10);
		rst_REL(11);
		rst_REL(12);
		rst_REL(13);
		rst_REL(30);
		rst_REL(33);
		rst_REL(34);
		rst_REL(35);
		rst_REL(36);
		set_REL(6);
		set_REL(9);
		set_REL(27);
		set_REL(28);
	}
	
}
void mach_RISO_phasesToPE(void)
{
	switch(mach_RISO_count)
	{
		case 0:
			mach_RISO_init();
			SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__MACHINES__,__PHASES_TO_PE_STARTED__,"",device.device_dir);
			mach_task_control |=__MACH_RISO_PHASES_TO_PE_IN_PROGRESS;
			SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__MACHINES__,__START_RISO__,"",device.device_dir);
			mach_RISO_count++;
			break;
		case 1:
			if(mach_task_control & __MACH_RISO_STARTED)
			{
				if(MACH_PHASE_NUM_SETTING == _1_PHASE)
				{
					CON_L1_A;
					CON_N_A;
					CON_PE_B;
				}	
				else 
				{
					CON_L1_A;
					CON_L2_A;
					CON_L3_A;
					CON_N_A;
					CON_PE_B;
				}
				mach_task_control |= __MACH_RISO_RES_REQUESTED;
				SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__MACHINES__,__GET_RISO_RES__,"PHASES_PE",device.device_dir);
				mach_RISO_count++;
			}
			break;
		case 2://L1,L2,L3,N proti PE ali L1,N proti PE za enofazin podalsek
			if(!(mach_task_control&__MACH_RISO_RES_REQUESTED))
			{
				
				SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__MACHINES__,__STOP_RISO__,"",device.device_dir);
				mach_RISO_count++;
			}
		break;
	}
	if(mach_RISO_count < 3)
		restart_timer(CORD_RISO_PHASES_TO_PE,5,mach_RISO_phasesToPE);
	else
	{
		if(mach_task_control & __MACH_RISO_STARTED)
			restart_timer(MACH_RISO_PHASES_TO_PE,5,mach_RISO_phasesToPE);
		else if(MACH_AUTO_CONTINUE_MEAS==_ON)
			set_timer(MACH_RISO_ONE_PHASE_TO_PE,5,mach_RISO_onePhaseToPE);
		if(!(mach_task_control & __MACH_RISO_STARTED))
		{
			mach_task_control &=(~__MACH_RISO_PHASES_TO_PE_IN_PROGRESS);
			mach_task_control |= __MACH_RISO_PHASES_TO_PE_MEASURED;
			if(mach_check_RISO_resistance())
			{
				mach_transmittPhasesToPE(true);
				mach_RISO_count=11;
			}
			else
			{
				mach_transmittPhasesToPE(false);
			}
		}
	}
}

// v naslednjo funkcijo pridemo samo ce je prej ze pomerjena upornost proti PE in je bila ta premajhna
void mach_RISO_onePhaseToPE(void)
{
	if((mach_RISO_count<3)&&(!(mach_task_control & __MACH_RISO_PHASES_TO_PE_IN_PROGRESS))&&(!(mach_task_control & __MACH_RISO_STARTED)))
		SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__MACHINES__,__START_RISO__,"",device.device_dir);
	else if((mach_RISO_count>=3)&&(!(mach_task_control & __MACH_RISO_ONE_PHASE_TO_PE_IN_PROGRESS))&&((mach_task_control & __MACH_RISO_PHASES_TO_PE_MEASURED))&&(!(mach_task_control & __MACH_RISO_STARTED)))
		SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__MACHINES__,__START_RISO__,"",device.device_dir);
	if(mach_task_control & __MACH_RISO_STARTED)
	{
		switch (mach_RISO_count)
		{
			case 0:
				mach_RISO_init();
				SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__MACHINES__,__PHASES_TO_PE_STARTED__,"",device.device_dir);
				mach_task_control |=__MACH_RISO_PHASES_TO_PE_IN_PROGRESS;
				mach_RISO_count++;
				break;
			case 1:
				if(MACH_PHASE_NUM_SETTING == _1_PHASE)
				{
					CON_L1_A;
					CON_N_A;
					CON_PE_B;
				}	
				else 
				{
					CON_L1_A;
					CON_L2_A;
					CON_L3_A;
					CON_N_A;
					CON_PE_B;
				}
				mach_task_control |= __MACH_RISO_RES_REQUESTED;
				SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__MACHINES__,__GET_RISO_RES__,"PHASES_PE",device.device_dir);
				mach_RISO_count++;
				break;
			case 2://L1,L2,L3,N proti PE ali L1,N proti PE za enofazin podalsek
				if(!(mach_task_control&__MACH_RISO_RES_REQUESTED))
				{
					if(mach_check_RISO_resistance())
					{
						mach_transmittPhasesToPE(true);
					}
					else
					{
						mach_transmittPhasesToPE(false);
					}
					mach_RISO_count++;
					mach_task_control &=(~__MACH_RISO_PHASES_TO_PE_IN_PROGRESS);
					mach_task_control |= __MACH_RISO_PHASES_TO_PE_MEASURED;
				}
			break;
			case 3:
				SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__MACHINES__,__ONE_PHASE_TO_PE_STARTED__,"",device.device_dir);
				mach_task_control |= __MACH_RISO_ONE_PHASE_TO_PE_IN_PROGRESS;
				//mach_task_control |= __MACH_RISO_RES_REQUESTED;
				mach_RISO_count++;
				//SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__MACHINES__,__START_RISO__,"",device.device_dir);
				break;
			case 4:	
				if(MACH_PHASE_NUM_SETTING == _1_PHASE)
				{
					DIS_PE_A;
					DIS_L1_A;
					DIS_L1_B;
					DIS_L2_B;
					DIS_L3_B;
					DIS_N_B;
					DIS_L2_A;
					DIS_L3_A;
					CON_N_A;
					CON_PE_B;
					mach_RISO_count=7;
					mach_task_control |= __MACH_RISO_RES_REQUESTED;
					SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__MACHINES__,__GET_RISO_RES__,"N_PE",device.device_dir);
				}
				else
				{
					DIS_PE_A;
					DIS_L1_A;
					DIS_L1_B;
					DIS_L2_B;
					DIS_L3_B;
					DIS_N_B;
					CON_L2_A;
					CON_L3_A;
					CON_N_A;
					CON_PE_B;
					mach_RISO_count++;
					mach_task_control |= __MACH_RISO_RES_REQUESTED;
					SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__MACHINES__,__GET_RISO_RES__,"L2-L3-N_PE",device.device_dir);
				}

				break;
			case 5:	//L2,L3,N proti PE
				if(!(mach_task_control&__MACH_RISO_RES_REQUESTED))
				{
					if(mach_check_RISO_resistance())
					{
						mach_RISO_count=11;
						mach_insolation_status |= L1_PE_FAIL;
						DIS_PE_B;
						DIS_N_A;
						CON_L1_A;
						CON_N_B;
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__MACHINES__,__STOP_RISO__,"",device.device_dir);
					}
					else
					{
						DIS_L2_A;
						mach_RISO_count++;
						mach_task_control |= __MACH_RISO_RES_REQUESTED;
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__MACHINES__,__GET_RISO_RES__,"L3-N_PE",device.device_dir);
					}
					
				}
				break;
			case 6:	//L3,N proti PE
				if(!(mach_task_control&__MACH_RISO_RES_REQUESTED))
				{
					if(mach_check_RISO_resistance())
					{
						mach_RISO_count=10;
						mach_insolation_status |= L2_PE_FAIL;
						DIS_L3_A;
						DIS_N_A;
						CON_L1_A;
						mach_task_control |= __MACH_RISO_RES_REQUESTED;
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__MACHINES__,__GET_RISO_RES__,"L1_PE",device.device_dir);
					}
					else
					{
						DIS_L3_A;
						mach_RISO_count++;
						mach_task_control |= __MACH_RISO_RES_REQUESTED;
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__MACHINES__,__GET_RISO_RES__,"N_PE",device.device_dir);
					}
					
				}
				break;
			case 7:	//N proti PE
				if(!(mach_task_control&__MACH_RISO_RES_REQUESTED))
				{
					if(MACH_PHASE_NUM_SETTING == _1_PHASE)
					{
						if(mach_check_RISO_resistance())
						{
							mach_RISO_count=11;
							mach_insolation_status |= L1_PE_FAIL;
						}
						else
						{
							mach_insolation_status |= N_PE_FAIL;
							mach_RISO_count=11;
						}
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__MACHINES__,__STOP_RISO__,"",device.device_dir);
						DIS_N_A;
						DIS_PE_B;
						CON_L1_A;
						CON_N_B;
					}
					else
					{
						if(mach_check_RISO_resistance())
						{
							mach_RISO_count=9;
							mach_insolation_status |= L3_PE_FAIL;
							DIS_N_A;
							CON_L2_A;
							mach_task_control |= __MACH_RISO_RES_REQUESTED;
							SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__MACHINES__,__GET_RISO_RES__,"L2_PE",device.device_dir);
						}
						else
						{
							mach_insolation_status |= N_PE_FAIL;
							DIS_N_A;
							CON_L3_A;
							mach_RISO_count++;
							mach_task_control |= __MACH_RISO_RES_REQUESTED;
							SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__MACHINES__,__GET_RISO_RES__,"L3_PE",device.device_dir);
						}
					}	
				}
				break;
			case 8:	//L3 proti PE
			if(!(mach_task_control&__MACH_RISO_RES_REQUESTED))
			{
				if(!(mach_check_RISO_resistance()))
					mach_insolation_status |= L3_PE_FAIL;
				mach_RISO_count++;
				DIS_L3_A;
				CON_L2_A;
			}
			case 9:	//L2 proti PE
			if(!(mach_task_control&__MACH_RISO_RES_REQUESTED))
			{
				if(!(mach_check_RISO_resistance()))
					mach_insolation_status |= L2_PE_FAIL;
				mach_RISO_count++;
				DIS_L2_A;
				CON_L1_A;
				mach_task_control |= __MACH_RISO_RES_REQUESTED;
				SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__MACHINES__,__GET_RISO_RES__,"L1_PE",device.device_dir);
			}
			break;
			case 10:	//L1 proti PE
			if(!(mach_task_control&__MACH_RISO_RES_REQUESTED))
			{
				if(!(mach_check_RISO_resistance()))
					mach_insolation_status |= L1_PE_FAIL;
				mach_RISO_count++;
				SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__MACHINES__,__STOP_RISO__,"",device.device_dir);
			}
			break;
		}
	}
	if(mach_RISO_count <= 10)
		restart_timer(CORD_RISO_ONE_PHASE_TO_PE,5,mach_RISO_onePhaseToPE);
	else
	{
		//rezultat poslejmo sele ko nam vrne riso stopped
		if(mach_task_control & __MACH_RISO_STARTED)
			restart_timer(CORD_RISO_ONE_PHASE_TO_PE,5,mach_RISO_onePhaseToPE);
		else
		{
			mach_transmittOnePhaseToPE();
			mach_task_control |= __MACH_RISO_ONE_PHASE_TO_PE_MEASURED;
			mach_task_control &= (~__MACH_RISO_ONE_PHASE_TO_PE_IN_PROGRESS);
		}
	}
}

void mach_transmittPhasesToPE(bool pass)
{
	char temp_str[MAX_ADDITIONAL_COMMANDS_LENGTH];
	sprintf(temp_str,__RISO_PHASES_TO_PE__);
	if(pass)
	{
		strcat(temp_str,"|");
		strcat(temp_str,__PASS__);
	}
	else
	{
		strcat(temp_str,"|");
		strcat(temp_str,__FAIL__);
	}
	SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__MACHINES__,temp_str,"",device.device_dir);
} 
void mach_transmittOnePhaseToPE(void)
{
	char temp_str[MAX_ADDITIONAL_COMMANDS_LENGTH];
	sprintf(temp_str,__RISO_ONE_PHASE_TO_PE__);
	if(mach_insolation_status & MACH_ONE_PHASE_TO_PE_MASK)
	{
		strcat(temp_str,"|");
		strcat(temp_str,__FAIL__);
		if(mach_insolation_status & L1_PE_FAIL)
		{
			strcat(temp_str,",");
			strcat(temp_str,__L1_PE_FAIL__);
		}
		if(mach_insolation_status & L2_PE_FAIL)
		{
			strcat(temp_str,",");
			strcat(temp_str,__L2_PE_FAIL__);
		}
		if(mach_insolation_status & L3_PE_FAIL)
		{
			strcat(temp_str,",");
			strcat(temp_str,__L3_PE_FAIL__);
		}
		if(mach_insolation_status & N_PE_FAIL)
		{
			strcat(temp_str,",");
			strcat(temp_str,__N_PE_FAIL__);
		}
	}
	else
	{
		strcat(temp_str,"|");
		strcat(temp_str,__PASS__);
	}
	SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__MACHINES__,temp_str,"",device.device_dir);
}


void set_RISO_mach_resistance(char* value)
{
	mach_riso_resistance = (float)atof(value);
	mach_task_control &= (~__MACH_RISO_RES_REQUESTED);
}
bool mach_check_RISO_resistance(void)
{
	if(mach_riso_resistance < MACH_RISO_LIMIT)
		return false;
	else
		return true;
}

void mach_URES_init(void)
{
	mach_URES_count=0;
	mach_task_control &= (~MACH_URES_MASKS);
	if(connection_control & __CON_TO_MT310)
	{
		set_REL(2);
		set_REL(3);
		set_REL(4);
		set_REL(5);
		rst_REL(8);
		rst_REL(10);
		rst_REL(11);
		rst_REL(12);
		rst_REL(13);
		rst_REL(30);
		rst_REL(33);
		rst_REL(34);
		rst_REL(35);
		rst_REL(36);
		rst_REL(39);
		rst_REL(40);
		rst_REL(6);
		rst_REL(9);
		rst_REL(17);
		rst_REL(18);
		rst_REL(19);
		rst_REL(20);
		set_REL(27);
		set_REL(22);
		rst_REL(28);
		set_REL(16);
	}
}
	
void mach_URES(void)
{
	if(mach_URES_count==0)
	{
		mach_URES_init();
	}
	switch(current_URES_measurement)
	{
		case __L1_PE: 
			rst_REL(11);
			rst_REL(12);
			rst_REL(13);
			rst_REL(33);
			rst_REL(34);
			rst_REL(35);
			rst_REL(36);
			rst_REL(20);
			set_REL(15);
			set_REL(10);
			break;
		case __L2_PE: 
			rst_REL(10);
			rst_REL(12);
			rst_REL(13);
			rst_REL(33);
			rst_REL(34);
			rst_REL(35);
			rst_REL(36);
			rst_REL(20);
			set_REL(15);
			set_REL(11);
			break;
		case __L3_PE: 
			rst_REL(10);
			rst_REL(11);
			rst_REL(13);
			rst_REL(33);
			rst_REL(34);
			rst_REL(35);
			rst_REL(36);
			rst_REL(20);
			set_REL(15);
			set_REL(12);
			break;
		case __L1_N: 
			rst_REL(11);
			rst_REL(12);
			rst_REL(13);
			rst_REL(33);
			rst_REL(34);
			rst_REL(35);
			rst_REL(36);
			rst_REL(15);
			set_REL(20);
			set_REL(10);
			break;
		case __L2_N: 
			rst_REL(10);
			rst_REL(12);
			rst_REL(13);
			rst_REL(33);
			rst_REL(34);
			rst_REL(35);
			rst_REL(36);
			rst_REL(15);
			set_REL(20);
			set_REL(11);
			break;
		case __L3_N: 
			rst_REL(11);
			rst_REL(10);
			rst_REL(13);
			rst_REL(33);
			rst_REL(34);
			rst_REL(35);
			rst_REL(36);
			rst_REL(15);
			set_REL(20);
			set_REL(12);
			break;
		case __L1_L2: 
			rst_REL(11);
			rst_REL(12);
			rst_REL(13);
			rst_REL(33);
			rst_REL(34);
			rst_REL(35);
			rst_REL(36);
			rst_REL(15);
			rst_REL(17);
			rst_REL(19);
			rst_REL(20);
			set_REL(10);
			set_REL(18);
			break;
		case __L1_L3: 
			rst_REL(11);
			rst_REL(12);
			rst_REL(13);
			rst_REL(33);
			rst_REL(34);
			rst_REL(35);
			rst_REL(36);
			rst_REL(15);
			rst_REL(18);
			rst_REL(17);
			rst_REL(20);
			set_REL(10);
			set_REL(19);
			break;
		case __L2_L3: 
			rst_REL(10);
			rst_REL(12);
			rst_REL(13);
			rst_REL(33);
			rst_REL(34);
			rst_REL(35);
			rst_REL(36);
			rst_REL(15);
			rst_REL(18);
			rst_REL(17);
			rst_REL(20);
			set_REL(11);
			set_REL(19);
			break;
		default: break;
	}
}	


void stop_mach(void)
{
	meas_task_control &= (~__MACH_MEAS_IN_PROG);
	rst_REL(6);
	rst_REL(2);
	rst_REL(3);
	rst_REL(4);
	rst_REL(5);
	rst_REL(10);
	rst_REL(11);
	rst_REL(12);
	rst_REL(13);
	rst_REL(9);
	rst_REL(27);
	rst_REL(28);
	rst_REL(30);
	rst_REL(33);
	rst_REL(34);
	rst_REL(35);
	rst_REL(36);
	SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__MACHINES__,__STOPED_C__,"",device.device_dir);
}