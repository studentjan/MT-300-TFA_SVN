#include "welding.h"
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
#include "com_meas_tasks.h"
#include "stm32f3xx_hal.h"


static struct connected_device device;
extern uint32_t connection_control;
extern uint32_t meas_task_control;
extern uint32_t global_control;
extern uint32_t synchro_interrupt_control;
extern uint32_t current_URES_measurement;


float weld_riso_resistance = 0;
	
void WeldingInit(void);
void init_weld(void);
void WeldingRPEStart(void);
void WeldingRPEStop(void);

void weld_RISO_init(void);
void weld_transmittPhasesToPE(bool pass);
void weld_transmittOnePhaseToPE(void);
void weld_transmittMainsToWeld(bool pass);
void weld_transmittMainsToClass2(bool pass);
void weld_transmittWeldToPE(bool pass);
bool weld_check_RISO_resistance(void);
void WeldRstRels(void);
void weld_UNL_init(void);

uint32_t weld_task_control;
uint32_t weld_insolation_status;

uint32_t weld_RISO_count=0;
uint32_t weld_RISO_count2=0;
uint32_t start_weld_count=0;
uint32_t weld_URES_count=0;

uint32_t WELD_PHASE_NUM_SETTING = _3_PHASE;


void set_phase_num_weld(int phase_num)
{
	if(phase_num == 1)
		WELD_PHASE_NUM_SETTING = _1_PHASE;
	else
		WELD_PHASE_NUM_SETTING = _3_PHASE;
}

void init_weld(void)
{
	if(start_weld_count == 0)
	{
		global_control &= (~__INIT_TEST_PASS);
		//weld_task_control = 0;
		global_control |= __ON_TEST_IN_PROG;
		set_event(POWER_ON_TEST,power_on_test);
		start_weld_count++;
		set_timer(INIT_WELD,5,init_weld);
		device = get_connected_device();
	}
	else if(start_weld_count == 1)
	{
		if(!(global_control & __ON_TEST_IN_PROG))
		{
			if(global_control & __INIT_TEST_PASS)
			{
				start_weld_count++;
				set_event(INIT_WELD,init_weld);
			}
			else if(global_control & __INIT_TEST_FAIL)	//ce pride do napake skocimo tle not
			{
				set_event(SEND_TFA_MAINS_STATUS,send_mains_status);
				SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__WELD_INIT_FAILED__,"","",device.device_dir);
				start_weld_count = 0;
			}
			setNormal();
		}
		else 
			restart_timer(INIT_WELD,5,init_weld);
	}
	else if(start_weld_count == 2 )
	{
		start_weld_count=0;
		WeldingInit();
		set_event(SEND_TFA_MAINS_STATUS,send_mains_status);
		weld_task_control |= __WELD_INITIATED;
		SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__WELD_INITIATED__,"","",device.device_dir);
		//ce ne dobimo inita in hocemo direktno zagnat correct wiring se najprej izvede init nato pa gre na correct wiring
		if((WELD_AUTO_CONTINUE_MEAS==_ON)||(!(weld_task_control & __WELD_INIT_RECIEVED)))
			restart_timer(WELD_RPE_START,2,WeldingRPEStart);
	}
}

void WeldingInit(void)
{
	weld_task_control &= (~WELD_MEAS_MASKS);
	weld_insolation_status=0;
	weld_RISO_count=0;
	weld_URES_count=0;
	if(connection_control & __CON_TO_MT310)
	{
		//postavimo vse releje v nevtralno stanje, tako da vemo kaksno situacijo imamo
		WeldMachMt310_RelInit();
	}
	
}
void deinitWelding(void)
{
	end_task(WELD_RPE_START);
	end_task(INIT_WELD);
	end_task(WELD_RISO_PHASES_TO_PE);
	end_task(WELD_RISO_ONE_PHASE_TO_PE);
//	end_task(WELD_URES_STOP);
	start_weld_count=0;
	weld_task_control=0;
	weld_insolation_status=0;
	weld_RISO_count=0;
	weld_URES_count=0;
	setNormal();
	disable_sinchro_interrupt(__URES_SYNCHRO);
	meas_task_control &= ~__WELD_MEAS_IN_PROG;
	SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__DEINITIATED__,"","",device.device_dir);
}
void stop_weld(void)
{
	WeldingInit();
	disable_sinchro_interrupt(__URES_SYNCHRO);
	weld_RISO_count2=0;
	end_task(WELD_RPE_START);
	end_task(INIT_WELD);
	end_task(WELD_RISO_PHASES_TO_PE);
	end_task(WELD_RISO_ONE_PHASE_TO_PE);
//	end_task(WELD_URES_STOP);
	SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__STOPED_C__,"","",device.device_dir);
}

void WeldRstRels(void)
{
	RST_L1_CONTACTOR;
	RST_L2_CONTACTOR;
	RST_L3_CONTACTOR;
	RST_N_CONTACTOR;
	RST_PE_CONTACTOR;
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
	rst_REL(23);
	rst_REL(24);
	rst_REL(29);
	rst_REL(26);
	rst_REL(25);
	rst_REL(9);
	rst_REL(27);
	rst_REL(28);
}

//++++++++++++++++++++++++++++++++++++++++++++RPE+++++++++++++++++++++++++++++++++++++++++++
void WeldingRPEStart(void)
{
	WeldRstRels();
	set_REL(8);
	weld_task_control |= __WELD_RPE_IN_PROGRESS;
	SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__WELD_RPE_STARTED__,"","",device.device_dir);
}

void WeldingRPEStop(void)
{
	rst_REL(8);
	weld_task_control &= ~__WELD_RPE_IN_PROGRESS;
	weld_task_control |= __WELD_RPE_MEASURED;
	SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__WELD_RPE_STOPPED__,"","",device.device_dir);
}
//+++++++++++++++++++++++++++++++++++++++++++++END RPE+++++++++++++++++++++++++++++++++++


//+++++++++++++++++++++++++++++++++++++++++++++++RISO (MAINS - PE)+++++++++++++++++++++++++++++++++++++++++++++++++
void weld_RISO_init(void)
{
	weld_RISO_count=0;
	weld_insolation_status=0;
	weld_task_control &= (~WELD_RISO_PE_MASKS);
	if(connection_control & __CON_TO_MT310)
	{
		WeldRstRels();
		set_REL(9);
		set_REL(27);
		set_REL(28);
	}
	
}
void weld_RISO_phasesToPE(void)
{
	switch(weld_RISO_count)
	{
		case 0:
			weld_RISO_init();
			SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__PHASES_TO_PE_STARTED__,"","",device.device_dir);
			weld_task_control |=__WELD_RISO_PHASES_TO_PE_IN_PROGRESS;
			SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__START_RISO__,"","",device.device_dir);
			weld_RISO_count++;
			break;
		case 1:
			if(weld_task_control & __WELD_RISO_STARTED)
			{
				if(WELD_PHASE_NUM_SETTING == _1_PHASE)
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
				weld_task_control |= __WELD_RISO_RES_REQUESTED;
				SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__GET_RISO_RES__,"","PHASES_PE",device.device_dir);
				weld_RISO_count++;
			}
			break;
		case 2://L1,L2,L3,N proti PE ali L1,N proti PE za enofazin podalsek
			if(!(weld_task_control&__WELD_RISO_RES_REQUESTED))
			{
				
				SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__STOP_RISO__,"","",device.device_dir);
				weld_RISO_count++;
			}
		break;
	}
	if(weld_RISO_count < 3)
		restart_timer(WELD_RISO_PHASES_TO_PE,5,weld_RISO_phasesToPE);
	else
	{
		if(weld_task_control & __WELD_RISO_STARTED)
			restart_timer(WELD_RISO_PHASES_TO_PE,5,weld_RISO_phasesToPE);
		else if(WELD_AUTO_CONTINUE_MEAS==_ON)
			set_timer(WELD_RISO_ONE_PHASE_TO_PE,5,weld_RISO_onePhaseToPE);
		if(!(weld_task_control & __WELD_RISO_STARTED))
		{
			weld_task_control &=(~__WELD_RISO_PHASES_TO_PE_IN_PROGRESS);
			weld_task_control |= __WELD_RISO_PHASES_TO_PE_MEASURED;
			if(weld_check_RISO_resistance())
			{
				weld_transmittPhasesToPE(true);
				weld_RISO_count=11;
			}
			else
			{
				weld_transmittPhasesToPE(false);
			}
		}
	}
}

// v naslednjo funkcijo pridemo samo ce je prej ze pomerjena upornost proti PE in je bila ta premajhna
void weld_RISO_onePhaseToPE(void)
{
	switch (weld_RISO_count)
	{
		case 0:
			weld_RISO_init();
			SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__PHASES_TO_PE_STARTED__,"","",device.device_dir);
			weld_task_control |=__WELD_RISO_PHASES_TO_PE_IN_PROGRESS;
			SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__START_RISO__,"","",device.device_dir);
			weld_RISO_count++;
			break;
		case 1:
			if(!(weld_task_control & __WELD_RISO_STARTED))
			{
				if(WELD_PHASE_NUM_SETTING == _1_PHASE)
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
				weld_task_control |= __WELD_RISO_RES_REQUESTED;
				SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__GET_RISO_RES__,"","PHASES_PE",device.device_dir);
				weld_RISO_count++;
			}
			break;
		case 2://L1,L2,L3,N proti PE ali L1,N proti PE za enofazin podalsek
			if(weld_task_control & __WELD_RISO_STARTED)
			{
				if(weld_check_RISO_resistance())
				{
					weld_transmittPhasesToPE(true);
				}
				else
				{
					weld_transmittPhasesToPE(false);
				}
				weld_RISO_count++;
				weld_task_control &=(~__WELD_RISO_PHASES_TO_PE_IN_PROGRESS);
				weld_task_control |= __WELD_RISO_PHASES_TO_PE_MEASURED;
			}
		break;
		case 3:
			SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__ONE_PHASE_TO_PE_STARTED__,"","",device.device_dir);
			weld_task_control |= __WELD_RISO_ONE_PHASE_TO_PE_IN_PROGRESS;
			if(!(weld_task_control & __WELD_RISO_STARTED))
				SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__START_RISO__,"","",device.device_dir);
			//weld_task_control |= __WELD_RISO_RES_REQUESTED;
			weld_RISO_count++;
			//SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__START_RISO__,"","",device.device_dir);
			break;
		case 4:	
			if(weld_task_control & __WELD_RISO_STARTED)
			{
				if(WELD_PHASE_NUM_SETTING == _1_PHASE)
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
					weld_RISO_count=7;
					weld_task_control |= __WELD_RISO_RES_REQUESTED;
					SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__GET_RISO_RES__,"","N_PE",device.device_dir);
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
					weld_RISO_count++;
					weld_task_control |= __WELD_RISO_RES_REQUESTED;
					SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__GET_RISO_RES__,"","L2-L3-N_PE",device.device_dir);
				}
			}
			break;
		case 5:	//L2,L3,N proti PE
			if(!(weld_task_control&__WELD_RISO_RES_REQUESTED))
			{
				if(weld_check_RISO_resistance())
				{
					weld_RISO_count=11;
					weld_insolation_status |= L1_PE_FAIL;
					DIS_PE_B;
					DIS_N_A;
					CON_L1_A;
					CON_N_B;
					SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__STOP_RISO__,"","",device.device_dir);
				}
				else
				{
					DIS_L2_A;
					weld_RISO_count++;
					weld_task_control |= __WELD_RISO_RES_REQUESTED;
					SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__GET_RISO_RES__,"","L3-N_PE",device.device_dir);
				}
				
			}
			break;
		case 6:	//L3,N proti PE
			if(!(weld_task_control&__WELD_RISO_RES_REQUESTED))
			{
				if(weld_check_RISO_resistance())
				{
					weld_RISO_count=10;
					weld_insolation_status |= L2_PE_FAIL;
					DIS_L3_A;
					DIS_N_A;
					CON_L1_A;
					weld_task_control |= __WELD_RISO_RES_REQUESTED;
					SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__GET_RISO_RES__,"","L1_PE",device.device_dir);
				}
				else
				{
					DIS_L3_A;
					weld_RISO_count++;
					weld_task_control |= __WELD_RISO_RES_REQUESTED;
					SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__GET_RISO_RES__,"","N_PE",device.device_dir);
				}
				
			}
			break;
		case 7:	//N proti PE
			if(!(weld_task_control&__WELD_RISO_RES_REQUESTED))
			{
				if(WELD_PHASE_NUM_SETTING == _1_PHASE)
				{
					if(weld_check_RISO_resistance())
					{
						weld_RISO_count=11;
						weld_insolation_status |= L1_PE_FAIL;
					}
					else
					{
						weld_insolation_status |= N_PE_FAIL;
						weld_RISO_count=11;
					}
					SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__STOP_RISO__,"","",device.device_dir);
					DIS_N_A;
					DIS_PE_B;
					CON_L1_A;
					CON_N_B;
				}
				else
				{
					if(weld_check_RISO_resistance())
					{
						weld_RISO_count=9;
						weld_insolation_status |= L3_PE_FAIL;
						DIS_N_A;
						CON_L2_A;
						weld_task_control |= __WELD_RISO_RES_REQUESTED;
						SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__GET_RISO_RES__,"","L2_PE",device.device_dir);
					}
					else
					{
						weld_insolation_status |= N_PE_FAIL;
						DIS_N_A;
						CON_L3_A;
						weld_RISO_count++;
						weld_task_control |= __WELD_RISO_RES_REQUESTED;
						SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__GET_RISO_RES__,"","L3_PE",device.device_dir);
					}
				}	
			}
			break;
		case 8:	//L3 proti PE
		if(!(weld_task_control&__WELD_RISO_RES_REQUESTED))
		{
			if(!(weld_check_RISO_resistance()))
				weld_insolation_status |= L3_PE_FAIL;
			weld_RISO_count++;
			DIS_L3_A;
			CON_L2_A;
		}
		case 9:	//L2 proti PE
		if(!(weld_task_control&__WELD_RISO_RES_REQUESTED))
		{
			if(!(weld_check_RISO_resistance()))
				weld_insolation_status |= L2_PE_FAIL;
			weld_RISO_count++;
			DIS_L2_A;
			CON_L1_A;
			weld_task_control |= __WELD_RISO_RES_REQUESTED;
			SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__GET_RISO_RES__,"","L1_PE",device.device_dir);
		}
		break;
		case 10:	//L1 proti PE
		if(!(weld_task_control&__WELD_RISO_RES_REQUESTED))
		{
			if(!(weld_check_RISO_resistance()))
				weld_insolation_status |= L1_PE_FAIL;
			weld_RISO_count++;
			SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__STOP_RISO__,"","",device.device_dir);
		}
		break;
	}
	if(weld_RISO_count <= 10)
		restart_timer(WELD_RISO_ONE_PHASE_TO_PE,5,weld_RISO_onePhaseToPE);
	else
	{
		//rezultat poslejmo sele ko nam vrne riso stopped
		if(weld_task_control & __WELD_RISO_STARTED)
			restart_timer(WELD_RISO_ONE_PHASE_TO_PE,5,weld_RISO_onePhaseToPE);
		else
		{
			weld_transmittOnePhaseToPE();
			weld_task_control |= __WELD_RISO_ONE_PHASE_TO_PE_MEASURED;
			weld_task_control &= (~__WELD_RISO_ONE_PHASE_TO_PE_IN_PROGRESS);
		}
	}
}

void weld_transmittPhasesToPE(bool pass)
{
	char temp_str[MAX_ADDITIONAL_COMMANDS_LENGTH];
	if(pass)
	{
		sprintf(temp_str,__PASS__);
	}
	else
	{
		sprintf(temp_str,__FAIL__);
	}
	SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__RISO_PHASES_TO_PE__,temp_str,"",device.device_dir);
} 
void weld_transmittOnePhaseToPE(void)
{
	char temp_str[MAX_ADDITIONAL_COMMANDS_LENGTH];
	if(weld_insolation_status & WELD_ONE_PHASE_TO_PE_MASK)
	{
		sprintf(temp_str,__FAIL__);
		if(weld_insolation_status & L1_PE_FAIL)
		{
			strcat(temp_str,",");
			strcat(temp_str,__L1_PE_FAIL__);
		}
		if(weld_insolation_status & L2_PE_FAIL)
		{
			strcat(temp_str,",");
			strcat(temp_str,__L2_PE_FAIL__);
		}
		if(weld_insolation_status & L3_PE_FAIL)
		{
			strcat(temp_str,",");
			strcat(temp_str,__L3_PE_FAIL__);
		}
		if(weld_insolation_status & N_PE_FAIL)
		{
			strcat(temp_str,",");
			strcat(temp_str,__N_PE_FAIL__);
		}
	}
	else
	{
		sprintf(temp_str,__PASS__);
	}
	SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__RISO_ONE_PHASE_TO_PE__,temp_str,"",device.device_dir);
}


void set_RISO_weld_resistance(char* value)
{
	weld_riso_resistance = (float)atof(value);
	weld_task_control &= (~__WELD_RISO_RES_REQUESTED);
}
bool weld_check_RISO_resistance(void)
{
	if(weld_riso_resistance < WELD_RISO_LIMIT)
		return false;
	else
		return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++END RISO (MAINS - PE)+++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++RISO (MAINS - WELDING)++++++++++++++++++++++++++++
void weld_RISO_MainsToWeldClass2_init(void)
{
	weld_RISO_count=0;
	//weld_task_control &= (~WELD_RISO_MASKS);
	if(connection_control & __CON_TO_MT310)
	{
		WeldRstRels();
	}
	
}
void weld_RISO_MainsToWeld(void)
{
	if(weld_task_control & __WELD_RISO_CONTINIOUS_MEAS)
	{
		weld_RISO_MainsToWeldClass2_init();
		if(WELD_PHASE_NUM_SETTING == _1_PHASE)
		{
			CON_L1_A;
			CON_N_A;
			set_REL(25);
			set_REL(26);
		}	
		else 
		{
			CON_L1_A;
			CON_L2_A;
			CON_L3_A;
			CON_N_A;
			set_REL(25);
			set_REL(26);
		}
		weld_task_control |=__WELD_RISO_MAINS_WELD_IN_PROGRESS;
		SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__MAINS_TO_WELD_STARTED__,"","",device.device_dir);
	}
	else
	{
		switch(weld_RISO_count2)
		{
			case 0:
				weld_RISO_MainsToWeldClass2_init();
				SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__MAINS_TO_WELD_STARTED__,"","",device.device_dir);
				weld_task_control |=__WELD_RISO_MAINS_WELD_IN_PROGRESS;
				SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__START_RISO__,"","",device.device_dir);
				weld_RISO_count2++;
				break;
			case 1:
				if(weld_task_control & __WELD_RISO_STARTED)
				{
					if(WELD_PHASE_NUM_SETTING == _1_PHASE)
					{
						CON_L1_A;
						CON_N_A;
						set_REL(25);
						set_REL(26);
					}	
					else 
					{
						CON_L1_A;
						CON_L2_A;
						CON_L3_A;
						CON_N_A;
						set_REL(25);
						set_REL(26);
					}
					weld_task_control |= __WELD_RISO_RES_REQUESTED;
					SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__GET_RISO_RES__,"","MAINS-WELD",device.device_dir);
					weld_RISO_count2++;
				}
				break;
			case 2://L1,L2,L3,N proti PE ali L1,N proti PE za enofazin podalsek
				if(!(weld_task_control&__WELD_RISO_RES_REQUESTED))
				{
					
					SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__STOP_RISO__,"","",device.device_dir);
					weld_RISO_count2++;
				}
			break;
		}
		if(weld_RISO_count2 < 3)
			restart_timer(WELD_RISO_MAINS_TO_WELD,5,weld_RISO_MainsToWeld);
		else
		{
			if(weld_task_control & __WELD_RISO_STARTED)
				restart_timer(WELD_RISO_MAINS_TO_WELD,5,weld_RISO_MainsToWeld);
			else if(WELD_AUTO_CONTINUE_MEAS==_ON)
				set_timer(WELD_RISO_WELD_TO_PE,5,weld_RISO_WeldToPE);
			if(!(weld_task_control & __WELD_RISO_STARTED))
			{
				weld_task_control &=(~__WELD_RISO_MAINS_WELD_IN_PROGRESS);
				if(weld_check_RISO_resistance())
				{
					weld_transmittMainsToWeld(true);
				}
				else
				{
					weld_transmittMainsToWeld(false);
				}
				weld_RISO_count2=0;
			}
		}
	}
}
void weld_RISO_MainsToWeld_Stop(void)
{
	weld_RISO_MainsToWeldClass2_init();
	weld_task_control &= ~__WELD_RISO_MAINS_WELD_IN_PROGRESS;
	SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__MAINS_TO_WELD_STOPPED__,"","",device.device_dir);
	weld_RISO_count2=0;
}
void weld_transmittMainsToWeld(bool pass)
{
	char temp_str[MAX_ADDITIONAL_COMMANDS_LENGTH];
	if(pass)
	{
		sprintf(temp_str,__PASS__);
	}
	else
	{
		sprintf(temp_str,__FAIL__);
	}
	SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__MAINS_TO_WELD_RESULT__,temp_str,"",device.device_dir);
} 
//+++++++++++++++++++++++++++++++++++++++++++++END RISO (MAINS - WELDING)+++++++++++++++++++++++

//+++++++++++++++++++++++++++++++++++++++++++++RISO (WELDING - PE)+++++++++++++++++++++++++++++
void weld_RISO_WeldToPE(void)
{
	if(weld_task_control & __WELD_RISO_CONTINIOUS_MEAS)
	{
		weld_RISO_MainsToWeldClass2_init();
		set_REL(25);
		set_REL(26);
		set_REL(8);
		weld_task_control |=__WELD_RISO_WELD_PE_IN_PROGRESS;
		SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__WELD_TO_PE_STARTED__,"","",device.device_dir);
	}
	else
	{
		switch(weld_RISO_count2)
		{
			case 0:
				weld_RISO_MainsToWeldClass2_init();
				SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__WELD_TO_PE_STARTED__,"","",device.device_dir);
				weld_task_control |= __WELD_RISO_WELD_PE_IN_PROGRESS;
				SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__START_RISO__,"","",device.device_dir);
				weld_RISO_count2++;
				break;
			case 1:
				if(weld_task_control & __WELD_RISO_STARTED)
				{
					set_REL(25);
					set_REL(26);
					set_REL(8);
					weld_task_control |= __WELD_RISO_RES_REQUESTED;
					SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__GET_RISO_RES__,"","WELD-PE",device.device_dir);
					weld_RISO_count2++;
				}
				break;
			case 2:
				if(!(weld_task_control&__WELD_RISO_RES_REQUESTED))
				{
					SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__STOP_RISO__,"","",device.device_dir);
					weld_RISO_count2++;
				}
			break;
		}
		if(weld_RISO_count2 < 3)
			restart_timer(WELD_RISO_WELD_TO_PE,5,weld_RISO_WeldToPE);
		else
		{
			if(weld_task_control & __WELD_RISO_STARTED)
				restart_timer(WELD_RISO_WELD_TO_PE,5,weld_RISO_WeldToPE);
			else if(WELD_AUTO_CONTINUE_MEAS==_ON)
				set_timer(WELD_RISO_MAINS_TO_CLASS2,5,weld_RISO_MainsToClass2);
			if(!(weld_task_control & __WELD_RISO_STARTED))
			{
				weld_task_control &=(~__WELD_RISO_WELD_PE_IN_PROGRESS);
				if(weld_check_RISO_resistance())
				{
					weld_transmittWeldToPE(true);
				}
				else
				{
					weld_transmittWeldToPE(false);
				}
				weld_RISO_count2=0;
			}
		}
	}
}
void weld_RISO_WeldToPE_Stop(void)
{
	weld_RISO_MainsToWeldClass2_init();
	weld_task_control &= ~__WELD_RISO_WELD_PE_IN_PROGRESS;
	SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__WELD_TO_PE_STOPPED__,"","",device.device_dir);
	weld_RISO_count2=0;
}
void weld_transmittWeldToPE(bool pass)
{
	char temp_str[MAX_ADDITIONAL_COMMANDS_LENGTH];
	if(pass)
	{
		sprintf(temp_str,__PASS__);
	}
	else
	{
		sprintf(temp_str,__FAIL__);
	}
	SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__WELD_TO_PE_RESULT__,temp_str,"",device.device_dir);
} 
//++++++++++++++++++++++++++++++++++++++++++END RISO (WELDING - PE)+++++++++++++++++++++++++

//+++++++++++++++++++++++++++++++++++++++++++++RISO (MAINS - CLASS2)+++++++++++++++++++++++++++++
void weld_RISO_MainsToClass2(void)
{
	if(weld_task_control & __WELD_RISO_CONTINIOUS_MEAS)
	{
		weld_RISO_MainsToWeldClass2_init();
		if(WELD_PHASE_NUM_SETTING == _1_PHASE)
		{
			CON_L1_A;
			CON_N_A;
			set_REL(9);
		}	
		else 
		{
			CON_L1_A;
			CON_L2_A;
			CON_L3_A;
			CON_N_A;
			set_REL(9);
		}
		weld_task_control |=__WELD_RISO_MAINS_CLASS2_IN_PROGRESS;
		SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__MAINS_TO_CLASS2_STARTED__,"","",device.device_dir);
	}
	else
	{
		switch(weld_RISO_count2)
		{
			case 0:
				weld_RISO_MainsToWeldClass2_init();
				SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__MAINS_TO_CLASS2_STARTED__,"","",device.device_dir);
				weld_task_control |=__WELD_RISO_MAINS_CLASS2_IN_PROGRESS;
				SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__START_RISO__,"","",device.device_dir);
				weld_RISO_count2++;
				break;
			case 1:
				if(weld_task_control & __WELD_RISO_STARTED)
				{
					if(WELD_PHASE_NUM_SETTING == _1_PHASE)
					{
						CON_L1_A;
						CON_N_A;
						set_REL(9);
					}	
					else 
					{
						CON_L1_A;
						CON_L2_A;
						CON_L3_A;
						CON_N_A;
						set_REL(9);
					}
					weld_task_control |= __WELD_RISO_RES_REQUESTED;
					SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__GET_RISO_RES__,"","MAINS-CLASS2",device.device_dir);
					weld_RISO_count2++;
				}
				break;
			case 2://L1,L2,L3,N proti PE ali L1,N proti PE za enofazin podalsek
				if(!(weld_task_control&__WELD_RISO_RES_REQUESTED))
				{
					
					SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__STOP_RISO__,"","",device.device_dir);
					weld_RISO_count2++;
				}
			break;
		}
		if(weld_RISO_count2 < 3)
			restart_timer(WELD_RISO_MAINS_TO_CLASS2,5,weld_RISO_MainsToClass2);
		else
		{
			if(weld_task_control & __WELD_RISO_STARTED)
				restart_timer(WELD_RISO_MAINS_TO_CLASS2,5,weld_RISO_MainsToClass2);
			if(!(weld_task_control & __WELD_RISO_STARTED))
			{
				weld_task_control &=(~__WELD_RISO_MAINS_CLASS2_IN_PROGRESS);
				if(weld_check_RISO_resistance())
				{
					weld_transmittMainsToClass2(true);
				}
				else
				{
					weld_transmittMainsToClass2(false);
				}
				weld_RISO_count2=0;
			}
		}
	}
}
void weld_RISO_MainsToClass2_Stop(void)
{
	weld_RISO_MainsToWeldClass2_init();
	weld_task_control &= ~__WELD_RISO_MAINS_CLASS2_IN_PROGRESS;
	SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__MAINS_TO_CLASS2_STOPPED__,"","",device.device_dir);
	weld_RISO_count2=0;
}
void weld_transmittMainsToClass2(bool pass)
{
	char temp_str[MAX_ADDITIONAL_COMMANDS_LENGTH];
	if(pass)
	{
		sprintf(temp_str,__PASS__);
	}
	else
	{
		sprintf(temp_str,__FAIL__);
	}
	SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__MAINS_TO_CLASS2_RESULT__,temp_str,"",device.device_dir);
} 
//++++++++++++++++++++++++++++++++++++++++++END RISO (MAINS - CLASS2)+++++++++++++++++++++++++

//+++++++++++++++++++++++++++++++++++++++++++++U-NO-LOAD (PEAK)++++++++++++++++++++++++++++++++++++++
void weld_UNL_init(void)
{
	weld_RISO_count=0;
	//weld_task_control &= (~WELD_RISO_MASKS);
	if(connection_control & __CON_TO_MT310)
	{
		WeldRstRels();
	}
	
}

void weld_UnlStart_peak(void)
{
	weld_UNL_init();
	set_REL(24);
	set_REL(26);
	synchroSetContactor(__SET_L1_CONTACTOR);
	synchroSetContactor(__SET_L2_CONTACTOR);
	synchroSetContactor(__SET_L3_CONTACTOR);
	synchroSetContactor(__SET_N_CONTACTOR);
	synchroSetContactor(__SET_PE_CONTACTOR);
	weld_task_control |= __WELD_UNL_PEAK_IN_PROGRESS;
	SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__WELD_UNL_PEAK_STARTED_,"","",device.device_dir);
}
void weld_UnlStop_peak(void)
{
	WeldRstRels();
	weld_task_control &= ~__WELD_UNL_PEAK_IN_PROGRESS;
	SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__WELD_UNL_PEAK_STOPED_,"","",device.device_dir);
}
//+++++++++++++++++++++++++++++++++++++++++++END U-NO-LOAD (PEAK)++++++++++++++++++++++++++++++++++++++

//+++++++++++++++++++++++++++++++++++++++++++++U-NO-LOAD (RMS)++++++++++++++++++++++++++++++++++++++

void weld_UnlStart_RMS(void)
{
	weld_UNL_init();
	set_REL(24);
	set_REL(26);
	synchroSetContactor(__SET_L1_CONTACTOR);
	synchroSetContactor(__SET_L2_CONTACTOR);
	synchroSetContactor(__SET_L3_CONTACTOR);
	synchroSetContactor(__SET_N_CONTACTOR);
	synchroSetContactor(__SET_PE_CONTACTOR);
	weld_task_control |= __WELD_UNL_RMS_IN_PROGRESS;
	SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__WELD_UNL_RMS_STARTED_,"","",device.device_dir);
}
void weld_UnlStop_RMS(void)
{
	WeldRstRels();
	weld_task_control &= ~__WELD_UNL_RMS_IN_PROGRESS;
	SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__WELD_UNL_RMS_STOPED_,"","",device.device_dir);
}
//+++++++++++++++++++++++++++++++++++++++++++END U-NO-LOAD (RMS)++++++++++++++++++++++++++++++++++++++