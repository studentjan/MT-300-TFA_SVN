#include "do_task.h"
#include "defines.h"
#include "os.h"
#include "tasks.h"
#include "rel_driver.h"
#include "comunication.h"
#include "cord.h"
#include "serial_com.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "com_meas_tasks.h"

//--------------spremenljivke--------------------
extern uint32_t global_control;
extern uint32_t connection_control;
extern uint32_t init_test_counter;
extern uint32_t meas_task_control;
uint32_t cord_task_control=0;
static float cord_rpe_resistance;
static float cord_riso_resistance;
static uint32_t connection_reg=0;
static uint32_t connection_status =0;
static uint32_t connection_status2=0;
static uint32_t insolation_status=0;
static uint32_t sent_command_id=0;
static struct connected_device device;

//---------------------LIMITE------------------------
static float RpeContinuityLimit 		=	 		RPE_CONTINUITY_FAIL_LIMIT;
static float CordRisoLimit					=		 	CORD_RISO_LIMIT;
static float CordRpeLimit						=		 	CORD_RPE_LIMIT;
//---------------------------------------------------


bool cord_check_RPE_resistance(void);
bool cord_check_RISO_resistance(void);
void cord_get_and_transmitt_result(void);
void cord_transmittOnePhaseToPE(void);
void cord_transmittPhaseToPhase(void);
void cord_transmittPhasesToPE(bool pass);
void cord_transmittContiunuity(bool pass);
void tramsmitt_corect_wiring_state(uint32_t dir, char device_ID);
uint32_t numberOfSetBits(uint32_t i);

bool set_next_cord_task_case=false;

//spremenljivke za nastavitve
bool manual_set_phase_num = !(AUTO_DETECT_P_NUM);	//ce je na true potem rocno nastavimo ali kontroliramo 3p ali 1p kabel, ce je false pa se to naredi avtomatsko
int CORD_PHASE_NUM_SETTING = _3_PHASE;

uint32_t cord_cw_count=0;
uint32_t cord_RISO_count=0;
uint32_t cord_continuity_count=0;
int start_cord_count=0;
uint32_t cord_err_count;
float cord_continuity_L1_L1_res=0;
float cord_continuity_L2_L2_res=0;
float cord_continuity_L3_L3_res=0;
float cord_continuity_N_N_res=0;
float cord_continuity_PE_PE_res=0;

void set_phase_num(int phase_num)
{
	device = get_connected_device();
	if(phase_num == 1)
		CORD_PHASE_NUM_SETTING = _1_PHASE;
	else
		CORD_PHASE_NUM_SETTING = _3_PHASE;
	//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__SET_COMPLETED__,"","",device.device_dir);
}
void init_cord(void)
{
	if(start_cord_count == 0)
	{
		global_control &= (~__INIT_TEST_PASS);
		init_test_counter=0;
		cord_RISO_count=0;
		cord_continuity_count=0;
		global_control |= __ON_TEST_IN_PROG;
		set_event(POWER_ON_TEST,power_on_test);
		start_cord_count++;
		set_timer(INIT_CORD,5,init_cord);
		device = get_connected_device();
	}
	else if(start_cord_count == 1)
	{
		if(!(global_control & __ON_TEST_IN_PROG))
		{
			if(global_control & __INIT_TEST_PASS)
			{
				start_cord_count++;
				set_event(INIT_CORD,init_cord);
			}
			else //ce pride do napake skocimo tle not
			{
				set_event(SEND_TFA_MAINS_STATUS,send_mains_status);
//				SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__CORD_INIT_FAILED__,"","",device.device_dir);
				start_cord_count = 0;
			}
			setNormal();
		}
		else 
			restart_timer(INIT_CORD,5,init_cord);
	}
	else if(start_cord_count == 2)
	{
		start_cord_count=0;
		cord_global_init_set();
		cord_correct_wiring_init();
		set_event(SEND_TFA_MAINS_STATUS,send_mains_status);
		cord_task_control &= ~__CORD_REINIT;
		cord_task_control |= __CORD_INITIATED;
		meas_task_control |= __CORD_MEAS_IN_PROG;
//		SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__CORD_INITIATED__,"","",device.device_dir);
		//ce ne dobimo inita in hocemo direktno zagnat correct wiring se najprej izvede init nato pa gre na correct wiring
//		if((CORD_AUTO_CONTINUE_MEAS==_ON)||(!(cord_task_control & __CORD_INIT_RECIEVED)))
//			restart_timer(CORD_MEAS_CORRECT_WIRING,2,cord_meas_correct_wiring);
	}
}
void deinitCord(void)
{
	end_task(CORD_MEAS_CORRECT_WIRING);
	end_task(INIT_CORD);
	end_task(CORD_RISO_PHASES_TO_PE);
	end_task(CORD_RISO_ONE_PHASE_TO_PE);
	end_task(CORD_RISO_PHASE_TO_PHASE);
	end_task(CORD_MEAS_CONTINUITY);
	cord_RISO_count=0;
	cord_continuity_count=0;
	start_cord_count=0;
	cord_task_control=0;
	insolation_status=0;
	connection_status2=0;
	connection_status=0;
	connection_reg=0;
	setNormal();
	meas_task_control &= ~__CORD_MEAS_IN_PROG;
//	SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__DEINITIATED__,"","",device.device_dir);
}
void cord_meas_correct_wiring(void)
{
	if(checkAndChangeMeasurement(__CORD))
	{
	//	static uint32_t cord_cord_cw_count2=0;
		switch(cord_cw_count)
		{
			case 0:
			{
				cord_task_control |= __CORD_CORRECT_WIRING_IN_PROGRESS;
				//posljemo ukaz za start
				if(cord_task_control & __CORD_INITIATED)
				{
//					SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__CORD_CW_STARTED__,"","",device.device_dir);
//					SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__START_RPE_LOW__,"","",device.device_dir);		
					cord_correct_wiring_init();
					cord_cw_count++;
				}
				else
					set_event(INIT_CORD,init_cord);
				break;
			}
			case 1:
			{
//				if(cord_task_control & __CORD_RPE_L_STARTED)
//				{
					transmitCommFunc(__RPE,__START,__LOW,"L1_L1",device.device_dir);	//pri low meritvi se rezultat vrne ko se meritev starta
					cord_task_control |= __CORD_RPE_RES_REQUESTED;
//					transmitCommFunc(__RPE,__GET,__RESULT,"L1_PE",device.device_dir);"","L1_L1",device.device_dir);
//					cord_task_control |= __CORD_RPE_RES_REQUESTED;
					cord_cw_count++;
//				}
				break;
			}
			case 2://L1 L1 POVEZANA?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					cord_task_control |= __CORD_RPE_L_STARTED;
					if(cord_check_RPE_resistance())
					{
						//ce je L1 na L1 preverimo ce ni slucajn se kje drgje
						connection_reg |= L1_L1;
					}
					else
					{
						//ce ni L1 direktno na L1 pogledamo ce je mogoce povezan kam drugam
						connection_reg &= (~L1_L1);
					}
					rst_REL(33);
					set_REL(30);
					cord_cw_count++;
					transmitCommFunc(__RPE,__GET,__RESULT,"L1_PE",device.device_dir);
					cord_task_control |= __CORD_RPE_RES_REQUESTED;
				}
				break;
			}
			case 3://L1 PE POVEZANA?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						if(connection_reg & L1_L1)	//ce ne ustreza temu pogoju je lahko crossed
						{
							;
						}
						else
						{
							//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
							if(connection_reg & L1_CROSSED_MASK)
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= CROSSED_AND_SHORTED;
							}
	//						connection_status |= L1_PE_CROSSED;
							//ne smemos e postaviti zastavice za crossed, ker se ne vemo vseh pogojev
						}
	//					if(connection_status2 & MULTI_FAULT)
	//					{
	//						if(MULTI_FAULT_CONTINUE)
	//						{
	//							cord_cw_count++;
	//							rst_REL(30);
	//							set_REL(36);
	//						}
	//						else 
	//							cord_cw_count =100;	//skocmmo vn iz zanke ko zaznamo multifault
	//					}
	//					else
	//					{
							rst_REL(30);
							set_REL(36);
							cord_cw_count++;
	//					}
						connection_reg |= L1_PE;
					}
					else
					{
	//					if(connection_status2 & MULTI_FAULT)
	//					{
	//						if(MULTI_FAULT_CONTINUE)
	//						{
	//							cord_cw_count++;
	//							rst_REL(30);
	//							set_REL(36);
	//						}
	//						else 
	//							cord_cw_count =100;	//skocmmo vn iz zanke ko zaznamo multifault
	//					}
	//					else
	//					{
							rst_REL(30);
							set_REL(36);
							cord_cw_count++;
	//					}
						connection_reg &= (~L1_PE);
					}
					if(((connection_status2 & MULTI_FAULT)&&MULTI_FAULT_CONTINUE)|| !(connection_status2 & MULTI_FAULT))
					{
						if(cord_cw_count == 4)transmitCommFunc(__RPE,__GET,__RESULT,"L1_N",device.device_dir);
						cord_task_control |= __CORD_RPE_RES_REQUESTED;
					}
				}
				break;
			}
			case 4://L1 N POVEZANA?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						if(connection_reg & L1_L1)
						{
							if(connection_reg & L1_PE)//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_SHORTED;
							}
							//connection_status |= L1_N_SHORTED;
						}
						else
						{
							//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
							if(connection_reg & L1_PE)
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= CROSSED_AND_SHORTED;
							}
	//						connection_status |= L1_N_CROSSED;
							//ne smemos e postaviti zastavice za crossed, ker se ne vemo vseh pogojev
						}
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else
								cord_cw_count =100;
						}
						else
						{
							set_next_cord_task_case = true;
						}
						connection_reg |= L1_N;
					}
					else
					{
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else
								cord_cw_count =100;
						}
						else
						{
							set_next_cord_task_case = true;
						}
						connection_reg &= (~L1_N);
					}
					if(set_next_cord_task_case == true)
					{
						set_next_cord_task_case = false;
						if(manual_set_phase_num && (CORD_PHASE_NUM_SETTING==_1_PHASE))
						{
							rst_REL(10);
							set_REL(13);
							cord_cw_count=14;//gremo naprej na N-N
							//preverimo kaj je z 1P povezavo
	//						if((!(connection_reg & L1_L1)))
	//						{
	//							switch (connection_reg & L1_CROSSED_MASK)
	//							{
	//								case L1_N_CROSSED_MASK: connection_status |= L1_N_CROSSED; break;
	//								case L1_PE_CROSSED_MASK: connection_status |= L1_PE_CROSSED; break;
	//								case 0: connection_status |= L1_OPEN; break;	//vse ok
	//								default: 
	//									connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
	//									connection_status2 |= CROSSED_AND_SHORTED;
	//							}
	//						}
	//						//ce ni crossed in shorted potem pomeni, da je OK
	//						else
	//						{
	//							switch (connection_reg & (L1_CROSSED_MASK))
	//							{
	//								case L1_N_CROSSED_MASK: break;	//shorted - ampak se ne smem postavt shorted zastavice
	//								case L1_PE_CROSSED_MASK: break;
	//								case 0: connection_status |= L1_OK; break;	//vse ok
	//								default: 
	//									connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
	//									connection_status2 |= CROSSED_AND_SHORTED;
	//							}
	//						}
							if(!(connection_reg & L1_L1))
							{
								if((numberOfSetBits(connection_reg & (L1_CROSSED_MASK | L1_CROSSED_MASK2)))==0)
								{
									connection_status |= L1_OPEN; 
								}
								else if((numberOfSetBits(connection_reg & (L1_CROSSED_MASK | L1_CROSSED_MASK2)))==1)
								{
									switch (connection_reg & (L1_CROSSED_MASK | L1_CROSSED_MASK2))
									{
										case L1_PE: connection_status |= L1_PE_CROSSED; break;	
										case L1_N: connection_status |= L1_N_CROSSED; break;
										default: break;									
									}
								}
								else if((numberOfSetBits(connection_reg & (L1_CROSSED_MASK | L1_CROSSED_MASK2)))==2)
								{
									if(connection_reg & L1_N)connection_status |= L1_N_CROSSED;
									if(connection_reg & L1_PE)connection_status |= L1_PE_CROSSED;
									if((numberOfSetBits(connection_reg & L1_CROSSED_MASK))>1)	//pogledamo ce jih vec postavljenih na eni strani
									{
										connection_status2 |= MULTI_FAULT;
										connection_status2 |= CROSSED_AND_SHORTED;	
									}
									else if((numberOfSetBits(connection_reg & L1_CROSSED_MASK2))>1)
									{
										connection_status2 |= MULTI_FAULT;	
										connection_status2 |= CROSSED_AND_SHORTED;	
									}
									else 
									{
										connection_status2 |= MULTI_FAULT;	
										connection_status2 |= MULTI_CROSSED;	
									}
								}
								else
								{
									if(connection_reg & L1_N)connection_status |= L1_N_CROSSED;
									if(connection_reg & L1_PE)connection_status |= L1_PE_CROSSED;
									connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
									connection_status2 |= CROSSED_AND_SHORTED;
								}
							}
							//ce ni crossed in shorted potem pomeni, da je OK
							else
							{
								if(!((numberOfSetBits(connection_reg & (L1_CROSSED_MASK | L1_CROSSED_MASK2)))>0))
									connection_status |= L1_OK;
							}
						}
						else
						{
							rst_REL(36);
							set_REL(35);
							cord_cw_count++;
						}
					}
					if(((connection_status2 & MULTI_FAULT)&&MULTI_FAULT_CONTINUE)|| !(connection_status2 & MULTI_FAULT))
					{
						if(cord_cw_count==5)transmitCommFunc(__RPE,__GET,__RESULT,"L1_L3",device.device_dir);
						else if(cord_cw_count==14)transmitCommFunc(__RPE,__GET,__RESULT,"N_N",device.device_dir);
						cord_task_control |= __CORD_RPE_RES_REQUESTED;
					}
				}
				break;
			}
			case 5: //L1 L3 POVEZANA?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						if(connection_reg & L1_L1)
						{
							if(connection_reg & (L1_PE | L1_N))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_SHORTED;
							}

	//						connection_status |= L1_L3_SHORTED;
						}
						else
						{
							//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
							if(connection_reg & L1_CROSSED_MASK)
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= CROSSED_AND_SHORTED;
							}
	//						connection_status |= L1_L3_CROSSED;
							//ne smemos e postaviti zastavice za crossed, ker se ne vemo vseh pogojev
						}
						connection_reg |= L1_L3;
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								rst_REL(35);
								set_REL(34);
								cord_cw_count++;
							}
							else
								cord_cw_count =100;
						}
						else
						{
							rst_REL(35);
							set_REL(34);
							cord_cw_count++;
						}
					}
					else
					{
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								rst_REL(35);
								set_REL(34);
								cord_cw_count++;
							}
							else
								cord_cw_count =100;
						}
						else
						{
							rst_REL(35);
							set_REL(34);
							cord_cw_count++;
						}
						connection_reg &= (~L1_L3);
					}
					if(((connection_status2 & MULTI_FAULT)&&MULTI_FAULT_CONTINUE)|| !(connection_status2 & MULTI_FAULT))
					{
						if(cord_cw_count == 6)transmitCommFunc(__RPE,__GET,__RESULT,"L1_L2",device.device_dir);
						cord_task_control |= __CORD_RPE_RES_REQUESTED;
					}
				}
				break;
			}
			case 6://L1 L2 POVEZANA?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						if(connection_reg & L1_L1)
						{
							if(connection_reg & (L1_PE | L1_N | L1_L3))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_SHORTED;
							}
	//						connection_status |= L1_L2_SHORTED;
						}
						else
						{
							//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
							if(connection_reg & L1_CROSSED_MASK)
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= CROSSED_AND_SHORTED;
							}
						}
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								rst_REL(10);
								set_REL(11);
								cord_cw_count++;
							}
							else
								cord_cw_count =100;
						}
						else
						{
							rst_REL(10);
							set_REL(11);
							cord_cw_count++;
						}
						connection_reg |= L1_L2;
					}
					else
					{
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								rst_REL(10);
								set_REL(11);
								cord_cw_count++;
							}
							else
								cord_cw_count =100;
						}
						else
						{
							rst_REL(10);
							set_REL(11);
							cord_cw_count++;
						}
						connection_reg &= (~L1_L2);
					}
					//preverjanje za crossed
					if(!(connection_reg & L1_L1))
					{
						if((numberOfSetBits(connection_reg & (L1_CROSSED_MASK | L1_CROSSED_MASK2)))==0)
						{
							connection_status |= L1_OPEN; 
						}
						else if((numberOfSetBits(connection_reg & (L1_CROSSED_MASK | L1_CROSSED_MASK2)))==1)
						{
							switch (connection_reg & (L1_CROSSED_MASK | L1_CROSSED_MASK2))
							{
								case L1_L3: connection_status |= L1_L3_CROSSED; break;
								case L1_L2: connection_status |= L1_L2_CROSSED; break;
								case L1_PE: connection_status |= L1_PE_CROSSED; break;	
								case L1_N: connection_status |= L1_N_CROSSED; break;
								default: break;									
							}
						}
						else if((numberOfSetBits(connection_reg & (L1_CROSSED_MASK | L1_CROSSED_MASK2)))==2)
						{
							if(connection_reg & L1_N)connection_status |= L1_N_CROSSED;
							if(connection_reg & L1_L2)connection_status |= L1_L2_CROSSED;
							if(connection_reg & L1_L3)connection_status |= L1_L3_CROSSED;
							if(connection_reg & L1_PE)connection_status |= L1_PE_CROSSED;
							if((numberOfSetBits(connection_reg & L1_CROSSED_MASK))>1)	//pogledamo ce jih vec postavljenih na eni strani
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= CROSSED_AND_SHORTED;	
							}
							else if((numberOfSetBits(connection_reg & L1_CROSSED_MASK2))>1)
							{
								connection_status2 |= MULTI_FAULT;	
								connection_status2 |= CROSSED_AND_SHORTED;	
							}
							else 
							{
								connection_status2 |= MULTI_FAULT;	
								connection_status2 |= MULTI_CROSSED;	
							}
						}
						else
						{
							if(connection_reg & L1_N)connection_status |= L1_N_CROSSED;
							if(connection_reg & L1_L2)connection_status |= L1_L2_CROSSED;
							if(connection_reg & L1_L3)connection_status |= L1_L3_CROSSED;
							if(connection_reg & L1_PE)connection_status |= L1_PE_CROSSED;
							connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
							connection_status2 |= CROSSED_AND_SHORTED;
						}
					}
					//ce ni crossed in shorted potem pomeni, da je OK
					else
					{
						if(!((numberOfSetBits(connection_reg & (L1_CROSSED_MASK | L1_CROSSED_MASK2)))>0))
							connection_status |= L1_OK;
					}
					if(((connection_status2 & MULTI_FAULT)&&MULTI_FAULT_CONTINUE)|| !(connection_status2 & MULTI_FAULT))
					{
						if(cord_cw_count==7)transmitCommFunc(__RPE,__GET,__RESULT,"L2_L2",device.device_dir);
						cord_task_control |= __CORD_RPE_RES_REQUESTED;
					}
				}
				break;
			}
			case 7://L2 L2 POVEZANA?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_reg |= L2_L2;
							if((!(connection_reg & L1_L1))&&(connection_reg & L1_L2))	
							{
								connection_status2 |= CROSSED_AND_OPEN;
								connection_status2 |= MULTI_FAULT;
							}
	//						if((connection_reg & L1_L1)&&(connection_reg & L1_L2)) //gremo na L2 - PE
	//								{
	//									if(connection_reg & L2_L2)
	//									{
	//										connection_reg |= L2_L1;
	//										connection_status |= L1_L2_SHORTED;
	//									}
	//									else
	//									{
	//										connection_reg &= (~L2_L1);
	//										connection_status2 |= SHORTED_AND_OPEN;
	//										connection_status2 |= MULTI_FAULT;
	//									}
	//								}
							//-----------
							if(connection_status2 & MULTI_FAULT)
							{
								if(MULTI_FAULT_CONTINUE)
								{
									set_next_cord_task_case = true;
								}
								else cord_cw_count = 100;
							}
							else
							{
								set_next_cord_task_case = true;
							}
	//					}

					}
					else
					{
						if((connection_reg & L1_L1)&&(connection_reg & L1_L2))	
						{
							connection_status2 |= CROSSED_AND_OPEN;
							connection_status2 |= MULTI_FAULT;
						}
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else cord_cw_count = 100;
						}
						else
						{
							set_next_cord_task_case = true;
						}
						connection_reg &= (~L2_L2);
					}
					
					if(set_next_cord_task_case == true)
					{
						set_next_cord_task_case = false;
						if((connection_reg & L1_L1)&&(connection_reg & L1_L2)) //gremo na L2 - PE
						{
							if(connection_reg & L2_L2)
							{
								connection_reg |= L2_L1;
								connection_status |= L1_L2_SHORTED;
							}
							else
							{
								connection_reg &= (~L2_L1);
								connection_status2 |= SHORTED_AND_OPEN;
								connection_status2 |= MULTI_FAULT;
							}
							rst_REL(34);
							set_REL(33);
							cord_cw_count++;
						}
	//					if((connection_reg & L1_L1)&&(connection_reg & L1_L2)) //gremo na L2 - PE
	//					{
	//						if(connection_reg & L2_L2)
	//							connection_reg |= L2_L1;
	//						else
	//							connection_reg &= (~L2_L1);
	//						rst_REL(34);
	//						set_REL(33);
	//						cord_cw_count++;
	//					}
						else //Gremo na L2 - L1
						{
							rst_REL(34);
							set_REL(30);
							cord_cw_count=47;
						}
					}
					
					if(((connection_status2 & MULTI_FAULT)&&MULTI_FAULT_CONTINUE)|| !(connection_status2 & MULTI_FAULT))
					{
						if(cord_cw_count == 8)transmitCommFunc(__RPE,__GET,__RESULT,"L2_PE",device.device_dir);
						else if(cord_cw_count == 47)transmitCommFunc(__RPE,__GET,__RESULT,"L2_L1",device.device_dir);
						cord_task_control |= __CORD_RPE_RES_REQUESTED;
					}
				}
				break;
			}
			case 8://L2 PE POVEZANA?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						if(connection_reg & L2_L2)
						{
							if((connection_status & ALL_SHORTED_MASKS)||(connection_reg & (L2_L1)))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_SHORTED;
							}
	//						connection_status |= L2_PE_SHORTED;
						}
						else
						{
							//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
							if(connection_reg & L2_CROSSED_MASK)
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= CROSSED_AND_SHORTED;
							}
							if(connection_status & ALL_CROSSED_MASKS)
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_CROSSED;
							}
						}
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
									set_next_cord_task_case = true;
							}
							else
								cord_cw_count =100;
						}
						else
						{
								set_next_cord_task_case = true;
						}
						connection_reg |= L2_PE;
					}
					else
					{
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else cord_cw_count = 100;
						}
						else
						{
							set_next_cord_task_case = true;
						}
						connection_reg &= (~L2_PE);
					}
					if(set_next_cord_task_case==true)
					{
						set_next_cord_task_case = false;
						rst_REL(30);
						set_REL(36);
						cord_cw_count++;
					}
					if(((connection_status2 & MULTI_FAULT)&&MULTI_FAULT_CONTINUE)|| !(connection_status2 & MULTI_FAULT))
					{
						if(cord_cw_count == 9)transmitCommFunc(__RPE,__GET,__RESULT,"L2_N",device.device_dir);
						cord_task_control |= __CORD_RPE_RES_REQUESTED;
					}
				}
				break;
			}
			case 9://L2 N POVEZANA?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						if(connection_reg & L2_L2)
						{
							if((connection_status & ALL_SHORTED_MASKS)||(connection_reg & (L2_PE | L2_L1)))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_SHORTED;
							}
	//						connection_status |= L2_N_SHORTED;
						}
						else
						{
							//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
							if(connection_reg & L2_CROSSED_MASK)
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= CROSSED_AND_SHORTED;
							}
							if(connection_status & ALL_CROSSED_MASKS)
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_CROSSED;
							}
						}
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
									set_next_cord_task_case = true;
							}
							else
								cord_cw_count =100;
						}
						else
						{
								set_next_cord_task_case = true;
						}
						connection_reg |= L2_N;
					}
					else
					{
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else cord_cw_count = 100;
						}
						else
						{
							set_next_cord_task_case = true;
						}
						connection_reg &= (~L2_N);
					}
					if(set_next_cord_task_case == true)
					{
						set_next_cord_task_case = false;
						rst_REL(36);
						set_REL(35);
						cord_cw_count++;
					}
					if(((connection_status2 & MULTI_FAULT)&&MULTI_FAULT_CONTINUE)|| !(connection_status2 & MULTI_FAULT))
					{
						if(cord_cw_count==10)transmitCommFunc(__RPE,__GET,__RESULT,"L2_L3",device.device_dir);
						cord_task_control |= __CORD_RPE_RES_REQUESTED;
					}
				}
				break;
			}
			case 10://L2 L3 POVEZANA?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{	
						if(connection_reg & L2_L2)
						{
							if((connection_status & ALL_SHORTED_MASKS)||(connection_reg & (L2_N | L2_PE | L2_L1)))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_SHORTED;
							}
	//						connection_status |= L2_L3_SHORTED;
						}
						else
						{
							//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
							if(connection_reg & L2_CROSSED_MASK)
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= CROSSED_AND_SHORTED;
							}
							if(connection_status & ALL_CROSSED_MASKS)
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_CROSSED;
							}
						}
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else
								cord_cw_count =100;
						}
						else
						{
							//Gremo na L3 - L3
							set_next_cord_task_case = true;
						}
						connection_reg |= L2_L3;
					}
					else
					{
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else cord_cw_count = 100;
						}
						else
						{
							set_next_cord_task_case = true;
						}
						connection_reg &= (~L2_L3);
					}
					if(!(connection_reg & L2_L2))
					{
						if((connection_reg & L1_L2)&&(connection_reg & L2_L1))
							connection_status2 |= L1_L2_CRIS_CROSSED;
						else
						{
							if((numberOfSetBits(connection_reg & (L2_CROSSED_MASK | L2_CROSSED_MASK2)))==0)
							{
								//tole bomo nardil sele pri L3, zato ker ne mormo detektirat ce gre za 1p kabel
								#if AUTO_DETECT_P_NUM != true
								if((connection_status & (L1_OPEN)))
								{
									connection_status2 |= MULTI_FAULT;
									connection_status2 |= MULTI_OPENED;
								}
								#endif
								connection_status |= L2_OPEN; 
							}
							else if((numberOfSetBits(connection_reg & (L2_CROSSED_MASK | L2_CROSSED_MASK2)))==1)
							{
								switch (connection_reg & (L2_CROSSED_MASK | L2_CROSSED_MASK2))
								{
									case L2_L1: connection_status2 	|= L2_L1_CROSSED; break;	
									case L2_N: connection_status 		|= L2_N_CROSSED; break;
									case L2_PE: connection_status 	|= L2_PE_CROSSED; break;
									case L1_L2: connection_status 	|= L1_L2_CROSSED; break;	
									case L2_L3: connection_status 	|= L2_L3_CROSSED; break;
									default: break;									
								}
							}
							else if((numberOfSetBits(connection_reg & (L2_CROSSED_MASK | L2_CROSSED_MASK2)))==2)
							{
								if(connection_reg & L1_L2)connection_status |= L1_L2_CROSSED;
								if(connection_reg & L2_L1)connection_status2 |= L2_L1_CROSSED;
								if(connection_reg & L2_N)connection_status |= L2_N_CROSSED;
								if(connection_reg & L2_PE)connection_status |= L2_PE_CROSSED;
								if(connection_reg & L2_L3)connection_status |= L2_L3_CROSSED;
								if((numberOfSetBits(connection_reg & L2_CROSSED_MASK))>1)	//pogledamo ce jih vec postavljenih na eni strani
								{
									connection_status2 |= MULTI_FAULT;
									connection_status2 |= CROSSED_AND_SHORTED;	
								}
								else if((numberOfSetBits(connection_reg & L2_CROSSED_MASK2))>1)
								{
									connection_status2 |= MULTI_FAULT;	
									connection_status2 |= CROSSED_AND_SHORTED;	
								}
								else 
								{
									connection_status2 |= MULTI_FAULT;	
									connection_status2 |= MULTI_CROSSED;	
								}
							}
							else
							{
								if(connection_reg & L1_L2)connection_status |= L1_L2_CROSSED;
								if(connection_reg & L2_L1)connection_status2 |= L2_L1_CROSSED;
								if(connection_reg & L2_N)connection_status |= L2_N_CROSSED;
								if(connection_reg & L2_PE)connection_status |= L2_PE_CROSSED;
								if(connection_reg & L2_L3)connection_status |= L2_L3_CROSSED;
								connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
								connection_status2 |= CROSSED_AND_SHORTED;
							}
						}
					}
					//ce ni crossed in shorted potem pomeni, da je OK
					else
					{
						if(numberOfSetBits((connection_status & (L1_L1|L1_L2|L2_L1)))>=2)
						{
							switch(numberOfSetBits((connection_status & (L1_L1|L1_L2|L2_L1))))
							{
								case 2:connection_status2 |= CROSSED_AND_OPEN; connection_status2 |= MULTI_FAULT;break;
								case 3:connection_status |= L1_L2_SHORTED; break;
								default: break;
							}
						}
						if(numberOfSetBits(connection_status & ALL_SHORTED_MASKS)>1)
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= MULTI_SHORTED;
						}
						if(numberOfSetBits(connection_status & L2_SHORTED_MASK)>=1)
							;
						else if(!((numberOfSetBits(connection_reg & (L2_CROSSED_MASK | L2_CROSSED_MASK2)))>0))
							connection_status |= L2_OK;
					}
					if(set_next_cord_task_case == true)
					{
						set_next_cord_task_case = false;
						rst_REL(11);
						set_REL(12);
						cord_cw_count++;
					}
					if(((connection_status2 & MULTI_FAULT)&&MULTI_FAULT_CONTINUE)|| !(connection_status2 & MULTI_FAULT))
					{
						if(cord_cw_count == 11)transmitCommFunc(__RPE,__GET,__RESULT,"L3_L3",device.device_dir);
						cord_task_control |= __CORD_RPE_RES_REQUESTED;
					}
				}
				break;
			}
			case 11://L3 L3 POVEZANA?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_reg |= L3_L3;
	//					//pogoji ki se jih da razbrati zaradi prejsnih merjenj
	//					if(((connection_status & L1_L3_SHORTED)&&(connection_status & L1_PE_SHORTED))||((connection_status & L2_L3_SHORTED)&&(connection_status & L2_PE_SHORTED)))
	//					{connection_status |= L3_PE_SHORTED; connection_reg |= L3_PE;}
	//					if(((connection_status & L1_L3_SHORTED)&&(connection_status & L2_N_SHORTED))||((connection_status & L2_L3_SHORTED)&&(connection_status & L1_N_SHORTED)))
	//					{connection_status |= L3_N_SHORTED;connection_reg |= L3_N;}
	//					
						//------------pogoji za crossed (crossed in shorted ne morta bit skup-------
	//					if((connection_status & L1_L3_CROSSED)||(connection_status & L2_L3_CROSSED))
	//					{
	//						connection_status2 |= CROSSED_AND_OPEN;
	//						connection_status2 |= MULTI_FAULT;
	//					}
						if(((!(connection_reg & L2_L2))&&(connection_reg & L2_L3))||((!(connection_reg & L1_L1))&&(connection_reg & L1_L3)))
						{
							connection_status2 |= CROSSED_AND_OPEN;
							connection_status2 |= MULTI_FAULT;
						}
						
	//					if((connection_status & L3_PE_SHORTED)&&(connection_status & L3_N_SHORTED))
	//					{
							if(connection_status2 & MULTI_FAULT)
							{
								if(MULTI_FAULT_CONTINUE)
								{
									set_next_cord_task_case = true;
								}
								else cord_cw_count = 100;
							}
							else 
							{
								set_next_cord_task_case = true;
							}
	//					}
	//					else if((connection_status & L3_PE_SHORTED))
	//					{
	//						if(connection_status2 & MULTI_FAULT)
	//						{
	//							if(MULTI_FAULT_CONTINUE)
	//							{
	//								//Gremo na L3 - N
	//								rst_REL(35);
	//								set_REL(36);
	//								cord_cw_count=13;
	//							}
	//							else cord_cw_count = 100;
	//						}
	//						else
	//						{
	//							//Gremo na L3 - N
	//							rst_REL(35);
	//							set_REL(36);
	//							cord_cw_count=13;
	//						}
							
						}
						else //gremo za ena naprej torej na L3 - PE
						{
							if(connection_status2 & MULTI_FAULT)
							{
								if(MULTI_FAULT_CONTINUE)
								{
									set_next_cord_task_case = true;
								}
								else cord_cw_count = 100;
							}
							else
							{
								set_next_cord_task_case = true;
							}
							connection_reg &= (~L3_L3);
						}
	//				}
	//				else
	//				{
	////					if(connection_status & L1_L3_CROSSED)
	////					{
	////						//prepricamo se, da je prevezan z L1
	////						if(connection_status2 & MULTI_FAULT)
	////						{
	////							if(MULTI_FAULT_CONTINUE)
	////							{
	////								rst_REL(35);
	////								set_REL(33);
	////								cord_cw_count=18;
	////							}
	////							else cord_cw_count = 100;
	////						}
	////						else
	////						{
	////							rst_REL(35);
	////							set_REL(33);
	////							cord_cw_count=18;
	////						}
	////					}
	////					else if(connection_status & L2_L3_CROSSED)
	////					{
	////						//prepricamo se, da je prevezan z L1
	////						if(connection_status2 & MULTI_FAULT)
	////						{
	////							if(MULTI_FAULT_CONTINUE)
	////							{
	////								rst_REL(35);
	////								set_REL(34);
	////								cord_cw_count=19;
	////							}
	////							else cord_cw_count = 100;
	////						}
	////						else
	////						{
	////							rst_REL(35);
	////							set_REL(34);
	////							cord_cw_count=19;
	////						}
	////					}
	////					else
	////					{
	////						if(connection_status2 & MULTI_FAULT)
	////						{
	////							if(MULTI_FAULT_CONTINUE)
	////							{
	////								rst_REL(34);
	////								set_REL(30);
	////								cord_cw_count++;
	////							}
	////							else cord_cw_count = 100;
	////						}
	////						else
	////						{
	//							rst_REL(34);
	//							set_REL(30);
	//							cord_cw_count++;
	////						}
	//					}
	//					connection_reg &= (~L3_L3);
	//				}
					if(set_next_cord_task_case == true)
					{
						set_next_cord_task_case = false;
						if(((connection_reg & L1_L1)&&(connection_reg & L1_L3))&&((connection_reg & L2_L2)&&(connection_reg & L2_L3))) //gremo na L2 - PE
						{
							if(connection_reg & L3_L3)
							{
								connection_reg |= L3_L1;
								connection_reg |= L3_L2;
								connection_status |= L1_L3_SHORTED;
								connection_status |= L2_L3_SHORTED;
							}
							else
							{
								connection_reg &= (~L3_L1);
								connection_reg &= (~L3_L2);
								connection_status2 |= SHORTED_AND_OPEN;
								connection_status2 |= MULTI_FAULT;
							}
							rst_REL(35);
							set_REL(30);
							cord_cw_count++;
						}
						else if((connection_reg & L1_L1)&&(connection_reg & L1_L3)) //gremo na L2 - PE
						{
							if(connection_reg & L3_L3)
							{
								connection_reg |= L3_L1;
								connection_status |= L1_L3_SHORTED;
							}
							else
							{
								connection_reg &= (~L3_L1);
								connection_status2 |= SHORTED_AND_OPEN;
								connection_status2 |= MULTI_FAULT;
							}
							rst_REL(35);
							set_REL(33);
							cord_cw_count=48;
						}
						else if((connection_reg & L2_L2)&&(connection_reg & L2_L3)) //gremo na L2 - PE
						{
							if(connection_reg & L3_L3)
							{
								connection_reg |= L3_L2;
								connection_status |= L2_L3_SHORTED;
							}
							else
							{
								connection_reg &= (~L3_L2);
								connection_status2 |= SHORTED_AND_OPEN;
								connection_status2 |= MULTI_FAULT;
							}
							rst_REL(35);
							set_REL(34);
							cord_cw_count=49;
						}
						else //Gremo na L2 - L1
						{
							rst_REL(35);
							set_REL(33);
							cord_cw_count=50;
						}
	//								rst_REL(12);
	//								set_REL(13);
	//								rst_REL(35);
	//								set_REL(36);
	//								cord_cw_count=14;
					}
					if(((connection_status2 & MULTI_FAULT)&&MULTI_FAULT_CONTINUE)|| !(connection_status2 & MULTI_FAULT))
					{
						if(cord_cw_count == 12)transmitCommFunc(__RPE,__GET,__RESULT,"L3_PE",device.device_dir);
						else if(cord_cw_count == 48)transmitCommFunc(__RPE,__GET,__RESULT,"L3_L2",device.device_dir);
						else if(cord_cw_count == 49)transmitCommFunc(__RPE,__GET,__RESULT,"L3_L1",device.device_dir);
						else if(cord_cw_count == 50)transmitCommFunc(__RPE,__GET,__RESULT,"L3_L1",device.device_dir);
						cord_task_control |= __CORD_RPE_RES_REQUESTED;
					}
				}
				break;
			}
			case 12://L3 PE POVEZANA?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						if(connection_reg & L3_L3)
						{
							if((connection_status & ALL_SHORTED_MASKS)||(connection_reg & (L3_L1 | L3_L2)))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_SHORTED;
							}
						}
						else
						{
							//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
							if(connection_reg & L3_CROSSED_MASK)
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= CROSSED_AND_SHORTED;
							}
							if(connection_status & ALL_CROSSED_MASKS)
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_CROSSED;
							}
						}
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
	//							if((connection_status & L3_N_SHORTED))
	//							{
	//								//Gremo na N - N
	//								rst_REL(12);
	//								set_REL(13);
	//								rst_REL(30);
	//								set_REL(36);
	//								cord_cw_count=14;
	//							}
	//							else
	//							{
									set_next_cord_task_case = true;
	//							}
							}
							else
								cord_cw_count =100;
						}
						else
						{
	//						if((connection_status & L3_N_SHORTED))
	//						{
	//							//Gremo na N - N
	//							rst_REL(12);
	//							set_REL(13);
	//							rst_REL(30);
	//							set_REL(36);
	//							cord_cw_count=14;
	//						}
	//						else
	//						{
								set_next_cord_task_case = true;
	//						}
						}
						connection_reg |= L3_PE;
					}
					else
					{
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else cord_cw_count = 100;
						}
						else
						{
							set_next_cord_task_case = true;
						}
						connection_reg &= (~L3_PE);
					}
					if(set_next_cord_task_case == true)
					{
						set_next_cord_task_case = false;
						rst_REL(30);
						set_REL(36);
						cord_cw_count++;
					}
					if(((connection_status2 & MULTI_FAULT)&&MULTI_FAULT_CONTINUE)|| !(connection_status2 & MULTI_FAULT))
					{
						if(cord_cw_count ==13)transmitCommFunc(__RPE,__GET,__RESULT,"L3_N",device.device_dir);
						cord_task_control |= __CORD_RPE_RES_REQUESTED;
					}
				}
				break;
			}
			case 13://L3 N POVEZANA?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						if(connection_reg & L3_L3)
						{
							if((connection_status & ALL_SHORTED_MASKS)||(connection_reg & (L3_L1 | L3_L2 | L3_N)))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_SHORTED;
							}

						}
						else
						{
							//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
							if(connection_reg & L3_CROSSED_MASK)
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= CROSSED_AND_SHORTED;
							}
							if(connection_status & ALL_CROSSED_MASKS)
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_CROSSED;
							}
						}
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else
								cord_cw_count =100;
						}
						else
						{
							//Gremo na N - N
							set_next_cord_task_case = true;
						}
						connection_reg |= L3_N;
					}
					else
					{
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else cord_cw_count = 100;
						}
						else
						{
							set_next_cord_task_case = true;
						}
						connection_reg &= (~L3_N);
					}
					if(!(connection_reg & L3_L3))
					{
						if((connection_reg & L2_L3)&&(connection_reg & L3_L2))
							connection_status2 |= L2_L3_CRIS_CROSSED;
						else if((connection_reg & L1_L3)&&(connection_reg & L3_L1))
							connection_status2 |= L1_L3_CRIS_CROSSED;
						else
						{
							if((numberOfSetBits(connection_reg & (L3_CROSSED_MASK | L3_CROSSED_MASK2)))==0)
							{
								#if AUTO_DETECT_P_NUM == true
									//ce velja naslednje predpostavljamo da gre za 1P kabel ce ne potem je lahko napaka na 3p kablu
								if((connection_status & (L1_OPEN | L2_OPEN))||((!(connection_status & (L1_OPEN))) && connection_status & (L2_OPEN)))
								{
								}
								else if(connection_status & (L1_OPEN) && (!(connection_status & (L2_OPEN))))
								{
									connection_status2 |= MULTI_FAULT;
									connection_status2 |= MULTI_OPENED;
								}
								#else
								if(connection_status & (L1_OPEN | L2_OPEN))
								{
									connection_status2 |= MULTI_FAULT;
									connection_status2 |= MULTI_OPENED;
								}
								#endif
								connection_status |= L3_OPEN; 
							}
							else if((numberOfSetBits(connection_reg & (L3_CROSSED_MASK | L3_CROSSED_MASK2)))==1)
							{
								switch (connection_reg & (L3_CROSSED_MASK | L3_CROSSED_MASK2))
								{
									case L3_L1: connection_status2 	|= L3_L1_CROSSED; break;	
									case L3_L2: connection_status2 	|= L3_L2_CROSSED; break;
									case L3_N: connection_status 		|= L3_N_CROSSED; break;
									case L3_PE: connection_status 	|= L3_PE_CROSSED; break;
									case L1_L3: connection_status 	|= L1_L3_CROSSED; break;	
									case L2_L3: connection_status 	|= L2_L3_CROSSED; break;
									default: break;									
								}
							}
							else if((numberOfSetBits(connection_reg & (L3_CROSSED_MASK | L3_CROSSED_MASK2)))==2)
							{
								if(connection_reg & L1_L3)connection_status |= L1_L3_CROSSED;
								if(connection_reg & L2_L3)connection_status |= L2_L3_CROSSED;
								if(connection_reg & L3_L1)connection_status2 |= L3_L1_CROSSED;
								if(connection_reg & L3_L2)connection_status2 |= L3_L2_CROSSED;
								if(connection_reg & L3_N)connection_status |= L3_N_CROSSED;
								if(connection_reg & L3_PE)connection_status |= L3_PE_CROSSED;
								if((numberOfSetBits(connection_reg & L3_CROSSED_MASK))>1)	//pogledamo ce jih vec postavljenih na eni strani
								{
									connection_status2 |= MULTI_FAULT;
									connection_status2 |= CROSSED_AND_SHORTED;	
								}
								else if((numberOfSetBits(connection_reg & L3_CROSSED_MASK2))>1)
								{
									connection_status2 |= MULTI_FAULT;	
									connection_status2 |= CROSSED_AND_SHORTED;	
								}
								else 
								{
									connection_status2 |= MULTI_FAULT;	
									connection_status2 |= MULTI_CROSSED;	
								}
							}
							else
							{
								if(connection_reg & L1_L3)connection_status |= L1_L3_CROSSED;
								if(connection_reg & L2_L3)connection_status |= L2_L3_CROSSED;
								if(connection_reg & L3_L1)connection_status2 |= L3_L1_CROSSED;
								if(connection_reg & L3_L2)connection_status2 |= L3_L2_CROSSED;
								if(connection_reg & L3_N)connection_status |= L3_N_CROSSED;
								if(connection_reg & L3_PE)connection_status |= L3_PE_CROSSED;
								connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
								connection_status2 |= CROSSED_AND_SHORTED;
							}
						}
					}
					//ce ni crossed in shorted potem pomeni, da je OK
					else
					{
						if(numberOfSetBits((connection_status & (L1_L1|L1_L3|L3_L1)))>=2)
						{
							switch(numberOfSetBits((connection_status & (L1_L1|L1_L3|L3_L1))))
							{
								case 2:connection_status2 |= CROSSED_AND_OPEN; connection_status2 |= MULTI_FAULT;break;
								case 3:connection_status |= L1_L3_SHORTED; break;
								default: break;
							}
						}
						if(numberOfSetBits((connection_status & (L2_L2|L2_L3|L3_L2)))>=2)
						{
							switch(numberOfSetBits((connection_status & (L2_L2|L2_L3|L3_L2))))
							{
								case 2:connection_status2 |= CROSSED_AND_OPEN; connection_status2 |= MULTI_FAULT;break;
								case 3:connection_status |= L2_L3_SHORTED; break;
								default: break;
							}
						}
						if(numberOfSetBits(connection_status & ALL_SHORTED_MASKS)>1)
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= MULTI_SHORTED;
						}
						if(numberOfSetBits(connection_status & L3_SHORTED_MASK)>=1)
							;
						else if(!((numberOfSetBits(connection_reg & (L3_CROSSED_MASK | L3_CROSSED_MASK2)))>0))
							connection_status |= L3_OK;
					}
					if(set_next_cord_task_case == true)
					{
						set_next_cord_task_case = false;
						rst_REL(12);
						set_REL(13);
						cord_cw_count++;
					}
	//				if(!(connection_reg & L3_L3))
	//				{
	//					if(connection_status2 & L1_L3_CRIS_CROSSED)
	//					{
	//						if(connection_reg & (L2_L3 | L3_N | L3_PE))
	//						{
	//							connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
	//							connection_status2 |= CROSSED_AND_SHORTED;
	//						}
	//					}
	//					else if(connection_status2 & L2_L3_CRIS_CROSSED)
	//					{
	//						if(connection_reg & (L1_L3 | L3_N | L3_PE))
	//						{
	//							connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
	//							connection_status2 |= CROSSED_AND_SHORTED;
	//						}
	//					}
	//					else
	//					{
	//						switch (connection_reg & L3_CROSSED_MASK)
	//						{
	//							case L3_N_CROSSED_MASK: connection_status |= L3_N_CROSSED; break;
	//							case L3_PE_CROSSED_MASK: connection_status |= L3_PE_CROSSED; break;
	//							case 0: connection_status |= L3_OPEN; break;	//vse ok
	//							default: 
	//								connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
	//								connection_status2 |= CROSSED_AND_SHORTED;
	//						}
	//					}
	//				}
	//				//ce ni crossed in shorted potem pomeni, da je OK
	//				else if((connection_reg & L3_L3)&(!(connection_status & L3_SHORTED_MASK)))
	//				{
	//					connection_status |= L3_OK;
	//				}
					if(((connection_status2 & MULTI_FAULT)&&MULTI_FAULT_CONTINUE)|| !(connection_status2 & MULTI_FAULT))
					{
						if(cord_cw_count == 14) transmitCommFunc(__RPE,__GET,__RESULT,"N_N",device.device_dir);
						cord_task_control |= __CORD_RPE_RES_REQUESTED;
					}
				}
				break;
			}
			case 14:	//N N POVEZANA?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					rst_REL(36);
					set_REL(30);
					if(cord_check_RPE_resistance())
					{
						connection_reg |= N_N;
	//					if(((connection_status & L1_N_SHORTED)&&(connection_status & L1_PE_SHORTED))||((connection_status & L2_N_SHORTED)&&(connection_status & L2_PE_SHORTED))||((connection_status & L3_N_SHORTED)&&(connection_status & L3_PE_SHORTED)))
	//					{connection_status |= N_PE_SHORTED; connection_reg |= N_PE;}
	//					
	//					//------------pogoji za crossed (crossed in shorted ne morta bit skup-------
	//					if((connection_status & L1_N_CROSSED)||(connection_status & L2_N_CROSSED)||(connection_status & L3_N_CROSSED))
	//					{
	//						connection_status2 |= CROSSED_AND_OPEN;
	//						connection_status2 |= MULTI_FAULT;
	//					}
						if(((!(connection_reg & L3_L3))&&(connection_reg & L3_N))||((!(connection_reg & L1_L1))&&(connection_reg & L1_N))||((!(connection_reg & L2_L2))&&(connection_reg & L2_N)))
						{
							connection_status2 |= CROSSED_AND_OPEN;
							connection_status2 |= MULTI_FAULT;
						}
	//					if(connection_status & N_PE_SHORTED)
	//					{
						if(connection_status2 & MULTI_FAULT)
						{
							if(connection_status2 & MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else cord_cw_count = 100;
						}
						else 
						{
								set_next_cord_task_case = true;
						}
					}
					else
					{
	//					if(connection_status & L1_N_CROSSED)
	//					{
	//						//prepricamo se, da je prevezan z L1
	//						if(connection_status2 & MULTI_FAULT)
	//						{
	//							if(MULTI_FAULT_CONTINUE)
	//							{
	//								rst_REL(36);
	//								set_REL(33);
	//								cord_cw_count=22;
	//							}
	//							else cord_cw_count = 100;
	//						}
	//						else
	//						{
	//							rst_REL(36);
	//							set_REL(33);
	//							cord_cw_count=22;
	//						}
	//					}
	//					else if(connection_status & L2_N_CROSSED)
	//					{
	//						//prepricamo se, da je prevezan z L1
	//						if(connection_status2 & MULTI_FAULT)
	//						{
	//							if(MULTI_FAULT_CONTINUE)
	//							{
	//								rst_REL(36);
	//								set_REL(34);
	//								cord_cw_count=23;
	//							}
	//							else cord_cw_count = 100;
	//						}
	//						else
	//						{
	//							rst_REL(36);
	//							set_REL(34);
	//							cord_cw_count=23;
	//						}
	//					}
	//					else if(connection_status & L3_N_CROSSED)
	//					{
	//						//prepricamo se, da je prevezan z L1
	//						if(connection_status2 & MULTI_FAULT)
	//						{
	//							if(MULTI_FAULT_CONTINUE)
	//							{
	//								rst_REL(36);
	//								set_REL(35);
	//								cord_cw_count=24;
	//							}
	//							else cord_cw_count = 100;
	//						}
	//						else
	//						{
	//							rst_REL(36);
	//							set_REL(35);
	//							cord_cw_count=24;
	//						}
	//					}
	//					else
	//					{
							if(connection_status2 & MULTI_FAULT)
							{
								if(MULTI_FAULT_CONTINUE)
									set_next_cord_task_case = true;
								else cord_cw_count = 100;
							}
							else
								set_next_cord_task_case = true;
	//					}
						connection_reg &= (~N_N);
					}
					if(set_next_cord_task_case)
					{
						set_next_cord_task_case = false;
						if(!((connection_reg & L1_L1)&&(connection_reg & L1_N))) //preverimo ce je slucajno ze shorted
						{
							rst_REL(36);
							set_REL(33);
							cord_cw_count=52;
						}
						else if((!((connection_reg & L2_L2)&&(connection_reg & L2_N)))&&(!(manual_set_phase_num && CORD_PHASE_NUM_SETTING==_1_PHASE))) 
						{
							if(connection_reg & N_N)
							{
								connection_reg |= N_L1;
								connection_status |= L1_N_SHORTED;
							}
							else
							{
								connection_reg &= (~N_L1);
								connection_status2 |= SHORTED_AND_OPEN;
								connection_status2 |= MULTI_FAULT;
							}
							rst_REL(36);
							set_REL(34);
							cord_cw_count=53;
						}
						else if((!((connection_reg & L3_L3)&&(connection_reg & L3_N)))&&(!(manual_set_phase_num && CORD_PHASE_NUM_SETTING==_1_PHASE))) //gremo na L2 - PE
						{
							if(connection_reg & N_N)
							{
								connection_reg |= N_L1;
								connection_reg |= N_L2;
								connection_status |= L1_N_SHORTED;
								connection_status |= L2_N_SHORTED;
							}
							else
							{
								connection_reg &= (~N_L1);
								connection_reg &= (~N_L2);
								connection_status2 |= SHORTED_AND_OPEN;
								connection_status2 |= MULTI_FAULT;
							}
							rst_REL(36);
							set_REL(35);
							cord_cw_count=54;
						}
						else 
						{
							if(manual_set_phase_num && CORD_PHASE_NUM_SETTING==_1_PHASE)
							{
								if(connection_reg & N_N)
								{
									connection_reg |= N_L1;
									connection_status |= L1_N_SHORTED;
								}
								else
								{
									connection_reg &= (~N_L1);
									connection_status2 |= SHORTED_AND_OPEN;
									connection_status2 |= MULTI_FAULT;
								}
							}
							else
							{
								if(connection_reg & N_N)
								{
									connection_reg |= N_L1;
									connection_reg |= N_L2;
									connection_reg |= N_L3;
									connection_status |= L1_N_SHORTED;
									connection_status |= L2_N_SHORTED;
									connection_status |= L3_N_SHORTED;
								}
								else
								{
									connection_reg &= (~N_L1);
									connection_reg &= (~N_L2);
									connection_reg &= (~N_L3);
									connection_status2 |= SHORTED_AND_OPEN;
									connection_status2 |= MULTI_FAULT;
								}
							}
							rst_REL(36);
							set_REL(30);
							cord_cw_count++;
						}
					}
					if(((connection_status2 & MULTI_FAULT)&&MULTI_FAULT_CONTINUE)|| !(connection_status2 & MULTI_FAULT))
					{
						if(cord_cw_count ==15)transmitCommFunc(__RPE,__GET,__RESULT,"N_PE",device.device_dir);
						else if(cord_cw_count ==52)transmitCommFunc(__RPE,__GET,__RESULT,"N_L1",device.device_dir);
						else if(cord_cw_count ==53)transmitCommFunc(__RPE,__GET,__RESULT,"N_L2",device.device_dir);
						else if(cord_cw_count ==54)transmitCommFunc(__RPE,__GET,__RESULT,"N_L3",device.device_dir);
						cord_task_control |= __CORD_RPE_RES_REQUESTED;
					}
				}
				break;
			}
			case 15://N PE POVEZANA?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						if(connection_reg & N_N)
						{
							if(connection_status & N_SHORTED_MASK)//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_SHORTED;
							}
							connection_status |= N_PE_SHORTED;
						}
						else
						{
							//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
							if(connection_reg & N_CROSSED_MASK)
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= CROSSED_AND_SHORTED;
							}
						}
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else
								cord_cw_count =100;
						}
						else
						{
							//Gremo na N - N
							set_next_cord_task_case = true;
						}
						connection_reg |= N_PE;
					}
					else
					{
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else cord_cw_count = 100;
						}
						else
						{
							set_next_cord_task_case = true;
						}
						connection_reg &= (~N_PE);
					}
	//				if(!(connection_reg & N_N))
	//				{
	//					if(connection_status2 & L1_N_CRIS_CROSSED)
	//					{
	//						if(connection_reg & (L2_N | L3_N | N_PE))
	//						{
	//							connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
	//							connection_status2 |= CROSSED_AND_SHORTED;
	//						}
	//					}
	//					else if(connection_status2 & L2_N_CRIS_CROSSED)
	//					{
	//						if(connection_reg & (L1_N | L3_N | N_PE))
	//						{
	//							connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
	//							connection_status2 |= CROSSED_AND_SHORTED;
	//						}
	//					}
	//					else if(connection_status2 & L3_N_CRIS_CROSSED)
	//					{
	//						if(connection_reg & (L1_N | L2_N | N_PE))
	//						{
	//							connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
	//							connection_status2 |= CROSSED_AND_SHORTED;
	//						}
	//					}
	//					else
	//					{
	//						switch (connection_reg & N_CROSSED_MASK)
	//						{
	//							case N_PE_CROSSED_MASK: connection_status |= N_PE_CROSSED; break;
	//							case 0: connection_status |= N_OPEN; break;	//vse ok
	//							default: 
	//								connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
	//								connection_status2 |= CROSSED_AND_SHORTED;
	//						}
	//					}
	//				}
	//				//ce ni crossed in shorted potem pomeni, da je OK
	//				else if((connection_reg & N_N)&(!(connection_status & N_SHORTED_MASK)))
	//				{
	//					connection_status |= N_OK;
	//				}
					if(!(connection_reg & N_N))
					{
						if((connection_reg & L2_N)&&(connection_reg & N_L2))
							connection_status2 |= L2_N_CRIS_CROSSED;
						else if((connection_reg & L1_N)&&(connection_reg & N_L1))
							connection_status2 |= L1_N_CRIS_CROSSED;
						else if((connection_reg & L3_N)&&(connection_reg & N_L3))
							connection_status2 |= L3_N_CRIS_CROSSED;
						else
						{
							if((numberOfSetBits(connection_reg & (N_CROSSED_MASK | N_CROSSED_MASK2)))==0)
							{
								if(connection_status & (L1_OPEN | L2_OPEN | L3_OPEN))
								{
									connection_status2 |= MULTI_FAULT;
									connection_status2 |= MULTI_OPENED;
								}
								connection_status |= N_OPEN; 
							}
							else if((numberOfSetBits(connection_reg & (N_CROSSED_MASK | N_CROSSED_MASK2)))==1)
							{
								switch (connection_reg & (N_CROSSED_MASK | N_CROSSED_MASK2))
								{
									case N_L1: connection_status2 |= N_L1_CROSSED; break;	
									case N_L2: connection_status2 |= N_L2_CROSSED; break;
									case N_L3: connection_status2 |= N_L3_CROSSED; break;
									case L1_N: connection_status |= L1_N_CROSSED; break;	
									case L2_N: connection_status |= L2_N_CROSSED; break;
									case L3_N: connection_status |= L3_N_CROSSED; break;
									case N_PE_CROSSED_MASK: connection_status |= N_PE_CROSSED; break;
									default: break;									
								}
							}
							else if((numberOfSetBits(connection_reg & (N_CROSSED_MASK | N_CROSSED_MASK2)))==2)
							{
								if(connection_reg & L1_N)connection_status |= L1_N_CROSSED;
								if(connection_reg & L2_N)connection_status |= L2_N_CROSSED;
								if(connection_reg & L3_N)connection_status |= L3_N_CROSSED;
								if(connection_reg & N_L1)connection_status2 |= N_L1_CROSSED;
								if(connection_reg & N_L2)connection_status2 |= N_L2_CROSSED;
								if(connection_reg & N_L3)connection_status2 |= N_L3_CROSSED;
								if(connection_reg & N_PE)connection_status |= N_PE_CROSSED;
								if((numberOfSetBits(connection_reg & N_CROSSED_MASK))>1)	//pogledamo ce jih vec postavljenih na eni strani
								{
									connection_status2 |= MULTI_FAULT;
									connection_status2 |= CROSSED_AND_SHORTED;	
								}
								else if((numberOfSetBits(connection_reg & N_CROSSED_MASK2))>1)
								{
									connection_status2 |= MULTI_FAULT;	
									connection_status2 |= CROSSED_AND_SHORTED;	
								}
								else 
								{
									connection_status2 |= MULTI_FAULT;	
									connection_status2 |= MULTI_CROSSED;	
								}
							}
							else
							{
								if(connection_reg & L1_N)connection_status |= L1_N_CROSSED;
								if(connection_reg & L2_N)connection_status |= L2_N_CROSSED;
								if(connection_reg & L3_N)connection_status |= L3_N_CROSSED;
								if(connection_reg & N_L1)connection_status2 |= N_L1_CROSSED;
								if(connection_reg & N_L2)connection_status2 |= N_L2_CROSSED;
								if(connection_reg & N_L3)connection_status2 |= N_L3_CROSSED;
								if(connection_reg & N_PE)connection_status |= N_PE_CROSSED;
								connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
								connection_status2 |= CROSSED_AND_SHORTED;
							}
						}
					}
					//ce ni crossed in shorted potem pomeni, da je OK
					else
					{
						if(numberOfSetBits((connection_status & (L1_L1|L1_N|N_L1)))>=2)
						{
							switch(numberOfSetBits((connection_status & (L1_L1|L1_N|N_L1))))
							{
								case 2:connection_status2 |= CROSSED_AND_OPEN; connection_status2 |= MULTI_FAULT;break;
								case 3:connection_status |= L1_N_SHORTED; break;
								default: break;
							}
						}
						if(numberOfSetBits((connection_status & (L2_L2|L2_N|N_L2)))>=2)
						{
							switch(numberOfSetBits((connection_status & (L2_L2|L2_N|N_L2))))
							{
								case 2:connection_status2 |= CROSSED_AND_OPEN; connection_status2 |= MULTI_FAULT;break;
								case 3:connection_status |= L2_N_SHORTED; break;
								default: break;
							}
						}
						if(numberOfSetBits((connection_status & (L3_L3|L3_N|N_L3)))>=2)
						{
							switch(numberOfSetBits((connection_status & (L3_L3|L3_N|N_L3))))
							{
								case 2:connection_status2 |= CROSSED_AND_OPEN; connection_status2 |= MULTI_FAULT;break;
								case 3:connection_status |= N_PE_SHORTED; break;
								default: break;
							}
						}
						if(numberOfSetBits(connection_status & ALL_SHORTED_MASKS)>1)
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= MULTI_SHORTED;
						}
						if(numberOfSetBits(connection_status & N_SHORTED_MASK)>=1)
							;
						else if(!((numberOfSetBits(connection_reg & (N_CROSSED_MASK | N_CROSSED_MASK2)))>0))
							connection_status |= N_OK;
					}
					if(set_next_cord_task_case == true)
					{
						set_next_cord_task_case = false;
						rst_REL(13);
						set_REL(8);
						cord_cw_count++;
					}
					if(((connection_status2 & MULTI_FAULT)&&MULTI_FAULT_CONTINUE)|| !(connection_status2 & MULTI_FAULT))
					{
						if(cord_cw_count == 16) transmitCommFunc(__RPE,__GET,__RESULT,"PE_PE",device.device_dir);
						cord_task_control |= __CORD_RPE_RES_REQUESTED;
					}
				}
				break;
			}
			case 16://PE PE POVEZANA?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_reg |= PE_PE;
						//------------pogoji za crossed (crossed in shorted ne morta bit skup)-------
	//					if((connection_status & L1_PE_CROSSED)||(connection_status & L2_PE_CROSSED)||(connection_status & L3_PE_CROSSED)||(connection_status & N_PE_CROSSED))
	//					{
	//						connection_status2 |= CROSSED_AND_OPEN;
	//						connection_status2 |= MULTI_FAULT;
	//					}
						
						if(((!(connection_reg & L3_L3))&&(connection_reg & L3_PE))||((!(connection_reg & L1_L1))&&(connection_reg & L1_PE))||((!(connection_reg & L2_L2))&&(connection_reg & L2_PE))||((!(connection_reg & N_L2))&&(connection_reg & N_PE)))
						{
							connection_status2 |= CROSSED_AND_OPEN;
							connection_status2 |= MULTI_FAULT;
						}
						if(connection_status2 & MULTI_FAULT)
							{
								if(MULTI_FAULT_CONTINUE)
									set_next_cord_task_case = true;
								else cord_cw_count = 100;
							}
							else
								set_next_cord_task_case = true;
					}
					else
					{
	//					if(connection_status & L1_PE_CROSSED)
	//					{
	//						//prepricamo se, da je prevezan z L1
	//						if(connection_status2 & MULTI_FAULT)
	//						{
	//							if(MULTI_FAULT_CONTINUE)
	//							{
	//								rst_REL(30);
	//								set_REL(33);
	//								cord_cw_count=31;
	//							}
	//							else cord_cw_count = 100;
	//						}
	//						else
	//						{
	//							rst_REL(30);
	//							set_REL(33);
	//							cord_cw_count=31;
	//						}
	//					}
	//					else if(connection_status & L2_PE_CROSSED)
	//					{
	//						//prepricamo se, da je prevezan z L1
	//						if(connection_status2 & MULTI_FAULT)
	//						{
	//							if(MULTI_FAULT_CONTINUE)
	//							{
	//								rst_REL(30);
	//								set_REL(34);
	//								cord_cw_count=35;
	//							}
	//							else cord_cw_count = 100;
	//						}
	//						else
	//						{
	//							rst_REL(30);
	//							set_REL(34);
	//							cord_cw_count=35;
	//						}
	//					}
	//					else if(connection_status & L3_PE_CROSSED)
	//					{
	//						//prepricamo se, da je prevezan z L1
	//						if(connection_status2 & MULTI_FAULT)
	//						{
	//							if(MULTI_FAULT_CONTINUE)
	//							{
	//								rst_REL(30);
	//								set_REL(35);
	//								cord_cw_count=39;
	//							}
	//							else cord_cw_count = 100;
	//						}
	//						else
	//						{
	//							rst_REL(30);
	//							set_REL(35);
	//							cord_cw_count=39;
	//						}
	//					}
	//					else if(connection_status & N_PE_CROSSED)
	//					{
	//						//prepricamo se, da je prevezan z L1
	//						if(connection_status2 & MULTI_FAULT)
	//						{
	//							if(MULTI_FAULT_CONTINUE)
	//							{
	//								rst_REL(30);
	//								set_REL(36);
	//								cord_cw_count=43;
	//							}
	//							else cord_cw_count = 100;
	//						}
	//						else
	//						{
	//							rst_REL(30);
	//							set_REL(36);
	//							cord_cw_count=43;
	//						}
	//					}
	//					else
	//					{
	//						if(connection_status2 & MULTI_FAULT)
	//						{
	//							if(MULTI_FAULT_CONTINUE)
	//							{
	//								rst_REL(36);
	//								set_REL(30);
	//								cord_cw_count=98;
	//							}
	//							else cord_cw_count = 100;
	//						}
	//						else
	//						{
	//							rst_REL(36);
	//							set_REL(30);
	//							cord_cw_count=98;
	//						}
	//					}
							if(connection_status2 & MULTI_FAULT)
							{
								if(MULTI_FAULT_CONTINUE)
									set_next_cord_task_case = true;
								else cord_cw_count = 100;
							}
							else
							{
								set_next_cord_task_case = true;
							}
						connection_reg &= (~PE_PE);
					}
					if(set_next_cord_task_case)
					{
						set_next_cord_task_case = false;
						if(!((connection_reg & L1_L1)&&(connection_reg & L1_PE))) //gremo na L2 - PE
						{
							rst_REL(30);
							set_REL(33);
							cord_cw_count=55;
						}
						else if((!((connection_reg & L2_L2)&&(connection_reg & L2_PE)))&&(!(manual_set_phase_num && CORD_PHASE_NUM_SETTING==_1_PHASE))) //gremo na L2 - PE
						{
							if(connection_reg & PE_PE)
							{
								connection_reg |= PE_L1;
								connection_status |= L1_PE_SHORTED;
							}
							else
							{
								connection_reg &= (~PE_L1);
								connection_status2 |= SHORTED_AND_OPEN;
								connection_status2 |= MULTI_FAULT;
							}
							rst_REL(30);
							set_REL(34);
							cord_cw_count=56;
						}
						else if((!((connection_reg & L3_L3)&&(connection_reg & L3_PE)))&&(!(manual_set_phase_num && CORD_PHASE_NUM_SETTING==_1_PHASE))) //gremo na L2 - PE
						{
							if(connection_reg & PE_PE)
							{
								connection_reg |= PE_L1;
								connection_reg |= PE_L2;
								connection_status |= L1_PE_SHORTED;
								connection_status |= L2_PE_SHORTED;
							}
							else
							{
								connection_reg &= (~PE_L1);
								connection_reg &= (~PE_L2);
								connection_status2 |= SHORTED_AND_OPEN;
								connection_status2 |= MULTI_FAULT;
							}
							rst_REL(30);
							set_REL(35);
							cord_cw_count=57;
						}
						else if(!((connection_reg & N_N)&&(connection_reg & N_PE)))
						{
							if(connection_reg & PE_PE)
							{
								connection_reg |= PE_L1;
								connection_reg |= PE_L2;
								connection_reg |= PE_L3;
								connection_status |= L1_PE_SHORTED;
								connection_status |= L2_PE_SHORTED;
								connection_status |= L3_PE_SHORTED;
							}
							else
							{
								connection_reg &= (~PE_L1);
								connection_reg &= (~PE_L2);
								connection_reg &= (~PE_L3);
								connection_status2 |= SHORTED_AND_OPEN;
								connection_status2 |= MULTI_FAULT;
							}
							rst_REL(30);
							set_REL(36);
							cord_cw_count=58;
						}
						else
						{
							if(manual_set_phase_num && CORD_PHASE_NUM_SETTING == _1_PHASE)
							{
								if(connection_reg & PE_PE)
								{
									connection_reg |= PE_L1;
									connection_reg |= PE_N;
									connection_status |= L1_PE_SHORTED;
									connection_status |= N_PE_SHORTED;
								}
								else
								{
									connection_reg &= (~PE_L1);
									connection_reg &= (~PE_N);
									connection_status2 |= SHORTED_AND_OPEN;
									connection_status2 |= MULTI_FAULT;
								}
							}
							else
							{
								if(connection_reg & PE_PE)
								{
									connection_reg |= PE_L1;
									connection_reg |= PE_L2;
									connection_reg |= PE_L3;
									connection_reg |= PE_N;
									connection_status |= L1_PE_SHORTED;
									connection_status |= L2_PE_SHORTED;
									connection_status |= L3_PE_SHORTED;
									connection_status |= N_PE_SHORTED;
								}
								else
								{
									connection_reg &= (~PE_L1);
									connection_reg &= (~PE_L2);
									connection_reg &= (~PE_L3);
									connection_reg &= (~PE_N);
									connection_status2 |= SHORTED_AND_OPEN;
									connection_status2 |= MULTI_FAULT;
								}
							}
							cord_cw_count=98;
						}
					}
					if(cord_cw_count!=98)
					{
						if(((connection_status2 & MULTI_FAULT)&&MULTI_FAULT_CONTINUE)|| !(connection_status2 & MULTI_FAULT))
						{
							if(cord_cw_count == 55)transmitCommFunc(__RPE,__GET,__RESULT,"PE_L1",device.device_dir);
							else if(cord_cw_count == 56)transmitCommFunc(__RPE,__GET,__RESULT,"PE_L2",device.device_dir);
							else if(cord_cw_count == 57)transmitCommFunc(__RPE,__GET,__RESULT,"PE_L3",device.device_dir);
							else if(cord_cw_count == 58)transmitCommFunc(__RPE,__GET,__RESULT,"PE_N",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			//dodatno
			case 17://L2_L1 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_status2 |= L1_L2_CRIS_CROSSED;
					}
					else
					{
						//ce nista povezane je multicrossed ali pa crossed and open
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							//gremo nazaj na L2-PE
							rst_REL(33);
							set_REL(30);
							cord_cw_count=8;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(33);
						set_REL(30);
						cord_cw_count=8;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 18://L3_L1 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_status2 |= L1_L3_CRIS_CROSSED;
					}
					else
					{
						//ce nista povezane je multicrossed ali pa crossed and open
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							//gremo nazaj na L3-PE
							rst_REL(33);
							set_REL(34);
							cord_cw_count=20;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(33);
						set_REL(34);
						cord_cw_count=20;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 19://L3_L2 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_status2 |= L2_L3_CRIS_CROSSED;
					}
					else
					{
						//ce nista povezane je multicrossed ali pa crossed and open
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							//gremo nazaj na L3-PE
							rst_REL(34);
							set_REL(33);
							cord_cw_count=21;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(34);
						set_REL(33);
						cord_cw_count=21;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 20://L3_L2 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_status |= L2_L3_CROSSED;
						if(connection_status2 & L1_L3_CRIS_CROSSED)
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= MULTI_SHORTED;
						}
					}
					else
					{
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							//gremo nazaj na L3-PE
							rst_REL(33);
							set_REL(30);
							cord_cw_count=12;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(33);
						set_REL(30);
						cord_cw_count=12;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 21://L3_L1 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_status |= L1_L3_CROSSED;
						if(connection_status2 & L2_L3_CRIS_CROSSED)
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= MULTI_SHORTED;
						}
					}
					else
					{
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							//gremo nazaj na L3-PE
							rst_REL(33);
							set_REL(30);
							cord_cw_count=12;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(33);
						set_REL(30);
						cord_cw_count=12;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 22://N_L1 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_status2 |= L1_N_CRIS_CROSSED;
					}
					else
					{
						//ce nista povezane je multicrossed ali pa crossed and open
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							//gremo nazaj na L3-PE
							rst_REL(33);
							set_REL(34);
							cord_cw_count=25;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(33);
						set_REL(34);
						cord_cw_count=25;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 23://N_L2 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_status2 |= L2_N_CRIS_CROSSED;
					}
					else
					{
						//ce nista povezane je multicrossed ali pa crossed and open
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							//gremo nazaj na L3-PE
							rst_REL(34);
							set_REL(33);
							cord_cw_count=26;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(34);
						set_REL(33);
						cord_cw_count=26;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 24://N_L3 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_status2 |= L3_N_CRIS_CROSSED;
					}
					else
					{
						//ce nista povezane je multicrossed ali pa crossed and open
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							//gremo nazaj na L3-PE
							rst_REL(35);
							set_REL(33);
							cord_cw_count=27;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(35);
						set_REL(33);
						cord_cw_count=27;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 25://N_L2 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_status |= L2_N_CROSSED;
						if(connection_status2 & L1_N_CRIS_CROSSED)
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= MULTI_SHORTED;
						}
					}
					else
					{
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							//gremo nazaj na L3-PE
							rst_REL(34);
							set_REL(35);
							cord_cw_count=28;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(34);
						set_REL(35);
						cord_cw_count=28;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 26://N_L1 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_status |= L1_N_CROSSED;
						if(connection_status2 & L2_N_CRIS_CROSSED)
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= MULTI_SHORTED;
						}
					}
					else
					{
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							//gremo nazaj na L3-PE
							rst_REL(33);
							set_REL(35);
							cord_cw_count=29;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(33);
						set_REL(35);
						cord_cw_count=29;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 27://N_L1 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_status |= L1_N_CROSSED;
						if(connection_status2 & L3_N_CRIS_CROSSED)
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= MULTI_SHORTED;
						}
					}
					else
					{
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							//gremo nazaj na N-PE
							rst_REL(33);
							set_REL(34);
							cord_cw_count=30;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(33);
						set_REL(34);
						cord_cw_count=30;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 28://N_L3 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_status |= L3_N_CROSSED;
						if((connection_status2 & L1_N_CRIS_CROSSED)||(connection_status & L2_N_CROSSED))
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= MULTI_SHORTED;
						}
					}
					else
					{
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							//gremo nazaj na N-PE
							rst_REL(35);
							set_REL(30);
							cord_cw_count=15;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(35);
						set_REL(30);
						cord_cw_count=15;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 29://N_L3 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_status |= L3_N_CROSSED;
						if((connection_status2 & L2_N_CRIS_CROSSED)||(connection_status & L1_N_CROSSED))
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= MULTI_SHORTED;
						}
					}
					else
					{
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							//gremo nazaj na N-PE
							rst_REL(35);
							set_REL(30);
							cord_cw_count=15;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(35);
						set_REL(30);
						cord_cw_count=15;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 30://N_L2 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_status |= L2_N_CROSSED;
						if((connection_status2 & L3_N_CRIS_CROSSED)||(connection_status & L1_N_CROSSED))
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= MULTI_SHORTED;
						}
					}
					else
					{
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							//gremo nazaj na N-PE
							rst_REL(34);
							set_REL(30);
							cord_cw_count=15;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(34);
						set_REL(30);
						cord_cw_count=15;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 31://PE_L1 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_status2 |= L1_PE_CRIS_CROSSED;
						connection_reg |= PE_L1;
					}
					else
					{
						//ce nista povezane je multicrossed ali pa crossed and open
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							//gremo nazaj na L3-PE
							rst_REL(33);
							set_REL(34);
							cord_cw_count=32;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(33);
						set_REL(34);
						cord_cw_count=32;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 32://PE_L2 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_reg |= PE_L2;
						if((connection_status2 & L1_PE_CRIS_CROSSED))
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= CROSSED_AND_SHORTED;
						}
					}
					else
					{
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							//gremo nazaj na N-PE
							rst_REL(34);
							set_REL(35);
							cord_cw_count=33;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(34);
						set_REL(35);
						cord_cw_count=33;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 33://PE_L3 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_reg |= PE_L3;
						if((connection_status2 & L3_PE_CRIS_CROSSED)||(connection_reg & PE_L2))
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= CROSSED_AND_SHORTED;
						}
					}
					else
					{
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							//gremo nazaj na N-PE
							rst_REL(35);
							set_REL(36);
							cord_cw_count=34;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(35);
						set_REL(36);
						cord_cw_count=34;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 34://PE_N povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_reg |= PE_N;
						if((connection_status2 & L3_PE_CRIS_CROSSED)||(connection_reg & PE_L2)||(connection_reg & PE_L3))
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= CROSSED_AND_SHORTED;
						}
					}
					else
					{
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							//gremo nazaj na N-PE
							rst_REL(36);
							set_REL(30);
							cord_cw_count=98;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(36);
						set_REL(30);
						cord_cw_count=98;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 35://PE_L2 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_status2 |= L2_PE_CRIS_CROSSED;
						connection_reg |= PE_L2;
					}
					else
					{
						//ce nista povezane je multicrossed ali pa crossed and open
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							//gremo nazaj na L3-PE
							rst_REL(34);
							set_REL(33);
							cord_cw_count=36;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(34);
						set_REL(33);
						cord_cw_count=36;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 36://PE_L1 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_reg |= PE_L1;
						if((connection_status2 & L2_PE_CRIS_CROSSED))
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= CROSSED_AND_SHORTED;
						}
					}
					else
					{
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							rst_REL(33);
							set_REL(35);
							cord_cw_count=37;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(33);
						set_REL(35);
						cord_cw_count=37;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 37://PE_L3 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_reg |= PE_L3;
						if((connection_status2 & L2_PE_CRIS_CROSSED)||(connection_reg & PE_L1))
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= CROSSED_AND_SHORTED;
						}
					}
					else
					{
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							rst_REL(35);
							set_REL(36);
							cord_cw_count=38;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(35);
						set_REL(36);
						cord_cw_count=38;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 38://PE_N povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_reg |= PE_N;
						if((connection_status2 & L2_PE_CRIS_CROSSED)||(connection_reg & PE_L1)||(connection_reg & PE_L3))
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= CROSSED_AND_SHORTED;
						}
					}
					else
					{
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							rst_REL(36);
							set_REL(30);
							cord_cw_count=98;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(36);
						set_REL(30);
						cord_cw_count=98;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 39://PE_L3 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_status2 |= L3_PE_CRIS_CROSSED;
						connection_reg |= PE_L3;
					}
					else
					{
						//ce nista povezane je multicrossed ali pa crossed and open
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							//gremo nazaj na L3-PE
							rst_REL(35);
							set_REL(33);
							cord_cw_count=40;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(35);
						set_REL(33);
						cord_cw_count=40;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 40://PE_L1 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_reg |= PE_L1;
						if((connection_status2 & L3_PE_CRIS_CROSSED))
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= CROSSED_AND_SHORTED;
						}
					}
					else
					{
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							rst_REL(33);
							set_REL(34);
							cord_cw_count=41;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(33);
						set_REL(34);
						cord_cw_count=41;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 41://PE_L2 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_reg |= PE_L2;
						if((connection_status2 & L3_PE_CRIS_CROSSED)||(connection_reg & PE_L1))
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= CROSSED_AND_SHORTED;
						}
					}
					else
					{
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							rst_REL(34);
							set_REL(36);
							cord_cw_count=42;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(34);
						set_REL(36);
						cord_cw_count=42;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 42://PE_N povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_reg |= PE_N;
						if((connection_status2 & L3_PE_CRIS_CROSSED)||(connection_reg & PE_L1)||(connection_reg & PE_L2))
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= CROSSED_AND_SHORTED;
						}
					}
					else
					{
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							rst_REL(36);
							set_REL(30);
							cord_cw_count=98;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(36);
						set_REL(30);
						cord_cw_count=98;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 43://PE_N povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_status2 |= N_PE_CRIS_CROSSED;
						connection_reg |= PE_N;
					}
					else
					{
						//ce nista povezane je multicrossed ali pa crossed and open
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							rst_REL(36);
							set_REL(33);
							cord_cw_count=44;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(36);
						set_REL(33);
						cord_cw_count=44;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 44://PE_L1 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_reg |= PE_L1;
						if((connection_status2 & N_PE_CRIS_CROSSED))
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= CROSSED_AND_SHORTED;
						}
					}
					else
					{
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							rst_REL(33);
							set_REL(34);
							cord_cw_count=45;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(33);
						set_REL(34);
						cord_cw_count=45;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 45://PE_L2 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_reg |= PE_L2;
						if((connection_status2 & N_PE_CRIS_CROSSED)||(connection_reg & PE_L1))
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= CROSSED_AND_SHORTED;
						}
					}
					else
					{
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							rst_REL(34);
							set_REL(35);
							cord_cw_count=46;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(34);
						set_REL(35);
						cord_cw_count=46;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 46://PE_L3 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						connection_reg |= PE_L3;
						if((connection_status2 & N_PE_CRIS_CROSSED)||(connection_reg & PE_L1)||(connection_reg & PE_L2))
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= CROSSED_AND_SHORTED;
						}
					}
					else
					{
						;
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							rst_REL(35);
							set_REL(30);
							cord_cw_count=98;
						}
						else cord_cw_count = 100;
					}
					else
					{
						rst_REL(35);
						set_REL(30);
						cord_cw_count=98;
					}
					if(((connection_status2 & MULTI_FAULT)&&MULTI_FAULT_CONTINUE)|| !(connection_status2 & MULTI_FAULT))
					{
						transmitCommFunc(__RPE,__GET,__RESULT,"",device.device_dir);
						cord_task_control |= __CORD_RPE_RES_REQUESTED;
					}
				}
				break;
			}
			///od tle naprej je nova koda
			case 47://L2_L1 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						if(connection_reg & L2_L2)
						{
							if(connection_status & (ALL_SHORTED_MASKS & (~L1_L2_SHORTED)))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_SHORTED;
							}
	//						connection_status |= L1_L2_SHORTED;
						}
						else
						{
							//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
							if(connection_reg & L2_CROSSED_MASK2)
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= CROSSED_AND_SHORTED;
							}
							if(connection_status & (ALL_CROSSED_MASKS & (~L1_L2_CROSSED)))
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_CROSSED;
							}
						}
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else
								cord_cw_count =100;
						}
						else
						{
							set_next_cord_task_case = true;
						}
						connection_reg |= L2_L1;
					}
					else
					{
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else cord_cw_count = 100;
						}
						else
						{
							set_next_cord_task_case = true;
						}
						connection_reg &= (~L2_L1);
					}
					if(set_next_cord_task_case == true)
					{
						set_next_cord_task_case = false;
						rst_REL(33);
						set_REL(30);
						cord_cw_count=8;
					}
					if(((connection_status2 & MULTI_FAULT)&&MULTI_FAULT_CONTINUE)|| !(connection_status2 & MULTI_FAULT))
					{
						if(cord_cw_count == 8)transmitCommFunc(__RPE,__GET,__RESULT,"L2_PE",device.device_dir);
						cord_task_control |= __CORD_RPE_RES_REQUESTED;
					}
				}
				break;
			}
			case 48://L3_L2 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						if(connection_reg & L3_L3)
						{
							if(connection_status & (ALL_SHORTED_MASKS & (~L2_L3_SHORTED)))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_SHORTED;
							}
	//						connection_status |= L1_L2_SHORTED;
						}
						else
						{
							//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
							if(connection_reg & L3_CROSSED_MASK2)
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= CROSSED_AND_SHORTED;
							}
							if(connection_status & (ALL_CROSSED_MASKS & (~L2_L3_CROSSED)))
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_CROSSED;
							}
						}
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else
								cord_cw_count =100;
						}
						else
						{
							set_next_cord_task_case = true;
						}
						connection_reg |= L3_L2;
					}
					else
					{
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else cord_cw_count = 100;
						}
						else
						{
							set_next_cord_task_case = true;
						}
						connection_reg &= (~L3_L2);
					}
					if(set_next_cord_task_case == true)
					{
						set_next_cord_task_case = false;
						rst_REL(34);
						set_REL(30);
						cord_cw_count=12;
					}
					if(((connection_status2 & MULTI_FAULT)&&MULTI_FAULT_CONTINUE)|| !(connection_status2 & MULTI_FAULT))
					{
						if(cord_cw_count==12)transmitCommFunc(__RPE,__GET,__RESULT,"L3_PE",device.device_dir);
						cord_task_control |= __CORD_RPE_RES_REQUESTED;
					}
				}
				break;
			}
			case 49://L3_L1 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						if(connection_reg & L3_L3)
						{
							if(connection_status & (ALL_SHORTED_MASKS & (~L1_L3_SHORTED)))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_SHORTED;
							}
						}
						else
						{
							//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
							if(connection_reg & L3_CROSSED_MASK2)
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= CROSSED_AND_SHORTED;
							}
							if(connection_status & (ALL_CROSSED_MASKS & (~L1_L3_CROSSED)))
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_CROSSED;
							}
						}
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else
								cord_cw_count =100;
						}
						else
						{
							set_next_cord_task_case = true;
						}
						connection_reg |= L3_L1;
					}
					else
					{
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else cord_cw_count = 100;
						}
						else
						{
							set_next_cord_task_case = true;
						}
						connection_reg &= (~L3_L1);
					}
					if(set_next_cord_task_case == true)
					{
						set_next_cord_task_case = false;
						rst_REL(33);
						set_REL(30);
						cord_cw_count=12;
					}
					if(((connection_status2 & MULTI_FAULT)&&MULTI_FAULT_CONTINUE)|| !(connection_status2 & MULTI_FAULT))
					{
						if(cord_cw_count == 12)transmitCommFunc(__RPE,__GET,__RESULT,"L3_PE",device.device_dir);
						cord_task_control |= __CORD_RPE_RES_REQUESTED;
					}
				}
				break;
			}
			case 50://L3_L1 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						if(connection_reg & L3_L3)
						{
							if(connection_status & (ALL_SHORTED_MASKS & (~L1_L3_SHORTED)))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_SHORTED;
							}
						}
						else
						{
							//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
							if(connection_reg & L3_CROSSED_MASK2)
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= CROSSED_AND_SHORTED;
							}
							if(connection_status & (ALL_CROSSED_MASKS & (~L1_L3_CROSSED)))
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_CROSSED;
							}
						}
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
									set_next_cord_task_case = true;
							}
							else
								cord_cw_count =100;
						}
						else
						{
								set_next_cord_task_case = true;
						}
						connection_reg |= L3_L1;
					}
					else
					{
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else cord_cw_count = 100;
						}
						else
						{
							set_next_cord_task_case = true;
						}
						connection_reg &= (~L3_L1);
					}
					if(set_next_cord_task_case == true)
					{
						set_next_cord_task_case = false;
						rst_REL(33);
						set_REL(34);
						cord_cw_count=51;
					}
					if(((connection_status2 & MULTI_FAULT)&&MULTI_FAULT_CONTINUE)|| !(connection_status2 & MULTI_FAULT))
					{
						transmitCommFunc(__RPE,__GET,__RESULT,"L3_L2",device.device_dir);
						cord_task_control |= __CORD_RPE_RES_REQUESTED;
					}
				}
				break;
			}
			case 51://L3_L2 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						if(connection_reg & L3_L3)
						{
							if((connection_status & (ALL_SHORTED_MASKS & (~L2_L3_SHORTED)))||(connection_reg & L3_L1))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_SHORTED;
							}
						}
						else
						{
							//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
							if(connection_reg & L3_CROSSED_MASK2)
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= CROSSED_AND_SHORTED;
							}
							if(connection_status & (ALL_CROSSED_MASKS & (~L2_L3_CROSSED)))
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_CROSSED;
							}
						}
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else
								cord_cw_count =100;
						}
						else
						{
							set_next_cord_task_case = true;
						}
						connection_reg |= L3_L2;
					}
					else
					{
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else cord_cw_count = 100;
						}
						else
						{
							set_next_cord_task_case = true;
						}
						connection_reg &= (~L3_L2);
					}
					if(set_next_cord_task_case == true)
					{
						set_next_cord_task_case = false;
						rst_REL(34);
						set_REL(30);
						cord_cw_count=12;
					}
					if(((connection_status2 & MULTI_FAULT)&&MULTI_FAULT_CONTINUE)|| !(connection_status2 & MULTI_FAULT))
					{
						transmitCommFunc(__RPE,__GET,__RESULT,"L3_PE",device.device_dir);
						cord_task_control |= __CORD_RPE_RES_REQUESTED;
					}
				}
				break;
			}
			case 52://N_L1 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						if(connection_reg & N_N)
						{
							if(connection_status & (ALL_SHORTED_MASKS & (~L1_N_SHORTED)))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_SHORTED;
							}
							
						}
						else
						{
							//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
							if(connection_reg & N_CROSSED_MASK2)
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= CROSSED_AND_SHORTED;
							}
							if(connection_status & (ALL_CROSSED_MASKS & (~L1_N_CROSSED)))
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_CROSSED;
							}
						}
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
								set_next_cord_task_case = true;
							else
								cord_cw_count =100;
						}
						else
							set_next_cord_task_case = true;
						connection_reg |= N_L1;
					}
					else
					{
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
								set_next_cord_task_case = true;
							else cord_cw_count = 100;
						}
						else
							set_next_cord_task_case = true;
						connection_reg &= (~N_L1);
					}
					if(set_next_cord_task_case)
					{
						set_next_cord_task_case = false;
						if(manual_set_phase_num && CORD_PHASE_NUM_SETTING == _1_PHASE)
						{
							rst_REL(33);
							set_REL(30);
							cord_cw_count=15;
						}
						else
						{
							if(!((connection_reg & L2_L2)&&(connection_reg & L2_N))) //gremo na L2 - PE
							{
								rst_REL(33);
								set_REL(34);
								cord_cw_count=53;
							}
							else if(!((connection_reg & L3_L3)&&(connection_reg & L3_N))) //gremo na L2 - PE
							{
								if(connection_reg & N_N)
								{
									connection_reg |= N_L2;
									connection_status |= L2_N_SHORTED;
								}
								else
								{
									connection_reg &= (~N_L2);
									connection_status2 |= SHORTED_AND_OPEN;
									connection_status2 |= MULTI_FAULT;
								}
								rst_REL(33);
								set_REL(35);
								cord_cw_count=54;
							}
							else 
							{
								if(connection_reg & N_N)
								{
									connection_reg |= N_L2;
									connection_reg |= N_L3;
									connection_status |= L2_N_SHORTED;
									connection_status |= L3_N_SHORTED;
								}
								else
								{
									connection_reg &= (~N_L2);
									connection_reg &= (~N_L3);
									connection_status2 |= SHORTED_AND_OPEN;
									connection_status2 |= MULTI_FAULT;
								}
								rst_REL(33);
								set_REL(30);
								cord_cw_count=15;
							}
						}
					}
					if(((connection_status2 & MULTI_FAULT)&&MULTI_FAULT_CONTINUE)|| !(connection_status2 & MULTI_FAULT))
					{
						if(cord_cw_count ==15)transmitCommFunc(__RPE,__GET,__RESULT,"N_PE",device.device_dir);
						else if(cord_cw_count ==53)transmitCommFunc(__RPE,__GET,__RESULT,"N_L2",device.device_dir);
						else if(cord_cw_count ==54)transmitCommFunc(__RPE,__GET,__RESULT,"N_L3",device.device_dir);
						cord_task_control |= __CORD_RPE_RES_REQUESTED;
					}
				}
				break;
			}
			case 53://N_L2 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						if(connection_reg & N_N)
						{
							if((connection_status & (ALL_SHORTED_MASKS & (~L2_N_SHORTED)))||(connection_reg & N_L1))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_SHORTED;
							}
						}
						else
						{
							//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
							if(connection_reg & N_CROSSED_MASK2)
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= CROSSED_AND_SHORTED;
							}
							if(connection_status & (ALL_CROSSED_MASKS & (~L2_N_CROSSED)))
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_CROSSED;
							}
						}
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else
								cord_cw_count =100;
						}
						else
						{
							set_next_cord_task_case = true;
						}
						connection_reg |= N_L2;
					}
					else
					{
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else cord_cw_count = 100;
						}
						else
						{
							set_next_cord_task_case = true;
						}
						connection_reg &= (~N_L2);
					}
					if(set_next_cord_task_case == true)
					{
						set_next_cord_task_case = false;
						if(!((connection_reg & L3_L3)&&(connection_reg & L3_N))) //gremo na L2 - PE
						{
							rst_REL(33);
							set_REL(35);
							cord_cw_count=54;
						}
						else 
						{
							if(connection_reg & N_N)
							{
								connection_reg |= N_L3;
								connection_status |= L3_N_SHORTED;
							}
							else
							{
								connection_reg &= (~N_L3);
								connection_status2 |= SHORTED_AND_OPEN;
								connection_status2 |= MULTI_FAULT;
							}
							rst_REL(33);
							set_REL(30);
							cord_cw_count=15;
						}
					}
					if(((connection_status2 & MULTI_FAULT)&&MULTI_FAULT_CONTINUE)|| !(connection_status2 & MULTI_FAULT))
					{
						if(cord_cw_count == 54)transmitCommFunc(__RPE,__GET,__RESULT,"N_L3",device.device_dir);
						else if(cord_cw_count == 15)transmitCommFunc(__RPE,__GET,__RESULT,"N_PE",device.device_dir);
						cord_task_control |= __CORD_RPE_RES_REQUESTED;
					}
				}
				break;
			}
			case 54://N_L3 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						if(connection_reg & N_N)
						{
							if((connection_status & (ALL_SHORTED_MASKS & (~L3_N_SHORTED)))||(connection_reg & N_L1)||(connection_reg & N_L2))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_SHORTED;
							}
						}
						else
						{
							//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
							if(connection_reg & N_CROSSED_MASK2)
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= CROSSED_AND_SHORTED;
							}
							if(connection_status & (ALL_CROSSED_MASKS & (~L3_N_CROSSED)))
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_CROSSED;
							}
						}
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case= true;
							}
							else
								cord_cw_count =100;
						}
						else
						{
							set_next_cord_task_case= true;
						}
						connection_reg |= N_L3;
					}
					else
					{
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case= true;
							}
							else cord_cw_count = 100;
						}
						else
						{
							set_next_cord_task_case= true;
						}
						connection_reg &= (~N_L3);
					}
					if(set_next_cord_task_case ==true)
					{
						set_next_cord_task_case = false;
						rst_REL(35);
						set_REL(30);
						cord_cw_count=15;
					}
					if(((connection_status2 & MULTI_FAULT)&&MULTI_FAULT_CONTINUE)|| !(connection_status2 & MULTI_FAULT))
					{
						if(cord_cw_count == 15)transmitCommFunc(__RPE,__GET,__RESULT,"N_PE",device.device_dir);
						cord_task_control |= __CORD_RPE_RES_REQUESTED;
					}
				}
				break;
			}
			case 55://PE_L1 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						if(connection_reg & PE_PE)
						{
							if(connection_status & (ALL_SHORTED_MASKS & (~L1_PE_SHORTED)))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_SHORTED;
							}
						}
						else
						{
							//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
							if(connection_reg & PE_CROSSED_MASK2)
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= CROSSED_AND_SHORTED;
							}
							if(connection_status & (ALL_CROSSED_MASKS & (~L1_PE_CROSSED)))
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_CROSSED;
							}
						}
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else
								cord_cw_count =100;
						}
						else
						{
							set_next_cord_task_case = true;
	//						if(!((connection_reg & L2_L2)&&(connection_reg & L2_PE))) //gremo na L2 - PE
	//								{
	//									rst_REL(33);
	//									set_REL(34);
	//									cord_cw_count=56;
	//								}
	//								else if(!((connection_reg & L3_L3)&&(connection_reg & L3_PE))) //gremo na L2 - PE
	//								{
	//									if(connection_reg & PE_PE)
	//									{
	//										connection_reg |= PE_L2;
	//										connection_status |= L2_PE_SHORTED;
	//									}
	//									else
	//									{
	//										connection_reg &= (~PE_L2);
	//										connection_status2 |= SHORTED_AND_OPEN;
	//										connection_status2 |= MULTI_FAULT;
	//									}
	//									rst_REL(33);
	//									set_REL(35);
	//									cord_cw_count=57;
	//								}
	//								else if(!((connection_reg & N_N)&&(connection_reg & N_PE))) //gremo na L2 - PE
	//								{
	//									if(connection_reg & PE_PE)
	//									{
	//										connection_reg |= PE_L2;
	//										connection_reg |= PE_L3;
	//										connection_status |= L2_PE_SHORTED;
	//										connection_status |= L3_PE_SHORTED;
	//									}
	//									else
	//									{
	//										connection_reg &= (~PE_L2);
	//										connection_reg &= (~PE_L3);
	//										connection_status2 |= SHORTED_AND_OPEN;
	//										connection_status2 |= MULTI_FAULT;
	//									}
	//									rst_REL(33);
	//									set_REL(36);
	//									cord_cw_count=58;
	//								}
	//								else 
	//								{
	//									if(connection_reg & PE_PE)
	//									{
	//										connection_reg |= PE_L2;
	//										connection_reg |= PE_L3;
	//										connection_reg |= PE_N;
	//										connection_status |= L2_PE_SHORTED;
	//										connection_status |= L3_PE_SHORTED;
	//										connection_status |= N_PE_SHORTED;
	//									}
	//									else
	//									{
	//										connection_reg &= (~PE_L2);
	//										connection_reg &= (~PE_L3);
	//										connection_reg &= (~PE_N);
	//										connection_status2 |= SHORTED_AND_OPEN;
	//										connection_status2 |= MULTI_FAULT;
	//									}
	//									rst_REL(33);
	//									set_REL(30);
	//									cord_cw_count=98;
	//								}
						}
						connection_reg |= PE_L1;
					}
					else
					{
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else cord_cw_count = 100;
						}
						else
						{
							set_next_cord_task_case = true;
						}
						connection_reg &= (~PE_L1);
					}
					if(set_next_cord_task_case)
					{
						set_next_cord_task_case = false;
						if((!((connection_reg & L2_L2)&&(connection_reg & L2_PE)))&&(!(manual_set_phase_num && CORD_PHASE_NUM_SETTING==_1_PHASE))) //gremo na L2 - PE
						{
							rst_REL(33);
							set_REL(34);
							cord_cw_count=56;
						}
						else if((!((connection_reg & L3_L3)&&(connection_reg & L3_PE)))&&(!(manual_set_phase_num && CORD_PHASE_NUM_SETTING==_1_PHASE))) //gremo na L2 - PE
						{
							if(connection_reg & PE_PE)
							{
								connection_reg |= PE_L2;
								connection_status |= L2_PE_SHORTED;
							}
							else
							{
								connection_reg &= (~PE_L2);
								connection_status2 |= SHORTED_AND_OPEN;
								connection_status2 |= MULTI_FAULT;
							}
							rst_REL(33);
							set_REL(35);
							cord_cw_count=57;
						}
						else if(!((connection_reg & N_N)&&(connection_reg & N_PE))) //gremo na L2 - PE
						{
							if(manual_set_phase_num && CORD_PHASE_NUM_SETTING==_1_PHASE)
							{
								;
							}
							else
							{
								if(connection_reg & PE_PE)
								{
									connection_reg |= PE_L2;
									connection_reg |= PE_L3;
									connection_status |= L2_PE_SHORTED;
									connection_status |= L3_PE_SHORTED;
								}
								else
								{
									connection_reg &= (~PE_L2);
									connection_reg &= (~PE_L3);
									connection_status2 |= SHORTED_AND_OPEN;
									connection_status2 |= MULTI_FAULT;
								}
							}
							rst_REL(33);
							set_REL(36);
							cord_cw_count=58;
						}
						else 
						{
							if(manual_set_phase_num && CORD_PHASE_NUM_SETTING==_1_PHASE)
							{
								if(connection_reg & PE_PE)
								{
									connection_reg |= PE_N;
									connection_status |= N_PE_SHORTED;
								}
								else
								{
									connection_reg &= (~PE_N);
									connection_status2 |= SHORTED_AND_OPEN;
									connection_status2 |= MULTI_FAULT;
								}
							}
							else
							{
								if(connection_reg & PE_PE)
								{
									connection_reg |= PE_L2;
									connection_reg |= PE_L3;
									connection_reg |= PE_N;
									connection_status |= L2_PE_SHORTED;
									connection_status |= L3_PE_SHORTED;
									connection_status |= N_PE_SHORTED;
								}
								else
								{
									connection_reg &= (~PE_L2);
									connection_reg &= (~PE_L3);
									connection_reg &= (~PE_N);
									connection_status2 |= SHORTED_AND_OPEN;
									connection_status2 |= MULTI_FAULT;
								}
								rst_REL(33);
								set_REL(30);
								cord_cw_count=98;
							}
						}
					}
					if(cord_cw_count!=98)
					{
						if(((connection_status2 & MULTI_FAULT)&&MULTI_FAULT_CONTINUE)|| !(connection_status2 & MULTI_FAULT))
						{
							if(cord_cw_count == 56)transmitCommFunc(__RPE,__GET,__RESULT,"PE_L2",device.device_dir);
							else if(cord_cw_count == 57)transmitCommFunc(__RPE,__GET,__RESULT,"PE_L3",device.device_dir);
							else if(cord_cw_count == 58)transmitCommFunc(__RPE,__GET,__RESULT,"PE_N",device.device_dir);
							cord_task_control |= __CORD_RPE_RES_REQUESTED;
						}
					}
				}
				break;
			}
			case 56://PE_L2 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						if(connection_reg & PE_PE)
						{
							if((connection_status & (ALL_SHORTED_MASKS & (~L2_PE_SHORTED)))||(connection_reg & PE_L1))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_SHORTED;
							}
						}
						else
						{
							//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
							if(connection_reg & PE_CROSSED_MASK2)
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= CROSSED_AND_SHORTED;
							}
							if(connection_status & (ALL_CROSSED_MASKS & (~L2_PE_CROSSED)))
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_CROSSED;
							}
						}
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else
								cord_cw_count =100;
						}
						else
						{
							set_next_cord_task_case = true;
						}
						connection_reg |= PE_L2;
					}
					else
					{
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else cord_cw_count = 100;
						}
						else
						{
							set_next_cord_task_case = true;
						}
						connection_reg &= (~PE_L2);
					}
					if(set_next_cord_task_case == true)
					{
						set_next_cord_task_case = false;
						if(!((connection_reg & L3_L3)&&(connection_reg & L3_PE))) //gremo na L2 - PE
						{
							rst_REL(34);
							set_REL(35);
							cord_cw_count=57;
						}
						else if(!((connection_reg & N_N)&&(connection_reg & N_PE))) //gremo na L2 - PE
						{
							if(connection_reg & PE_PE)
							{
								connection_reg |= PE_L3;
								connection_status |= L2_PE_SHORTED;
								connection_status |= L3_PE_SHORTED;
							}
							else
							{
								connection_reg &= (~PE_L3);
								connection_status2 |= SHORTED_AND_OPEN;
								connection_status2 |= MULTI_FAULT;
							}
							rst_REL(34);
							set_REL(36);
							cord_cw_count=58;
						}
						else 
						{
							if(connection_reg & PE_PE)
							{
								connection_reg |= PE_L3;
								connection_reg |= PE_N;
								connection_status |= L3_PE_SHORTED;
								connection_status |= N_PE_SHORTED;
							}
							else
							{
								connection_reg &= (~PE_L3);
								connection_reg &= (~PE_N);
								connection_status2 |= SHORTED_AND_OPEN;
								connection_status2 |= MULTI_FAULT;
							}
							rst_REL(34);
							set_REL(30);
							cord_cw_count=98;
						}
					}
					if(((connection_status2 & MULTI_FAULT)&&MULTI_FAULT_CONTINUE)|| !(connection_status2 & MULTI_FAULT))
					{
						if(cord_cw_count == 57)transmitCommFunc(__RPE,__GET,__RESULT,"PE_L3",device.device_dir);
						else if(cord_cw_count == 58)transmitCommFunc(__RPE,__GET,__RESULT,"PE_N",device.device_dir);
						cord_task_control |= __CORD_RPE_RES_REQUESTED;
					}

				}
				break;
			}
			case 57://PE_L3 povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						if(connection_reg & PE_PE)
						{
							if((connection_status & (ALL_SHORTED_MASKS & (~L3_PE_SHORTED)))||(connection_reg & (PE_L1 | PE_L2)))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_SHORTED;
							}
						}
						else
						{
							//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
							if(connection_reg & PE_CROSSED_MASK2)
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= CROSSED_AND_SHORTED;
							}
							if(connection_status & (ALL_CROSSED_MASKS & (~L3_PE_CROSSED)))
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_CROSSED;
							}
						}
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else
								cord_cw_count =100;
						}
						else
						{
							set_next_cord_task_case = true;
						}
						connection_reg |= PE_L3;
					}
					else
					{
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else cord_cw_count = 100;
						}
						else
						{
							set_next_cord_task_case = true;
						}
						connection_reg &= (~PE_L3);
					}
					if(set_next_cord_task_case == true)
					{
						set_next_cord_task_case = false;
						if(!((connection_reg & N_N)&&(connection_reg & N_PE))) //gremo na L2 - PE
						{
							rst_REL(35);
							set_REL(36);
							cord_cw_count=58;
						}
						else 
						{
							if(connection_reg & PE_PE)
							{
								connection_reg |= PE_N;
								connection_status |= L3_PE_SHORTED;
								connection_status |= N_PE_SHORTED;
							}
							else
							{
								connection_reg &= (~PE_N);
								connection_status2 |= SHORTED_AND_OPEN;
								connection_status2 |= MULTI_FAULT;
							}
							rst_REL(35);
							set_REL(30);
							cord_cw_count=98;
						}
					}
					if(((connection_status2 & MULTI_FAULT)&&MULTI_FAULT_CONTINUE)|| !(connection_status2 & MULTI_FAULT))
					{
						if(cord_cw_count == 58)transmitCommFunc(__RPE,__GET,__RESULT,"PE_N",device.device_dir);
						cord_task_control |= __CORD_RPE_RES_REQUESTED;
					}
				}
				break;
			}
			case 58://PE_N povezano?
			{
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					if(cord_check_RPE_resistance())
					{
						if(connection_reg & PE_PE)
						{
							if((connection_status & (ALL_SHORTED_MASKS & (~N_PE_SHORTED)))||(connection_reg & (PE_L1 | PE_L2 | PE_L3)))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_SHORTED;
							}
						}
						else
						{
							//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
							if(connection_reg & PE_CROSSED_MASK2)
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= CROSSED_AND_SHORTED;
							}
							if(connection_status & (ALL_CROSSED_MASKS & (~N_PE_CROSSED)))
							{
								connection_status2 |= MULTI_FAULT;
								connection_status2 |= MULTI_CROSSED;
							}
						}
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else
								cord_cw_count =100;
						}
						else
						{
							set_next_cord_task_case = true;
						}
						connection_reg |= PE_N;
					}
					else
					{
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
								set_next_cord_task_case = true;
							}
							else cord_cw_count = 100;
						}
						else
						{
							set_next_cord_task_case = true;
						}
						connection_reg &= (~PE_N);
					}
					if(set_next_cord_task_case == true)
					{
						set_next_cord_task_case = false;
						rst_REL(36);
						set_REL(30);
						cord_cw_count=98;
					}
	//				if(connection_status2 & MULTI_FAULT)
	//				{
	//					if(MULTI_FAULT_CONTINUE)
	//					{
	//						transmitCommFunc(__RPE,__GET,__RESULT,"L1_PE",device.device_dir);"","PE_N",device.device_dir);
	//						cord_task_control |= __CORD_RPE_RES_REQUESTED;
	//					}
	//				}
				}
				break;
			}
			case 98:
			{
					if(!(connection_reg & PE_PE))
					{
						if((connection_reg & L2_PE)&&(connection_reg & PE_L2))
							connection_status2 |= L2_PE_CRIS_CROSSED;
						else if((connection_reg & L1_PE)&&(connection_reg & PE_L1))
							connection_status2 |= L1_PE_CRIS_CROSSED;
						else if((connection_reg & L3_PE)&&(connection_reg & PE_L3))
							connection_status2 |= L3_PE_CRIS_CROSSED;
						else if((connection_reg & N_PE)&&(connection_reg & N_L3))
							connection_status2 |= N_PE_CRIS_CROSSED;
						else
						{
							if((numberOfSetBits(connection_reg & (PE_CROSSED_MASK | PE_CROSSED_MASK2)))==0)
							{
								if(connection_status & (L1_OPEN | L2_OPEN | L3_OPEN | N_OPEN))
								{
									connection_status2 |= MULTI_FAULT;
									connection_status2 |= MULTI_OPENED;
								}
								connection_status |= PE_OPEN; 
							}
							else if((numberOfSetBits(connection_reg & (PE_CROSSED_MASK | PE_CROSSED_MASK2)))==1)
							{
								switch (connection_reg & (PE_CROSSED_MASK | PE_CROSSED_MASK2))
								{
									case PE_L1: connection_status2 |= PE_L1_CROSSED; break;	
									case PE_L2: connection_status2 |= PE_L2_CROSSED; break;
									case PE_L3: connection_status2 |= PE_L3_CROSSED; break;
									case L1_PE: connection_status |= L1_PE_CROSSED; break;	
									case L2_PE: connection_status |= L2_PE_CROSSED; break;
									case L3_PE: connection_status |= L3_PE_CROSSED; break;
									case N_PE: connection_status |= N_PE_CROSSED; break;
									case PE_N: connection_status2 |= PE_N_CROSSED; break;
									default: break;									
								}
							}
							else if((numberOfSetBits(connection_reg & (PE_CROSSED_MASK | PE_CROSSED_MASK2)))==2)
							{
								if(connection_reg & L1_PE)connection_status |= L1_PE_CROSSED;
								if(connection_reg & L2_PE)connection_status |= L2_PE_CROSSED;
								if(connection_reg & L3_PE)connection_status |= L3_PE_CROSSED;
								if(connection_reg & PE_N)connection_status2 |= PE_N_CROSSED;
								if(connection_reg & PE_L1)connection_status2 |= PE_L1_CROSSED;
								if(connection_reg & PE_L2)connection_status2 |= PE_L2_CROSSED;
								if(connection_reg & PE_L3)connection_status2 |= PE_L3_CROSSED;
								if(connection_reg & N_PE)connection_status |= N_PE_CROSSED;
								if((numberOfSetBits(connection_reg & PE_CROSSED_MASK))>1)	//pogledamo ce jih vec postavljenih na eni strani
								{
									connection_status2 |= MULTI_FAULT;
									connection_status2 |= CROSSED_AND_SHORTED;	
								}
								else if((numberOfSetBits(connection_reg & PE_CROSSED_MASK2))>1)
								{
									connection_status2 |= MULTI_FAULT;	
									connection_status2 |= CROSSED_AND_SHORTED;	
								}
								else 
								{
									connection_status2 |= MULTI_FAULT;	
									connection_status2 |= MULTI_CROSSED;	
								}
							}
							else
							{
								if(connection_reg & L1_PE)connection_status |= L1_PE_CROSSED;
								if(connection_reg & L2_PE)connection_status |= L2_PE_CROSSED;
								if(connection_reg & L3_PE)connection_status |= L3_PE_CROSSED;
								if(connection_reg & PE_N)connection_status2 |= PE_N_CROSSED;
								if(connection_reg & PE_L1)connection_status2 |= PE_L1_CROSSED;
								if(connection_reg & PE_L2)connection_status2 |= PE_L2_CROSSED;
								if(connection_reg & PE_L3)connection_status2 |= PE_L3_CROSSED;
								if(connection_reg & N_PE)connection_status |= N_PE_CROSSED;
								connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
								connection_status2 |= CROSSED_AND_SHORTED;
							}
						}
					}
					//ce ni crossed in shorted potem pomeni, da je OK
					else
					{
						if(numberOfSetBits((connection_status & (L1_L1|L1_PE|PE_L1)))>=2)
						{
							switch(numberOfSetBits((connection_status & (L1_L1|L1_PE|PE_L1))))
							{
								case 2:connection_status2 |= CROSSED_AND_OPEN; connection_status2 |= MULTI_FAULT;break;
								case 3:connection_status |= L1_PE_SHORTED; break;
								default: break;
							}
						}
						if(numberOfSetBits((connection_status & (L2_L2|L2_PE|PE_L2)))>=2)
						{
							switch(numberOfSetBits((connection_status & (L2_L2|L2_PE|PE_L2))))
							{
								case 2:connection_status2 |= CROSSED_AND_OPEN; connection_status2 |= MULTI_FAULT;break;
								case 3:connection_status |= L2_PE_SHORTED; break;
								default: break;
							}
						}
						if(numberOfSetBits((connection_status & (L3_L3|L3_PE|PE_L3)))>=2)
						{
							switch(numberOfSetBits((connection_status & (L3_L3|L3_PE|PE_L3))))
							{
								case 2:connection_status2 |= CROSSED_AND_OPEN; connection_status2 |= MULTI_FAULT;break;
								case 3:connection_status |= L3_PE_SHORTED; break;
								default: break;
							}
						}
						if(numberOfSetBits((connection_status & (N_N|N_PE|PE_N)))>=2)
						{
							switch(numberOfSetBits((connection_status & (N_N|N_PE|PE_N))))
							{
								case 2:connection_status2 |= CROSSED_AND_OPEN; connection_status2 |= MULTI_FAULT;break;
								case 3:connection_status |= N_PE_SHORTED; break;
								default: break;
							}
						}
						if(numberOfSetBits(connection_status & ALL_SHORTED_MASKS)>1)
							connection_status2 |= MULTI_FAULT;
						if(numberOfSetBits(connection_status & PE_SHORTED_MASK)>=1)
							;
						else if(!((numberOfSetBits(connection_reg & (PE_CROSSED_MASK | PE_CROSSED_MASK2)))>0))
							connection_status |= PE_OK;
					}
	//			if(!(connection_reg & PE_PE))
	//			{
	//				if(connection_status2 & L1_PE_CRIS_CROSSED)
	//				{
	//					if(connection_reg & (L2_PE | L3_PE | N_PE | PE_N | PE_L2 | PE_L3))
	//					{
	//						connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
	//						connection_status2 |= CROSSED_AND_SHORTED;
	//					}
	//				}
	//				else if(connection_status2 & L2_PE_CRIS_CROSSED)
	//				{
	//					if(connection_reg & (L1_PE | L3_PE | N_PE | PE_N | PE_L1 | PE_L3))
	//					{
	//						connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
	//						connection_status2 |= CROSSED_AND_SHORTED;
	//					}
	//				}
	//				else if(connection_status2 & L3_PE_CRIS_CROSSED)
	//				{
	//					if(connection_reg & (L2_PE | L1_PE | N_PE | PE_N | PE_L2 | PE_L1))
	//					{
	//						connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
	//						connection_status2 |= CROSSED_AND_SHORTED;
	//					}
	//				}
	//				else if(connection_status2 & N_PE_CRIS_CROSSED)
	//				{
	//					if(connection_reg & (L2_PE | L1_PE | L3_PE | PE_L3 | PE_L2 | PE_L1))
	//					{
	//						connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
	//						connection_status2 |= CROSSED_AND_SHORTED;
	//					}
	//				}
	//				else
	//				{
	//					//ostale L1_PE... dobimo ze v prejsnih postopkih
	//					bool open_flag=false;
	//					bool multi_crossed_flag=false;
	//					switch (connection_reg & PE_CROSSED_MASK2)
	//					{
	//						case PE_L1: connection_status2 |= PE_L1_CROSSED; break;
	//						case PE_L2: connection_status2 |= PE_L2_CROSSED; break;
	//						case PE_L3: connection_status2 |= PE_L3_CROSSED; break;
	//						case PE_N: connection_status2 |= PE_N_CROSSED; break;
	//						case 0: open_flag = true; break;	//vse ok
	//						default: 
	//							multi_crossed_flag = true;
	//					}
	//					switch (connection_reg & PE_CROSSED_MASK)
	//					{
	//						case L1_PE: break;
	//						case L2_PE: break;
	//						case L3_PE: break;
	//						case N_PE: break;
	//						case 0: if(open_flag)connection_status2 |= PE_OPEN; break;	//vse ok
	//						default: 
	//							if(multi_crossed_flag)
	//							{
	//								connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
	//								connection_status2 |= CROSSED_AND_SHORTED;
	//							}
	//					}
	//				}
	//			}
	//			//ce ni crossed in shorted potem pomeni, da je OK
	//			else if((connection_reg & PE_PE)&(!(connection_status & N_SHORTED_MASK)))
	//			{
	//				if(connection_status2 & CROSSED_AND_OPEN)
	//				{
	//					if(!(connection_status & (L1_PE_CROSSED | L2_PE_CROSSED | L3_PE_CROSSED | N_PE_CROSSED)))
	//						connection_status |= PE_OK;
	//				}
	//				else
	//					connection_status |= PE_OK;
	//			}
				cord_cw_count = 99;
				tramsmitt_corect_wiring_state(device.device_dir, device.device_ID);
				break;
			}
			case 99:
			{
				
				
				break;
			}
			case 100:
			{
				break;
			}
			default: break;
		}
	}
	if(cord_cw_count==99)
	{
		if(!(cord_task_control & __CORD_RPE_L_STARTED))
		{	
			//tramsmitt_corect_wiring_state(device.device_dir, device.device_ID);
			cord_task_control &= (~__CORD_CORRECT_WIRING_IN_PROGRESS);
			cord_task_control |= __CORD_CORECT_WIRING_MEASURED;
			if(CORD_AUTO_CONTINUE_MEAS == _ON)
			{	
				if(RISO_TEST_ON_FLAG ==_ON)
				{
					cord_RISO_init();
					set_event(CORD_RISO_PHASES_TO_PE,cord_RISO_phasesToPE);
				}
//			else
//				set_event(STOP_CORD,stop_cord);
			}
//		else
//			set_event(STOP_CORD,stop_cord);
		}
		else
				restart_timer(CORD_MEAS_CORRECT_WIRING,5,cord_meas_correct_wiring);
	}
	else if(cord_cw_count == 100)
	{
		cord_task_control &= (~__CORD_CORRECT_WIRING_IN_PROGRESS);
		tramsmitt_corect_wiring_state(device.device_dir, device.device_ID);
		//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__STOP_RPE__,"","",device.device_dir);
//		set_event(STOP_CORD,stop_cord);
	}
	else 
		restart_timer(CORD_MEAS_CORRECT_WIRING,5,cord_meas_correct_wiring);
}
void cord_RISO_phasesToPE(void)
{
	if(checkAndChangeMeasurement(__CORD))
	{
		switch(cord_RISO_count)
		{
			case 0:
				cord_RISO_init();
				//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__PHASES_TO_PE_STARTED__,"","",device.device_dir);
				cord_task_control |=__CORD_RISO_PHASES_TO_PE_IN_PROGRESS;
				//transmitCommFunc(__RISO,__START,__500V,"N_PE",device.device_dir);
				//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__START_RISO__,"","",device.device_dir);
				cord_RISO_count++;
				break;
			case 1:
//				if(cord_task_control & __CORD_RISO_STARTED)
//				{
					if(CORD_PHASE_NUM_SETTING == _1_PHASE)
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
					cord_task_control |= __CORD_RISO_RES_REQUESTED;
					transmitCommFunc(__RISO,__START,__500V,"PHASES_PE",device.device_dir);
					//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__GET_RISO_RES__,"","PHASES_PE",device.device_dir);
					cord_RISO_count++;
//				}
				break;
			case 2://L1,L2,L3,N proti PE ali L1,N proti PE za enofazin podalsek
				if(!(cord_task_control&__CORD_RISO_RES_REQUESTED))
				{
					
					//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__STOP_RISO__,"","",device.device_dir);
					cord_RISO_count++;
				}
			break;
		}
	}
	if(cord_RISO_count < 3)
		restart_timer(CORD_RISO_PHASES_TO_PE,5,cord_RISO_phasesToPE);
	else
	{
//		if(cord_task_control & __CORD_RISO_STARTED)
//			restart_timer(CORD_RISO_PHASES_TO_PE,5,cord_RISO_phasesToPE);
//		else if(CORD_AUTO_CONTINUE_MEAS==_ON)
//			set_timer(CORD_RISO_ONE_PHASE_TO_PE,5,cord_RISO_onePhaseToPE);
//		if(!(cord_task_control & __CORD_RISO_STARTED))
//		{
		cord_task_control &=(~__CORD_RISO_PHASES_TO_PE_IN_PROGRESS);
		cord_task_control |= __CORD_RISO_PHASES_TO_PE_MEASURED;
		cord_task_control &= ~__CORD_RISO_STARTED;
		//zaenkrat ne vracamo rezultata!!!!
//			if(cord_check_RISO_resistance())
//			{
//				cord_transmittPhasesToPE(true);
//				cord_RISO_count=11;
//			}
//			else
//			{
//				cord_transmittPhasesToPE(false);
//			}
//		}
	}
}

// v naslednjo funkcijo pridemo samo ce je prej ze pomerjena upornost proti PE in je bila ta premajhna
void cord_RISO_onePhaseToPE(void)
{
	if(checkAndChangeMeasurement(__CORD))
	{
		switch (cord_RISO_count)
		{
			case 0:
				cord_RISO_count = 3;
			case 1:
				cord_RISO_count = 3;
			case 2://L1,L2,L3,N proti PE ali L1,N proti PE za enofazin podalsek
				cord_RISO_count = 3;
			case 3://uporablja se od tle naprej!!!!!!!!!	
						cord_task_control |= __CORD_RISO_ONE_PHASE_TO_PE_IN_PROGRESS;
						if(CORD_PHASE_NUM_SETTING == _1_PHASE)
						{
							CON_L1_A;
							CON_N_A;
							CON_PE_B;
							DIS_PE_A;
							DIS_L1_B;
							DIS_L2_B;
							DIS_L3_B;
							DIS_N_B;
							DIS_L2_A;
							DIS_L3_A;
						}	
						else 
						{
							CON_L1_A;
							CON_L2_A;
							CON_L3_A;
							CON_N_A;
							CON_PE_B;
							DIS_PE_A;
							DIS_L1_B;
							DIS_L2_B;
							DIS_L3_B;
							DIS_N_B;
						}
						cord_task_control |= __CORD_RISO_RES_REQUESTED;
						transmitCommFunc(__RISO,__START,__500V,"PHASES_PE",device.device_dir);
						cord_RISO_count++;
				break;
			case 4:	
				if(!(cord_task_control&__CORD_RISO_RES_REQUESTED))
				{
					if(cord_check_RISO_resistance())
					{
						cord_RISO_count=11;
					}
					else
					{
						if(cord_task_control & __CORD_RISO_STARTED)
						{
							if(CORD_PHASE_NUM_SETTING == _1_PHASE)
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
								cord_RISO_count=7;
								cord_task_control |= __CORD_RISO_RES_REQUESTED;
								transmitCommFunc(__RISO,__GET,__RESULT,"N_PE",device.device_dir);
								//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__GET_RISO_RES__,"","N_PE",device.device_dir);
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
								cord_RISO_count++;
								cord_task_control |= __CORD_RISO_RES_REQUESTED;
								transmitCommFunc(__RISO,__GET,__RESULT,"L2-L3-N_PE",device.device_dir);
								//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__GET_RISO_RES__,"","L2-L3-N_PE",device.device_dir);
							}
						}
					}
				}
				break;
			case 5:	//L2,L3,N proti PE
				if(!(cord_task_control&__CORD_RISO_RES_REQUESTED))
				{
					cord_task_control |= __CORD_RISO_STARTED;
					if(cord_check_RISO_resistance())
					{
						cord_RISO_count=11;
						insolation_status |= L1_PE_FAIL;
						DIS_PE_B;
						DIS_N_A;
						CON_L1_A;
						CON_N_B;
						//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__STOP_RISO__,"","",device.device_dir);
					}
					else
					{
						DIS_L2_A;
						cord_RISO_count++;
						cord_task_control |= __CORD_RISO_RES_REQUESTED;
						//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__GET_RISO_RES__,"","L3-N_PE",device.device_dir);
					}
					
				}
				break;
			case 6:	//L3,N proti PE
				if(!(cord_task_control&__CORD_RISO_RES_REQUESTED))
				{
					if(cord_check_RISO_resistance())
					{
						cord_RISO_count=10;
						insolation_status |= L2_PE_FAIL;
						DIS_L3_A;
						DIS_N_A;
						CON_L1_A;
						cord_task_control |= __CORD_RISO_RES_REQUESTED;
						transmitCommFunc(__RISO,__GET,__RESULT,"L1_PE",device.device_dir);
						//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__GET_RISO_RES__,"","L1_PE",device.device_dir);
					}
					else
					{
						DIS_L3_A;
						cord_RISO_count++;
						cord_task_control |= __CORD_RISO_RES_REQUESTED;
						transmitCommFunc(__RISO,__GET,__RESULT,"N_PE",device.device_dir);
						//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__GET_RISO_RES__,"","N_PE",device.device_dir);
					}
					
				}
				break;
			case 7:	//N proti PE
				if(!(cord_task_control&__CORD_RISO_RES_REQUESTED))
				{
					if(CORD_PHASE_NUM_SETTING == _1_PHASE)
					{
						if(cord_check_RISO_resistance())
						{
							cord_RISO_count=11;
							insolation_status |= L1_PE_FAIL;
						}
						else
						{
							insolation_status |= N_PE_FAIL;
							cord_RISO_count=11;
						}
						//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__STOP_RISO__,"","",device.device_dir);
						DIS_N_A;
						DIS_PE_B;
						CON_L1_A;
						CON_N_B;
					}
					else
					{
						if(cord_check_RISO_resistance())
						{
							cord_RISO_count=9;
							insolation_status |= L3_PE_FAIL;
							DIS_N_A;
							CON_L2_A;
							cord_task_control |= __CORD_RISO_RES_REQUESTED;
							transmitCommFunc(__RISO,__GET,__RESULT,"L2_PE",device.device_dir);
							//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__GET_RISO_RES__,"","L2_PE",device.device_dir);
						}
						else
						{
							insolation_status |= N_PE_FAIL;
							DIS_N_A;
							CON_L3_A;
							cord_RISO_count++;
							cord_task_control |= __CORD_RISO_RES_REQUESTED;
							transmitCommFunc(__RISO,__GET,__RESULT,"L3_PE",device.device_dir);
							//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__GET_RISO_RES__,"","L3_PE",device.device_dir);
						}
					}
					
				}
				break;
			case 8:	//L3 proti PE
			if(!(cord_task_control&__CORD_RISO_RES_REQUESTED))
			{
				if(!(cord_check_RISO_resistance()))
					insolation_status |= L3_PE_FAIL;
				cord_RISO_count++;
				DIS_L3_A;
				CON_L2_A;
				cord_task_control |= __CORD_RISO_RES_REQUESTED;
				transmitCommFunc(__RISO,__GET,__RESULT,"L2_PE",device.device_dir);
				//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__GET_RISO_RES__,"","L2_PE",device.device_dir);
			}
			break;
			case 9:	//L2 proti PE
			if(!(cord_task_control&__CORD_RISO_RES_REQUESTED))
			{
				if(!(cord_check_RISO_resistance()))
					insolation_status |= L2_PE_FAIL;
				cord_RISO_count++;
				DIS_L2_A;
				CON_L1_A;
				cord_task_control |= __CORD_RISO_RES_REQUESTED;
				transmitCommFunc(__RISO,__GET,__RESULT,"L1_PE",device.device_dir);
				//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__GET_RISO_RES__,"","L1_PE",device.device_dir);
			}
			break;
			case 10:	//L1 proti PE
			if(!(cord_task_control&__CORD_RISO_RES_REQUESTED))
			{
				if(!(cord_check_RISO_resistance()))
					insolation_status |= L1_PE_FAIL;
				cord_RISO_count++;
				//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__STOP_RISO__,"","",device.device_dir);
			}
			break;
		}
	}
	if(cord_RISO_count <= 10)
		restart_timer(CORD_RISO_ONE_PHASE_TO_PE,5,cord_RISO_onePhaseToPE);
	else
	{
//		//rezultat poslejmo sele ko nam vrne riso stopped
//		if(cord_task_control & __CORD_RISO_STARTED)
//			restart_timer(CORD_RISO_ONE_PHASE_TO_PE,5,cord_RISO_onePhaseToPE);
//		else if(CORD_AUTO_CONTINUE_MEAS ==_ON)
//		{
//			set_timer(CORD_RISO_PHASE_TO_PHASE,5,cord_RISO_phaseToPhase);
//		}
//		if(!(cord_task_control & __CORD_RISO_STARTED))
//		{
		cord_transmittOnePhaseToPE();
		cord_task_control |= __CORD_RISO_ONE_PHASE_TO_PE_MEASURED;
		cord_task_control &= (~__CORD_RISO_ONE_PHASE_TO_PE_IN_PROGRESS);
//		}
	}
}


void cord_RISO_phaseToPhase(void)
{
	if(checkAndChangeMeasurement(__CORD))
	{
		if(!(cord_task_control & __CORD_RISO_PHASE_TO_PHASE_IN_PROGRESS))
		{
			if(cord_RISO_count<=11) cord_RISO_count=10;	//damo count za ena nazaj zato da lahko se enkrat posljemo get resistance
			cord_task_control |= __CORD_RISO_PHASE_TO_PHASE_IN_PROGRESS;
			//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__PHASE_TO_PHASE_STARTED__,"","",device.device_dir);
			//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__START_RISO__,"","",device.device_dir);
		}
			switch(cord_RISO_count)
			{
				case 10:
					if(CORD_PHASE_NUM_SETTING == _1_PHASE)
					{
						DIS_L1_B;
						DIS_L2_B;
						DIS_L3_B;
						DIS_PE_B;
						CON_N_B;
						CON_L1_A;
						DIS_L2_A;
						DIS_L3_A;
						DIS_N_A;
						DIS_PE_A;
						cord_task_control |= __CORD_RISO_RES_REQUESTED;
						transmitCommFunc(__RISO,__START,__500V,"L1_N",device.device_dir);
	//					SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__GET_RISO_RES__,"","L1_N",device.device_dir);
					}
					else
					{
						DIS_L1_B;
						DIS_L2_B;
						DIS_L3_B;
						DIS_PE_B;
						CON_N_B;
						CON_L1_A;
						CON_L2_A;
						CON_L3_A;
						DIS_N_A;
						DIS_PE_A;
						cord_task_control |= __CORD_RISO_RES_REQUESTED;
						transmitCommFunc(__RISO,__START,__500V,"L1_N",device.device_dir);
						//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__GET_RISO_RES__,"","L1-L2-L3_N",device.device_dir);
					}
					cord_RISO_count++;
				break;
				case 11:	//L1,L2,L3 proti N ali L1 proti N
					if(!(cord_task_control&__CORD_RISO_RES_REQUESTED))
					{
						cord_task_control |= __CORD_RISO_STARTED;
						if(CORD_PHASE_NUM_SETTING == _1_PHASE)
						{
							if(!(cord_check_RISO_resistance()))
								insolation_status |= L1_N_FAIL;
							DIS_L1_A;
							DIS_N_B;
							cord_RISO_count=20;
							//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__STOP_RISO__,"","",device.device_dir);
						}
						else
						{
							if(cord_check_RISO_resistance())
							{
								cord_RISO_count=16;
								DIS_L3_A;
								DIS_N_B;
								CON_L3_B;
								cord_task_control |= __CORD_RISO_RES_REQUESTED;
								transmitCommFunc(__RISO,__GET,__RESULT,"L1-L2_L3",device.device_dir);
								//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__GET_RISO_RES__,"","L1-L2_L3",device.device_dir);
							}
							else
							{
								DIS_L1_A;
								cord_RISO_count++;
								cord_task_control |= __CORD_RISO_RES_REQUESTED;
								transmitCommFunc(__RISO,__GET,__RESULT,"L2-L3_N",device.device_dir);
								//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__GET_RISO_RES__,"","L2-L3_N",device.device_dir);
							}

						}
					}
					break;
					case 12:	//L2,L3 proti N
					if(!(cord_task_control&__CORD_RISO_RES_REQUESTED))
					{
						if(cord_check_RISO_resistance())
						{
							insolation_status |= L1_N_FAIL;
							cord_RISO_count=16;
							DIS_L3_A;
							DIS_N_B;
							CON_L3_B;
							CON_L1_A;
							cord_task_control |= __CORD_RISO_RES_REQUESTED;
							transmitCommFunc(__RISO,__GET,__RESULT,"L1-L2_L3",device.device_dir);
							//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__GET_RISO_RES__,"","L1-L2_L3",device.device_dir);
						}
						else
						{
							DIS_L2_A;
							cord_RISO_count++;
							cord_task_control |= __CORD_RISO_RES_REQUESTED;
							transmitCommFunc(__RISO,__GET,__RESULT,"L3_N",device.device_dir);
							//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__GET_RISO_RES__,"","L3_N",device.device_dir);
						}
					}
					break;
					case 13:	//L3 proti N
					if(!(cord_task_control&__CORD_RISO_RES_REQUESTED))
					{
						if(cord_check_RISO_resistance())
						{
							insolation_status |= L2_N_FAIL;
							cord_RISO_count=15;
							DIS_L3_A;
							CON_L1_A;
							cord_task_control |= __CORD_RISO_RES_REQUESTED;
							transmitCommFunc(__RISO,__GET,__RESULT,"L1_N",device.device_dir);
							//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__GET_RISO_RES__,"","L1_N",device.device_dir);
						}
						else
						{
							insolation_status |= L3_N_FAIL;
							DIS_L1_A;
							cord_RISO_count++;
							cord_task_control |= __CORD_RISO_RES_REQUESTED;
							transmitCommFunc(__RISO,__GET,__RESULT,"L2_N",device.device_dir);
							//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__GET_RISO_RES__,"","L2_N",device.device_dir);
						}
					}
					break;
					case 14:	//L2 proti N
					if(!(cord_task_control&__CORD_RISO_RES_REQUESTED))
					{
						if(!(cord_check_RISO_resistance()))
							insolation_status |= L2_N_FAIL;
						cord_RISO_count++;
						DIS_L2_A;
						CON_L1_A;
						cord_task_control |= __CORD_RISO_RES_REQUESTED;
						transmitCommFunc(__RISO,__GET,__RESULT,"L1_N",device.device_dir);
						//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__GET_RISO_RES__,"","L1_N",device.device_dir);
					}
					break;
					case 15:	//L1 proti N
					if(!(cord_task_control&__CORD_RISO_RES_REQUESTED))
					{
						if(!(cord_check_RISO_resistance()))
							insolation_status |= L1_N_FAIL;
						cord_RISO_count++;
						DIS_N_B;
						CON_L2_A;
						CON_L3_B;
						cord_task_control |= __CORD_RISO_RES_REQUESTED;
						transmitCommFunc(__RISO,__GET,__RESULT,"L1-L2_L3",device.device_dir);
						//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__GET_RISO_RES__,"","L1-L2_L3",device.device_dir);
					}
					break;
					case 16:	//L1,L2 proti L3
					if(!(cord_task_control&__CORD_RISO_RES_REQUESTED))
					{
						if(cord_check_RISO_resistance())
						{
							cord_RISO_count=19;
							DIS_L2_A;
							DIS_L3_B;
							CON_L2_B;
							cord_task_control |= __CORD_RISO_RES_REQUESTED;
							transmitCommFunc(__RISO,__GET,__RESULT,"L1_L2",device.device_dir);
							//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__GET_RISO_RES__,"","L1_L2",device.device_dir);
						}
						else
						{
							DIS_L1_A;
							cord_RISO_count++;
							cord_task_control |= __CORD_RISO_RES_REQUESTED;
							transmitCommFunc(__RISO,__GET,__RESULT,"L2_L3",device.device_dir);
							//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__GET_RISO_RES__,"","L2_L3",device.device_dir);
						}
					}
					break;
					case 17:	//L2 proti L3
					if(!(cord_task_control&__CORD_RISO_RES_REQUESTED))
					{
						if(cord_check_RISO_resistance())
						{
							insolation_status |= L1_L3_FAIL;
							cord_RISO_count=19;
							DIS_L2_A;
							DIS_L3_B;
							CON_L1_A;
							CON_L2_B;
							cord_task_control |= __CORD_RISO_RES_REQUESTED;
							transmitCommFunc(__RISO,__GET,__RESULT,"L1_L2",device.device_dir);
							//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__GET_RISO_RES__,"","L1_L2",device.device_dir);
						}
						else
						{
							insolation_status |= L2_L3_FAIL;
							DIS_L2_A;
							CON_L1_A;
							cord_RISO_count++;
							cord_task_control |= __CORD_RISO_RES_REQUESTED;
							transmitCommFunc(__RISO,__GET,__RESULT,"L1_L3",device.device_dir);
							//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__GET_RISO_RES__,"","L1_L3",device.device_dir);
						}		
					}
					break;
					case 18:	//L1 proti L3
					if(!(cord_task_control&__CORD_RISO_RES_REQUESTED))
					{
						if(!(cord_check_RISO_resistance()))
							insolation_status |= L1_L3_FAIL;
						cord_RISO_count++;
						DIS_L3_B;
						CON_L2_B;
						cord_task_control |= __CORD_RISO_RES_REQUESTED;
						transmitCommFunc(__RISO,__GET,__RESULT,"L1_L2",device.device_dir);
						//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__GET_RISO_RES__,"","L1_L2",device.device_dir);
					}
					break;
					case 19:	//L1 proti L2
					if(!(cord_task_control&__CORD_RISO_RES_REQUESTED))
					{
						if(!(cord_check_RISO_resistance()))
							insolation_status |= L1_L2_FAIL;
						cord_RISO_count++;
						DIS_L2_B;
						DIS_L1_A;
						//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__STOP_RISO__,"","",device.device_dir);
					}
					break;
					default: break;
			}
		
	}
	if(cord_RISO_count <= 19)
		restart_timer(CORD_RISO_PHASE_TO_PHASE,5,cord_RISO_phaseToPhase);
	else
	{
//		if(cord_task_control & __CORD_RISO_STARTED)
//			restart_timer(CORD_RISO_PHASE_TO_PHASE,5,cord_RISO_phaseToPhase);
//		else
//		{
		cord_transmittPhaseToPhase();
		cord_task_control |= __CORD_RISO_PHASE_TO_PHASE_MEASURED;
		cord_task_control &= (~__CORD_RISO_PHASE_TO_PHASE_IN_PROGRESS);
//		}
	}
	
}
void cord_transmittPhaseToPhase(void)
{
	char temp_str[MAX_ADDITIONAL_COMMANDS_LENGTH];
	//sprintf(temp_str,__RISO_PHASE_TO_PHASE__);
	cord_task_control &= ~__CORD_RISO_STARTED;
	if(insolation_status & CORD_PHASE_TO_PHASE_MASK)
	{
		//strcat(temp_str,"|");
		sprintf(temp_str,__FAIL__);
		if(insolation_status & L1_N_FAIL)
		{
			strcat(temp_str,",");
			strcat(temp_str,__L1_N_FAIL__);
		}
		if(insolation_status & L2_N_FAIL)
		{
			strcat(temp_str,",");
			strcat(temp_str,__L2_N_FAIL__);
		}
		if(insolation_status & L3_N_FAIL)
		{
			strcat(temp_str,",");
			strcat(temp_str,__L3_N_FAIL__);
		}
		if(insolation_status & L1_L3_FAIL)
		{
			strcat(temp_str,",");
			strcat(temp_str,__L1_L3_FAIL__);
		}
		if(insolation_status & L1_L2_FAIL)
		{
			strcat(temp_str,",");
			strcat(temp_str,__L1_L2_FAIL__);
		}
		if(insolation_status & L2_L3_FAIL)
		{
			strcat(temp_str,",");
			strcat(temp_str,__L2_L3_FAIL__);
		}
	}
	else
	{
		//strcat(temp_str,"|");
		sprintf(temp_str,__PASS__);
	}
	transmitFunc(__PH_PH, __RESULT ,temp_str,"", device.device_dir);
	//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__RISO_PHASE_TO_PHASE__,temp_str,"",device.device_dir);
}

void cord_transmittPhasesToPE(bool pass)
{
	char temp_str[MAX_ADDITIONAL_COMMANDS_LENGTH];
	cord_task_control &= ~__CORD_RISO_STARTED;
	if(pass)
	{
		sprintf(temp_str,__PASS__);
	}
	else
	{
		sprintf(temp_str,__FAIL__);
	}
	transmitFunc(__ALL_PE, __RESULT ,temp_str,"", device.device_dir);
	//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__RISO_PHASES_TO_PE__,temp_str,"",device.device_dir);
}
void cord_transmittOnePhaseToPE(void)
{
	char temp_str[MAX_ADDITIONAL_COMMANDS_LENGTH];
	cord_task_control &= ~__CORD_RISO_STARTED;
	if(insolation_status & CORD_ONE_PHASE_TO_PE_MASK)
	{
		sprintf(temp_str,__FAIL__);
		if(insolation_status & L1_PE_FAIL)
		{
			strcat(temp_str,",");
			strcat(temp_str,__L1_PE_FAIL__);
		}
		if(insolation_status & L2_PE_FAIL)
		{
			strcat(temp_str,",");
			strcat(temp_str,__L2_PE_FAIL__);
		}
		if(insolation_status & L3_PE_FAIL)
		{
			strcat(temp_str,",");
			strcat(temp_str,__L3_PE_FAIL__);
		}
		if(insolation_status & N_PE_FAIL)
		{
			strcat(temp_str,",");
			strcat(temp_str,__N_PE_FAIL__);
		}
	}
	else
	{
		sprintf(temp_str,__PASS__);
	}
	transmitFunc(__ONE_PE, __RESULT ,temp_str,"", device.device_dir);
	//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__RISO_ONE_PHASE_TO_PE__,temp_str,"",device.device_dir);
}
void cord_continuity_test(void)
{
	if(checkAndChangeMeasurement(__CORD))
	{
		switch(cord_continuity_count)
		{
			case 0:
				cord_continuity_init();
				cord_continuity_count++;
				//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__CONTINUITY_STARTED__,"","",device.device_dir);
				cord_task_control |= __CORD_CONTINUITY_IN_PROGRESS;
				CON_L1_A;
				CON_L1_B;
				cord_task_control |= __CORD_RPE_RES_REQUESTED;
				transmitCommFunc(__RPE,__START,__MID,"L1_L1",device.device_dir);
				cord_task_control |= __CORD_RPE_M_STARTED;
				//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__START_RPE_HIGH__,"","",device.device_dir);
				break;
//			case 1:
//				if(cord_task_control & __CORD_RPE_H_STARTED)
//				{
//					cord_task_control |= __CORD_RPE_RES_REQUESTED;
//					transmitCommFunc(__RPE,__GET,__RESULT,"L1_L1",device.device_dir);
//					cord_continuity_count++;
//				}
//				break;
			case 1://L1_L1
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					cord_task_control &= ~__CORD_RPE_M_STARTED;
					cord_continuity_L1_L1_res = cord_rpe_resistance;
					
					cord_task_control |= __CORD_RPE_RES_REQUESTED;
					if(CORD_PHASE_NUM_SETTING == _1_PHASE)
					{
						DIS_L1_A;
						DIS_L1_B;
						CON_N_A;
						CON_N_B;
						transmitCommFunc(__RPE,__START,__MID,"N_N",device.device_dir);
						cord_continuity_count=4;//na N_N
					}
					else
					{
						DIS_L1_A;
						DIS_L1_B;
						CON_L2_A;
						CON_L2_B;
						transmitCommFunc(__RPE,__START,__MID,"L2_L2",device.device_dir);
						cord_continuity_count++;
					}
					cord_task_control |= __CORD_RPE_M_STARTED;
					//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__STOP_RPE__,"","",device.device_dir);
					//cord_continuity_count++;
				}
				break;
			case 2://L2_L2
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					cord_task_control &= ~__CORD_RPE_M_STARTED;
					DIS_L2_A;
					DIS_L2_B;
					CON_L3_A;
					CON_L3_B;
					cord_continuity_L2_L2_res = cord_rpe_resistance;
					cord_task_control |= __CORD_RPE_RES_REQUESTED;
					transmitCommFunc(__RPE,__START,__MID,"L3_L3",device.device_dir);
					//transmitCommFunc(__RPE,__GET,__RESULT,"L3_L3",device.device_dir);
					cord_continuity_count++;
					//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__START_RPE_HIGH__,"","",device.device_dir);
					cord_task_control |= __CORD_RPE_M_STARTED;
					cord_continuity_count++;
				}
				break;
			case 3://L3_L3
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					DIS_L3_A;
					DIS_L3_B;
					CON_N_A;
					CON_N_B;
					cord_task_control &= ~__CORD_RPE_M_STARTED;
					cord_continuity_L3_L3_res = cord_rpe_resistance;
					cord_task_control |= __CORD_RPE_RES_REQUESTED;
					transmitCommFunc(__RPE,__START,__MID,"N_N",device.device_dir);
					//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__STOP_RPE__,"","",device.device_dir);
					cord_continuity_count++;
					cord_task_control |= __CORD_RPE_M_STARTED;
				}
				break;
			case 4://N_N
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					DIS_N_A;
					DIS_N_B;
					CON_PE_A;
					CON_PE_B;
					cord_task_control &= ~__CORD_RPE_M_STARTED;
					cord_continuity_N_N_res = cord_rpe_resistance;
					cord_task_control |= __CORD_RPE_RES_REQUESTED;
					transmitCommFunc(__RPE,__START,__MID,"PE_PE",device.device_dir);
					//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__STOP_RPE__,"","",device.device_dir);
					cord_continuity_count++;
					cord_task_control |= __CORD_RPE_M_STARTED;
				}
				break;
			case 5://PE_PE
				if(!(cord_task_control & __CORD_RPE_RES_REQUESTED))
				{
					DIS_PE_A;
					DIS_PE_B;
					cord_task_control &= ~__CORD_RPE_M_STARTED;
					cord_continuity_PE_PE_res = cord_rpe_resistance;
					//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__STOP_RPE__,"","",device.device_dir);
					cord_continuity_count++;
				}
				break;
		}
	}
	if(cord_continuity_count < 6)
		restart_timer(CORD_MEAS_CONTINUITY,5,cord_continuity_test);
	else
	{
		cord_task_control &= (~__CORD_CONTINUITY_IN_PROGRESS);
		cord_task_control |= __CORD_CONTINUITY_MEASURED;
		if(CORD_PHASE_NUM_SETTING == _1_PHASE)
		{
			if((cord_continuity_L1_L1_res > RpeContinuityLimit)||(cord_continuity_N_N_res > RpeContinuityLimit)||(cord_continuity_PE_PE_res > RpeContinuityLimit))
				cord_transmittContiunuity(false);
			else
				cord_transmittContiunuity(true);
		}
		else
		{
			if((cord_continuity_L1_L1_res > RpeContinuityLimit)||(cord_continuity_L2_L2_res > RpeContinuityLimit)||(cord_continuity_L3_L3_res > RpeContinuityLimit)||(cord_continuity_N_N_res > RpeContinuityLimit)||(cord_continuity_PE_PE_res > RpeContinuityLimit))
				cord_transmittContiunuity(false);
			else
				cord_transmittContiunuity(true);
		}
	}	
}
void cord_transmittContiunuity(bool pass)
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
	if(CORD_PHASE_NUM_SETTING == _1_PHASE)
		snprintf(&temp_str[strlen(temp_str)],(MAX_ADDITIONAL_COMMANDS_LENGTH-strlen(temp_str)),",%s|%.3f,%s|%.3f,%s|%.3f",__CORD_L1__,cord_continuity_L1_L1_res,__CORD_N__,cord_continuity_N_N_res,__CORD_PE__,cord_continuity_PE_PE_res);
	else
		snprintf(&temp_str[strlen(temp_str)],(MAX_ADDITIONAL_COMMANDS_LENGTH-strlen(temp_str)),",%s|%.3f,%s|%.3f,%s|%.3f,%s|%.3f,%s|%.3f",__CORD_L1__,cord_continuity_L1_L1_res,__CORD_L2__,cord_continuity_L2_L2_res,__CORD_L3__,cord_continuity_L3_L3_res,__CORD_N__,cord_continuity_N_N_res,__CORD_PE__,cord_continuity_PE_PE_res);
	transmitFunc(__CONT, __RESULT ,temp_str,"", device.device_dir);
	//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__CORD_CONTINUITY__,temp_str,"",device.device_dir);
}

void stop_cord(void)
{
	meas_task_control &= (~__CORD_MEAS_IN_PROG);
	cord_global_init_set();
	cord_task_control |= __CORD_INITIATED;
//	//SET_PE_CONTACTOR;
//	RST_L1_CONTACTOR;
//	RST_L2_CONTACTOR;
//	RST_L3_CONTACTOR;
//	RST_N_CONTACTOR;
//	rst_REL(10);
//	rst_REL(11);
//	rst_REL(12);
//	rst_REL(13);
//	//rst_REL(9);
//	rst_REL(27);
//	rst_REL(28);
//	rst_REL(30);
//	rst_REL(33);
//	rst_REL(34);
//	rst_REL(35);
//	rst_REL(36);
	end_task(CORD_MEAS_CORRECT_WIRING);
	end_task(INIT_CORD);
	end_task(CORD_RISO_PHASES_TO_PE);
	end_task(CORD_RISO_ONE_PHASE_TO_PE);
	end_task(CORD_RISO_PHASE_TO_PHASE);
	end_task(CORD_MEAS_CONTINUITY);
	cord_RISO_count=0;
	cord_cw_count=0;
	cord_RISO_count=0;
	cord_continuity_count=0;
	start_cord_count=0;
	cord_err_count=0;
	transmitEvent(__STOPPED_EN,__CORD_EN,"",device.device_dir);
	//SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__STOPED_C__,"","",device.device_dir);
}
//void stop_cord_and_transmitt(uint32_t dir, char device_ID)
//{
//	stop_cord();
//	SendComMessage(_ON,_ID_TFA,device_ID,__MT_300__,__CORD__,__STOPED_C__,"",dir);
//}

void cord_global_init_set(void)
{
	connection_reg =0;
	connection_status=0;
	connection_status2 =0;
	insolation_status=0;
	cord_task_control &= (~CORD_MEAS_MASKS);
	cord_task_control |= __CORD_INITIATED;
	if(connection_control & __CON_TO_MT310)
	{
		RST_L1_CONTACTOR;
		RST_L2_CONTACTOR;
		RST_L3_CONTACTOR;
		RST_N_CONTACTOR;
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
		RST_PE_CONTACTOR;//SET_PE_CONTACTOR;
		set_REL(9);
		set_REL(27);
		set_REL(28);
	}
}
void cord_correct_wiring_init(void)
{
	connection_reg =0;
	connection_status=0;
	connection_status2 =0;
	cord_cw_count=0;
	cord_task_control &= ~(__CORD_RPE_RES_REQUESTED|__CORD_CORECT_WIRING_MEASURED);
	if(connection_control & __CON_TO_MT310)
	{
		RST_L1_CONTACTOR;
		RST_L2_CONTACTOR;
		RST_L3_CONTACTOR;
		RST_N_CONTACTOR;
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
		RST_PE_CONTACTOR;//SET_PE_CONTACTOR;
		set_REL(9);
		set_REL(27);
		set_REL(28);
	}
}
void cord_RISO_init(void)
{
	cord_RISO_count=0;
	insolation_status=0;
	cord_task_control &= (~CORD_RISO_MASKS);
	if(connection_control & __CON_TO_MT310)
	{
		RST_L1_CONTACTOR;
		RST_L2_CONTACTOR;
		RST_L3_CONTACTOR;
		RST_N_CONTACTOR;
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
		RST_PE_CONTACTOR;//SET_PE_CONTACTOR;
		set_REL(9);
		set_REL(27);
		set_REL(28);
	}
	
}
void cord_continuity_init(void)
{
	cord_continuity_count=0;
	cord_continuity_L1_L1_res=0;
  cord_continuity_L2_L2_res=0;
  cord_continuity_L3_L3_res=0;
  cord_continuity_N_N_res=0;
  cord_continuity_PE_PE_res=0;
	if(connection_control & __CON_TO_MT310)
	{
		RST_L1_CONTACTOR;
		RST_L2_CONTACTOR;
		RST_L3_CONTACTOR;
		RST_N_CONTACTOR;
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
		RST_PE_CONTACTOR;//SET_PE_CONTACTOR;
		set_REL(9);
		set_REL(27);
		set_REL(28);
	}
}
void set_RPE_cord_resistance(char* value)
{
	cord_rpe_resistance = (float)atof(value);
	cord_task_control &= (~__CORD_RPE_RES_REQUESTED);
}
void set_RISO_cord_resistance(char* value)
{
	cord_riso_resistance = (float)atof(value);
	cord_task_control &= (~__CORD_RISO_RES_REQUESTED);
}
bool cord_check_RPE_resistance(void)
{
	if(cord_rpe_resistance > CordRpeLimit)
		return false;
	else
		return true;
}
bool cord_check_RISO_resistance(void)
{
	if(cord_riso_resistance < CordRisoLimit)
		return false;
	else
		return true;
}
void tramsmitt_corect_wiring_state(uint32_t dir, char device_ID)
{
	uint32_t msg_count=0;
	char temp_str[MAX_ADDITIONAL_COMMANDS_LENGTH];
	sprintf(temp_str,__FAIL__);//na zacetku napisemo fail in kasneje prepisemo ce je pass
	strcat(temp_str,",");
	cord_task_control &= ~__CORD_RPE_L_STARTED;
		//+++++++++++++++++VRNEMO REZULTAT ZA ENOFAZNI KABEL+++++++++++++++++++++
	#if AUTO_DETECT_P_NUM == true
	if(((!(connection_status & (~(CABLE_1P_MASK|L2_OPEN|L3_OPEN))))&&(!(manual_set_phase_num)))||(manual_set_phase_num && (CORD_PHASE_NUM_SETTING == _1_PHASE)))//pogledamo ce gre za enofazni kabel
	#else
	if(manual_set_phase_num && (CORD_PHASE_NUM_SETTING == _1_PHASE))
	#endif
	{
		if(((numberOfSetBits(connection_status & CABLE_1P_FAULT_MASK_S1))+(numberOfSetBits(connection_status2 & CABLE_1P_FAULT_MASK_S2)))>2)
		{
			if((numberOfSetBits(connection_status & CABLE_1P_CROSSED_LOW_MASKS)>1)&&(numberOfSetBits(connection_status2 & CABLE_1P_CROSSED_HIGH_MASKS)>1))
			{
				connection_status2 |= MULTI_FAULT;
				connection_status2 |= MULTI_CROSSED;
			}
			if(numberOfSetBits(connection_status & STATUS_FAULT_1P_SHORTED_MASK)>1)
			{
				connection_status2 |= MULTI_FAULT;
				connection_status2 |= MULTI_SHORTED;
			}
			if(numberOfSetBits(connection_status & (L1_OPEN|N_OPEN|PE_OPEN))>1)
			{
				connection_status2 |= MULTI_FAULT;
				connection_status2 |= MULTI_OPENED;
			}
		}
		if(connection_status2 & MULTI_FAULT)
		{
			if(MULTI_FAULT_CONTINUE)
			{
				strcat(temp_str,__MULTIFAULT__);
				if(connection_status2 & CROSSED_AND_OPEN)
				{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__CROSSED_AND_OPEN__);
					}
				}
				if(connection_status2 & SHORTED_AND_OPEN)
				{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__SHORTED_AND_OPEN__);
					}
				}
				if(connection_status2 & MULTI_SHORTED)
				{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__MULTI_SHORTED__);
					}
				}
				if(connection_status2 & MULTI_CROSSED)
				{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__MULTI_CROSSED__);
					}
				}
				if(connection_status2 & MULTI_OPENED)
				{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__MULTI_OPEN__);
					}
				}
				if(connection_status2 & CROSSED_AND_SHORTED)
				{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__CROSSED_AND_SHORTED__);
					}
				}
			}
			else
			{
				msg_count++;
				strcat(temp_str,__MULTIFAULT__);
				if(connection_status2 & CROSSED_AND_OPEN)
				{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__CROSSED_AND_OPEN__);
					}
				}
				if(connection_status2 & SHORTED_AND_OPEN)
				{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__SHORTED_AND_OPEN__);
					}
				}
				if(connection_status2 & MULTI_SHORTED)
				{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__MULTI_SHORTED__);
					}
				}
				if(connection_status2 & MULTI_CROSSED)
				{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__MULTI_CROSSED__);
					}
				}
				if(connection_status2 & MULTI_OPENED)
				{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__MULTI_OPEN__);
					}
				}
				if(connection_status2 & CROSSED_AND_SHORTED)
				{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__CROSSED_AND_SHORTED__);
					}
				}
				if(connection_status & L1_OPEN)
				{
						msg_count++;
						if(msg_count<MAX_ERROR_ADD_COMMAND)
						{
							strcat(temp_str,",");
							strcat(temp_str,__L1_OPEN__);
						}
				}
				if(connection_status2 & L1_N_CRIS_CROSSED)
				{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L1_N_CRIS_CROSSED__);
					}
				}
				else
				{
					if(connection_status & L1_N_CROSSED)
					{
							msg_count++;
							if(msg_count<MAX_ERROR_ADD_COMMAND)
							{
								strcat(temp_str,",");
								strcat(temp_str,__L1_N_CROSSED__);
							}
					}
					if(connection_status2 & N_L1_CROSSED)
					{
							msg_count++;
							if(msg_count<MAX_ERROR_ADD_COMMAND)
							{
								strcat(temp_str,",");
								strcat(temp_str,__N_L1_CROSSED__);
							}
					}
				}
				if(connection_status & L1_N_SHORTED)
				{
						msg_count++;
						if(msg_count<MAX_ERROR_ADD_COMMAND)
						{
							strcat(temp_str,",");
							strcat(temp_str,__L1_N_SHORTED__);
						}
				}
				if(connection_status2 & L1_PE_CRIS_CROSSED)
				{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L1_PE_CRIS_CROSSED__);
					}
				}
				else
				{
					if(connection_status & L1_PE_CROSSED)
					{
							msg_count++;
							if(msg_count<MAX_ERROR_ADD_COMMAND)
							{
								strcat(temp_str,",");
								strcat(temp_str,__L1_PE_CROSSED__);
							}
					}
					if(connection_status2 & PE_L1_CROSSED)
					{
							msg_count++;
							if(msg_count<MAX_ERROR_ADD_COMMAND)
							{
								strcat(temp_str,",");
								strcat(temp_str,__PE_L1_CROSSED__);
							}
					}
				}
				if(connection_status & L1_PE_SHORTED)
				{
						msg_count++;
						if(msg_count<MAX_ERROR_ADD_COMMAND)
						{
							strcat(temp_str,",");
							strcat(temp_str,__L1_PE_SHORTED__);
						}
				}
				if(connection_status & N_OPEN)
				{
						msg_count++;
						if(msg_count<MAX_ERROR_ADD_COMMAND)
						{
							strcat(temp_str,",");
							strcat(temp_str,__N_OPEN__);
						}
				}
				if(connection_status & PE_OPEN)
				{
						msg_count++;
						if(msg_count<MAX_ERROR_ADD_COMMAND)
						{
							strcat(temp_str,",");
							strcat(temp_str,__PE_OPEN__);
						}
				}
				if(connection_status2 & N_PE_CRIS_CROSSED)
				{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__N_PE_CRIS_CROSSED__);
					}
				}
				else
				{
					if(connection_status & N_PE_CROSSED)
					{
							msg_count++;
							if(msg_count<MAX_ERROR_ADD_COMMAND)
							{
								strcat(temp_str,",");
								strcat(temp_str,__N_PE_CROSSED__);
							}
					}
					if(connection_status2 & PE_N_CROSSED)
					{
							msg_count++;
							if(msg_count<MAX_ERROR_ADD_COMMAND)
							{
								strcat(temp_str,",");
								strcat(temp_str,__PE_N_CROSSED__);
							}
					}
				}
				if(connection_status & N_PE_SHORTED)
				{
						msg_count++;
						if(msg_count<MAX_ERROR_ADD_COMMAND)
						{
							strcat(temp_str,",");
							strcat(temp_str,__N_PE_SHORTED__);
						}
				}
			}
		}
		else
		{
			switch (connection_status2 & STATUS2_FAULT_1P_CROSSED_MASK)
			{
				case L1_N_CRIS_CROSSED:
					strcat(temp_str,__L1_N_CRIS_CROSSED__);
					break;
				case L1_PE_CRIS_CROSSED:
					strcat(temp_str,__L1_PE_CRIS_CROSSED__);
					break;
				case N_PE_CRIS_CROSSED:
					strcat(temp_str,__N_PE_CRIS_CROSSED__);
					break;
				default:
					switch (connection_status & STATUS_FAULT_1P_SHORTED_MASK)
					{
						case L1_N_SHORTED:
							strcat(temp_str,__L1_N_SHORTED__);
							break;
						case L1_PE_SHORTED:
							strcat(temp_str,__L1_PE_SHORTED__);
							break;
						case N_PE_SHORTED:
							strcat(temp_str,__N_PE_SHORTED__);
							break;
						default:
							switch (connection_status & STATUS_FAULT_1P_OPEN_MASK)
							{
								case L1_OPEN:
									strcat(temp_str,__L1_OPEN__);
									break;
								case N_OPEN:
									strcat(temp_str,__N_OPEN__);
									break;
								case PE_OPEN:
									strcat(temp_str,__PE_OPEN__);
									break;
								default:
									if(connection_status & STATUS_OK_1P_MASK)
									{
										temp_str[0]='M';
										temp_str[1]='A';
										temp_str[2]='I';
										temp_str[3]='N';
										temp_str[4]='_';
										temp_str[5]='R';
										temp_str[6]='E';
										temp_str[7]='S';
										temp_str[8]='|';
										temp_str[9]='P';
										temp_str[10]='A';
										temp_str[11]='S';
										temp_str[12]='S';
										//strcat(temp_str,__PASS__);	//ce ni nic od prej nastetega potem je to pass
									}
									else
									{
//										temp_str[0]='F';
//										temp_str[1]='A';
//										temp_str[2]='I';
//										temp_str[3]='L';
										//strcat(temp_str,__FAIL__);
									}
									break;
							}
							break;
					}
					break;
			}
		}
		if(AUTO_DETECT_P_NUM&&(!(manual_set_phase_num)))
			transmitFunc(__C_W, __RESULT ,temp_str,"", device.device_dir);
			//SendComMessage(_ON,_ID_TFA,device_ID,__CORD__,__CORD_CW_RESULT__,temp_str,__1_PHASE__,dir);
		else
			transmitFunc(__C_W, __RESULT ,temp_str,"", device.device_dir);
			//SendComMessage(_ON,_ID_TFA,device_ID,__CORD__,__CORD_CW_RESULT__,temp_str,"",dir);
	}
	//+++++++++++++++++VRNEMO REZULTAT ZA TROFAZNI KABEL+++++++++++++++++++++
	else
	{
		if(((numberOfSetBits(connection_status & CABLE_3P_FAULT_MASK_S1))+(numberOfSetBits(connection_status2 & CABLE_3P_FAULT_MASK_S2)))>2)
		{
			if((numberOfSetBits(connection_status & CABLE_3P_CROSSED_LOW_MASKS)>1)&&(numberOfSetBits(connection_status2 & CABLE_3P_CROSSED_HIGH_MASKS)>1))
			{
				connection_status2 |= MULTI_FAULT;
				connection_status2 |= MULTI_CROSSED;
			}
			if(numberOfSetBits(connection_status & ALL_SHORTED_MASKS)>1)
			{
				connection_status2 |= MULTI_FAULT;
				connection_status2 |= MULTI_SHORTED;
			}
			if(numberOfSetBits(connection_status & (L1_OPEN|L2_OPEN|L3_OPEN|N_OPEN|PE_OPEN))>1)
			{
				connection_status2 |= MULTI_FAULT;
				connection_status2 |= MULTI_OPENED;
			}
		}
		if(connection_status2 & MULTI_FAULT)
		{
			msg_count++;
			strcat(temp_str,__MULTIFAULT__);
			if(connection_status2 & CROSSED_AND_OPEN)
			{
				msg_count++;
				if(msg_count<MAX_ERROR_ADD_COMMAND)
				{
					strcat(temp_str,",");
					strcat(temp_str,__CROSSED_AND_OPEN__);
				}
			}
			if(connection_status2 & SHORTED_AND_OPEN)
			{
				msg_count++;
				if(msg_count<MAX_ERROR_ADD_COMMAND)
				{
					strcat(temp_str,",");
					strcat(temp_str,__SHORTED_AND_OPEN__);
				}
			}
			if(connection_status2 & MULTI_SHORTED)
			{
				msg_count++;
				if(msg_count<MAX_ERROR_ADD_COMMAND)
				{
					strcat(temp_str,",");
					strcat(temp_str,__MULTI_SHORTED__);
				}
			}
			if(connection_status2 & MULTI_CROSSED)
			{
				msg_count++;
				if(msg_count<MAX_ERROR_ADD_COMMAND)
				{
					strcat(temp_str,",");
					strcat(temp_str,__MULTI_CROSSED__);
				}
			}
			if(connection_status2 & MULTI_OPENED)
			{
				msg_count++;
				if(msg_count<MAX_ERROR_ADD_COMMAND)
				{
					strcat(temp_str,",");
					strcat(temp_str,__MULTI_OPEN__);
				}
			}
			if(connection_status2 & CROSSED_AND_SHORTED)
			{
				msg_count++;
				if(msg_count<MAX_ERROR_ADD_COMMAND)
				{
					strcat(temp_str,",");
					strcat(temp_str,__CROSSED_AND_SHORTED__);
				}
			}
			if(connection_status & L1_OPEN)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L1_OPEN__);
					}
			}
			if(connection_status2 & L1_L2_CRIS_CROSSED)
			{
				msg_count++;
				if(msg_count<MAX_ERROR_ADD_COMMAND)
				{
					strcat(temp_str,",");
					strcat(temp_str,__L1_L2_CRIS_CROSSED__);
				}
			}
			else
			{
				if(connection_status & L1_L2_CROSSED)
				{
						msg_count++;
						if(msg_count<MAX_ERROR_ADD_COMMAND)
						{
							strcat(temp_str,",");
							strcat(temp_str,__L1_L2_CROSSED__);
						}
				}
				if(connection_status2 & L2_L1_CROSSED)
				{
						msg_count++;
						if(msg_count<MAX_ERROR_ADD_COMMAND)
						{
							strcat(temp_str,",");
							strcat(temp_str,__L2_L1_CROSSED__);
						}
				}
			}
			if(connection_status & L1_L2_SHORTED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L1_L2_SHORTED__);
					}
			}
			if(connection_status2 & L1_L3_CRIS_CROSSED)
			{
				msg_count++;
				if(msg_count<MAX_ERROR_ADD_COMMAND)
				{
					strcat(temp_str,",");
					strcat(temp_str,__L1_L3_CRIS_CROSSED__);
				}
			}
			else
			{
				if(connection_status & L1_L3_CROSSED)
				{
						msg_count++;
						if(msg_count<MAX_ERROR_ADD_COMMAND)
						{
							strcat(temp_str,",");
							strcat(temp_str,__L1_L3_CROSSED__);
						}
				}
				if(connection_status2 & L3_L1_CROSSED)
				{
						msg_count++;
						if(msg_count<MAX_ERROR_ADD_COMMAND)
						{
							strcat(temp_str,",");
							strcat(temp_str,__L3_L1_CROSSED__);
						}
				}
			}
			if(connection_status & L1_L3_SHORTED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L1_L3_SHORTED__);
					}
			}
			if(connection_status2 & L1_N_CRIS_CROSSED)
			{
				msg_count++;
				if(msg_count<MAX_ERROR_ADD_COMMAND)
				{
					strcat(temp_str,",");
					strcat(temp_str,__L1_N_CRIS_CROSSED__);
				}
			}
			else
			{
				if(connection_status & L1_N_CROSSED)
				{
						msg_count++;
						if(msg_count<MAX_ERROR_ADD_COMMAND)
						{
							strcat(temp_str,",");
							strcat(temp_str,__L1_N_CROSSED__);
						}
				}
				if(connection_status2 & N_L1_CROSSED)
				{
						msg_count++;
						if(msg_count<MAX_ERROR_ADD_COMMAND)
						{
							strcat(temp_str,",");
							strcat(temp_str,__N_L1_CROSSED__);
						}
				}
			}
			if(connection_status & L1_N_SHORTED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L1_N_SHORTED__);
					}
			}
			if(connection_status2 & L1_PE_CRIS_CROSSED)
			{
				msg_count++;
				if(msg_count<MAX_ERROR_ADD_COMMAND)
				{
					strcat(temp_str,",");
					strcat(temp_str,__L1_PE_CRIS_CROSSED__);
				}
			}
			else
			{
				if(connection_status & L1_PE_CROSSED)
				{
						msg_count++;
						if(msg_count<MAX_ERROR_ADD_COMMAND)
						{
							strcat(temp_str,",");
							strcat(temp_str,__L1_PE_CROSSED__);
						}
				}
				if(connection_status2 & PE_L1_CROSSED)
				{
						msg_count++;
						if(msg_count<MAX_ERROR_ADD_COMMAND)
						{
							strcat(temp_str,",");
							strcat(temp_str,__PE_L1_CROSSED__);
						}
				}
			}
			if(connection_status & L1_PE_SHORTED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L1_PE_SHORTED__);
					}
			}
			if(connection_status2 & L2_L3_CRIS_CROSSED)
			{
				msg_count++;
				if(msg_count<MAX_ERROR_ADD_COMMAND)
				{
					strcat(temp_str,",");
					strcat(temp_str,__L2_L3_CRIS_CROSSED__);
				}
			}
			else
			{
				if(connection_status & L2_L3_CROSSED)
				{
						msg_count++;
						if(msg_count<MAX_ERROR_ADD_COMMAND)
						{
							strcat(temp_str,",");
							strcat(temp_str,__L2_L3_CROSSED__);
						}
				}
				if(connection_status2 & L3_L2_CROSSED)
				{
						msg_count++;
						if(msg_count<MAX_ERROR_ADD_COMMAND)
						{
							strcat(temp_str,",");
							strcat(temp_str,__L3_L2_CROSSED__);
						}
				}
			}
			if(connection_status & L2_L3_SHORTED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L2_L3_SHORTED__);
					}
			}
			if(connection_status2 & L2_N_CRIS_CROSSED)
			{
				msg_count++;
				if(msg_count<MAX_ERROR_ADD_COMMAND)
				{
					strcat(temp_str,",");
					strcat(temp_str,__L2_N_CRIS_CROSSED__);
				}
			}
			else
			{
				if(connection_status & L2_N_CROSSED)
				{
						msg_count++;
						if(msg_count<MAX_ERROR_ADD_COMMAND)
						{
							strcat(temp_str,",");
							strcat(temp_str,__L2_N_CROSSED__);
						}
				}
				if(connection_status2 & N_L2_CROSSED)
				{
						msg_count++;
						if(msg_count<MAX_ERROR_ADD_COMMAND)
						{
							strcat(temp_str,",");
							strcat(temp_str,__N_L2_CROSSED__);
						}
				}
			}
			if(connection_status & L2_N_SHORTED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L2_N_SHORTED__);
					}
			}
			if(connection_status2 & L2_PE_CRIS_CROSSED)
			{
				msg_count++;
				if(msg_count<MAX_ERROR_ADD_COMMAND)
				{
					strcat(temp_str,",");
					strcat(temp_str,__L2_PE_CRIS_CROSSED__);
				}
			}
			else
			{
				if(connection_status & L2_PE_CROSSED)
				{
						msg_count++;
						if(msg_count<MAX_ERROR_ADD_COMMAND)
						{
							strcat(temp_str,",");
							strcat(temp_str,__L2_PE_CROSSED__);
						}
				}
				if(connection_status2 & PE_L2_CROSSED)
				{
						msg_count++;
						if(msg_count<MAX_ERROR_ADD_COMMAND)
						{
							strcat(temp_str,",");
							strcat(temp_str,__PE_L2_CROSSED__);
						}
				}
			}
			if(connection_status & L2_PE_SHORTED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L2_PE_SHORTED__);
					}
			}
			if(connection_status2 & L3_N_CRIS_CROSSED)
			{
				msg_count++;
				if(msg_count<MAX_ERROR_ADD_COMMAND)
				{
					strcat(temp_str,",");
					strcat(temp_str,__L3_N_CRIS_CROSSED__);
				}
			}
			else
			{
				if(connection_status &  L3_N_CROSSED)
				{
						msg_count++;
						if(msg_count<MAX_ERROR_ADD_COMMAND)
						{
							strcat(temp_str,",");
							strcat(temp_str,__L3_N_CROSSED__);
						}
				}
				if(connection_status2 &  N_L3_CROSSED)
				{
						msg_count++;
						if(msg_count<MAX_ERROR_ADD_COMMAND)
						{
							strcat(temp_str,",");
							strcat(temp_str,__N_L3_CROSSED__);
						}
				}
			}
			if(connection_status & L3_N_SHORTED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L3_N_SHORTED__);
					}
			}
			if(connection_status2 & L3_PE_CRIS_CROSSED)
			{
				msg_count++;
				if(msg_count<MAX_ERROR_ADD_COMMAND)
				{
					strcat(temp_str,",");
					strcat(temp_str,__L3_PE_CRIS_CROSSED__);
				}
			}
			else
			{
				if(connection_status & L3_PE_CROSSED)
				{
						msg_count++;
						if(msg_count<MAX_ERROR_ADD_COMMAND)
						{
							strcat(temp_str,",");
							strcat(temp_str,__L3_PE_CROSSED__);
						}
				}
				if(connection_status2 & PE_L3_CROSSED)
				{
						msg_count++;
						if(msg_count<MAX_ERROR_ADD_COMMAND)
						{
							strcat(temp_str,",");
							strcat(temp_str,__PE_L3_CROSSED__);
						}
				}
			}
			if(connection_status & L3_PE_SHORTED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L3_PE_SHORTED__);
					}
			}
			if(connection_status & L2_OPEN)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L2_OPEN__);
					}
			}
			if(connection_status & L3_OPEN)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L3_OPEN__);
					}
			}
			if(connection_status & N_OPEN)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__N_OPEN__);
					}
			}
			if(connection_status & PE_OPEN)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__PE_OPEN__);
					}
			}
			if(connection_status & N_PE_CROSSED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__PE_N_CROSSED__);
					}
			}
			if(connection_status & N_PE_SHORTED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__N_PE_SHORTED__);
					}
			}
	//		switch (connection_status & STATUS_FAULT_MASK)
	//    {
	//    	case L1_OPEN:
	//				msg_count++;
	//				if(msg_count<MAX_ERROR_ADD_COMMAND)
	//				{
	//					strcat(temp_str,",");
	//					strcat(temp_str,__L1_OPEN__);
	//				}
	//    		break; 
	//    	case L1_L2_CROSSED:
	//				msg_count++;
	//				if(msg_count<MAX_ERROR_ADD_COMMAND)
	//				{
	//					strcat(temp_str,",");
	//					strcat(temp_str,__L1_L2_CROSSED__);
	//				}
	//    		break;
	//			case L1_L2_SHORTED:
	//				msg_count++;
	//				if(msg_count<MAX_ERROR_ADD_COMMAND)
	//				{
	//					strcat(temp_str,",");
	//					strcat(temp_str,__L1_L2_SHORTED__);
	//				}
	//    		break;
	//			case L1_L3_CROSSED:
	//				msg_count++;
	//				if(msg_count<MAX_ERROR_ADD_COMMAND)
	//				{
	//					strcat(temp_str,",");
	//					strcat(temp_str,__L1_L3_CROSSED__);
	//				}
	//    		break;
	//			case L1_L3_SHORTED:
	//				msg_count++;
	//				if(msg_count<MAX_ERROR_ADD_COMMAND)
	//				{
	//					strcat(temp_str,",");
	//					strcat(temp_str,__L1_L3_SHORTED__);
	//				}
	//    		break;
	//			case L1_N_CROSSED:
	//				msg_count++;
	//				if(msg_count<MAX_ERROR_ADD_COMMAND)
	//				{
	//					strcat(temp_str,",");
	//					strcat(temp_str,__L1_N_CROSSED__);
	//				}
	//    		break;
	//			case L1_N_SHORTED:
	//				msg_count++;
	//				if(msg_count<MAX_ERROR_ADD_COMMAND)
	//				{
	//					strcat(temp_str,",");
	//					strcat(temp_str,__L1_N_SHORTED__);
	//				}
	//    		break;
	//			case L1_PE_CROSSED:
	//				msg_count++;
	//				if(msg_count<MAX_ERROR_ADD_COMMAND)
	//				{
	//					strcat(temp_str,",");
	//					strcat(temp_str,__L1_PE_CROSSED__);
	//				}
	//    		break;
	//			case L1_PE_SHORTED:
	//				msg_count++;
	//				if(msg_count<MAX_ERROR_ADD_COMMAND)
	//				{
	//					strcat(temp_str,",");
	//					strcat(temp_str,__L1_PE_SHORTED__);
	//				}
	//    		break;
	//			case L2_L3_CROSSED:
	//				msg_count++;
	//				if(msg_count<MAX_ERROR_ADD_COMMAND)
	//				{
	//					strcat(temp_str,",");
	//					strcat(temp_str,__L2_L3_CROSSED__);
	//				}
	//    		break;
	//			case L2_L3_SHORTED:
	//				msg_count++;
	//				if(msg_count<MAX_ERROR_ADD_COMMAND)
	//				{
	//					strcat(temp_str,",");
	//					strcat(temp_str,__L2_L3_SHORTED__);
	//				}
	//    		break;
	//			case L2_N_CROSSED:
	//				msg_count++;
	//				if(msg_count<MAX_ERROR_ADD_COMMAND)
	//				{
	//					strcat(temp_str,",");
	//					strcat(temp_str,__L2_N_CROSSED__);
	//				}
	//				break;
	//			case L2_N_SHORTED:
	//				msg_count++;
	//				if(msg_count<MAX_ERROR_ADD_COMMAND)
	//				{
	//					strcat(temp_str,",");
	//					strcat(temp_str,__L2_N_SHORTED__);
	//				}
	//				break;
	//			case L2_PE_CROSSED:
	//				msg_count++;
	//				if(msg_count<MAX_ERROR_ADD_COMMAND)
	//				{
	//					strcat(temp_str,",");
	//					strcat(temp_str,__L1_PE_CROSSED__);
	//				}
	//    		break; 
	//    	case L2_PE_SHORTED:
	//				msg_count++;
	//				if(msg_count<MAX_ERROR_ADD_COMMAND)
	//				{
	//					strcat(temp_str,",");
	//					strcat(temp_str,__L2_PE_SHORTED__);
	//				}
	//    		break;
	//			case L3_N_CROSSED:
	//				msg_count++;
	//				if(msg_count<MAX_ERROR_ADD_COMMAND)
	//				{
	//					strcat(temp_str,",");
	//					strcat(temp_str,__L3_N_CROSSED__);
	//				}
	//    		break;
	//			case L3_N_SHORTED:
	//				msg_count++;
	//				if(msg_count<MAX_ERROR_ADD_COMMAND)
	//				{
	//					strcat(temp_str,",");
	//					strcat(temp_str,__L3_N_SHORTED__);
	//				}
	//    		break;
	//			case L3_PE_CROSSED:
	//				msg_count++;
	//				if(msg_count<MAX_ERROR_ADD_COMMAND)
	//				{
	//					strcat(temp_str,",");
	//					strcat(temp_str,__L3_PE_CROSSED__);
	//				}
	//    		break;
	//			case L3_PE_SHORTED:
	//				msg_count++;
	//				if(msg_count<MAX_ERROR_ADD_COMMAND)
	//				{
	//					strcat(temp_str,",");
	//					strcat(temp_str,__L3_PE_SHORTED__);
	//				}
	//    		break;
	//			case L2_OPEN:
	//				msg_count++;
	//				if(msg_count<MAX_ERROR_ADD_COMMAND)
	//				{
	//					strcat(temp_str,",");
	//					strcat(temp_str,__L2_OPEN__);
	//				}
	//    		break;
	//			case L3_OPEN:
	//				msg_count++;
	//				if(msg_count<MAX_ERROR_ADD_COMMAND)
	//				{
	//					strcat(temp_str,",");
	//					strcat(temp_str,__L3_OPEN__);
	//				}
	//    		break;
	//			case N_OPEN:
	//				msg_count++;
	//				if(msg_count<MAX_ERROR_ADD_COMMAND)
	//				{
	//					strcat(temp_str,",");
	//					strcat(temp_str,__N_OPEN__);
	//				}
	//    		break;
	//			case PE_OPEN:
	//				msg_count++;
	//				if(msg_count<MAX_ERROR_ADD_COMMAND)
	//				{
	//					strcat(temp_str,",");
	//					strcat(temp_str,__PE_OPEN__);
	//				}
	//    		break;
	//			case N_PE_CROSSED:
	//				msg_count++;
	//				if(msg_count<MAX_ERROR_ADD_COMMAND)
	//				{
	//					strcat(temp_str,",");
	//					strcat(temp_str,__PE_N_CROSSED__);
	//				}
	//    		break;
	//			case N_PE_SHORTED:
	//				msg_count++;
	//				if(msg_count<MAX_ERROR_ADD_COMMAND)
	//				{
	//					strcat(temp_str,",");
	//					strcat(temp_str,__N_PE_SHORTED__);
	//				}
	//				break;
	//    	default:
	//    		break;
	//    }
		}
		else
		{
			switch (connection_status2 & STATUS2_FAULT_CROSSED_MASK)
			{
				case L1_L2_CRIS_CROSSED:
					strcat(temp_str,__L1_L2_CRIS_CROSSED__);
					break;
				case L1_L3_CRIS_CROSSED:
					strcat(temp_str,__L1_L3_CRIS_CROSSED__);
					break;
				case L2_L3_CRIS_CROSSED:
					strcat(temp_str,__L2_L3_CRIS_CROSSED__);
					break;
				case L1_N_CRIS_CROSSED:
					strcat(temp_str,__L1_N_CRIS_CROSSED__);
					break;
				case L2_N_CRIS_CROSSED:
					strcat(temp_str,__L2_N_CRIS_CROSSED__);
					break;
				case L3_N_CRIS_CROSSED:
					strcat(temp_str,__L3_N_CRIS_CROSSED__);
					break;
				case L1_PE_CRIS_CROSSED:
					strcat(temp_str,__L1_PE_CRIS_CROSSED__);
					break;
				case L2_PE_CRIS_CROSSED:
					strcat(temp_str,__L2_PE_CRIS_CROSSED__);
					break;
				case L3_PE_CRIS_CROSSED:
					strcat(temp_str,__L3_PE_CRIS_CROSSED__);
					break;
				case N_PE_CRIS_CROSSED:
					strcat(temp_str,__N_PE_CRIS_CROSSED__);
					break;
				default:
					switch (connection_status & STATUS_FAULT_SHORTED_MASK)
					{
						case L1_L2_SHORTED:
							strcat(temp_str,__L1_L2_SHORTED__);
							break;
						case L1_L3_SHORTED:
							strcat(temp_str,__L1_L3_SHORTED__);
							break;
						case L2_L3_SHORTED:
							strcat(temp_str,__L2_L3_SHORTED__);
							break;
						case L1_N_SHORTED:
							strcat(temp_str,__L1_N_SHORTED__);
							break;
						case L2_N_SHORTED:
							strcat(temp_str,__L2_N_SHORTED__);
							break;
						case L3_N_SHORTED:
							strcat(temp_str,__L3_N_SHORTED__);
							break;
						case L1_PE_SHORTED:
							strcat(temp_str,__L1_PE_SHORTED__);
							break;
						case L2_PE_SHORTED:
							strcat(temp_str,__L2_PE_SHORTED__);
							break;
						case L3_PE_SHORTED:
							strcat(temp_str,__L3_PE_SHORTED__);
							break;
						case N_PE_SHORTED:
							strcat(temp_str,__N_PE_SHORTED__);
							break;
						default:
							switch (connection_status & STATUS_FAULT_OPEN_MASK)
							{
								case L1_OPEN:
									strcat(temp_str,__L1_OPEN__);
									break;
								case L2_OPEN:
									strcat(temp_str,__L2_OPEN__);
									break;
								case L3_OPEN:
									strcat(temp_str,__L3_OPEN__);
									break;
								case N_OPEN:
									strcat(temp_str,__N_OPEN__);
									break;
								case PE_OPEN:
									strcat(temp_str,__PE_OPEN__);
									break;
								default:
									temp_str[0]='M';
									temp_str[1]='A';
									temp_str[2]='I';
									temp_str[3]='N';
									temp_str[4]='_';
									temp_str[5]='R';
									temp_str[6]='E';
									temp_str[7]='S';
									temp_str[8]='|';
									temp_str[9]='P';
									temp_str[10]='A';
									temp_str[11]='S';
									temp_str[12]='S';
									//strcat(temp_str,__PASS__);	//ce ni nic od prej nastetega potem je to pass
									break;
							}
							break;
					}
					break;
			}
		}
		if(AUTO_DETECT_P_NUM)
			transmitFunc(__C_W, __RESULT ,temp_str,"", device.device_dir);
			//SendComMessage(_ON,_ID_TFA,device_ID,__CORD__,__CORD_CW_RESULT__,temp_str,__3_PHASE__,dir);
		else
			transmitFunc(__C_W, __RESULT ,temp_str,"", device.device_dir);
			//SendComMessage(_ON,_ID_TFA,device_ID,__CORD__,__CORD_CW_RESULT__,temp_str,"",dir);
	}
	cord_task_control &= ~__CORD_RPE_L_STARTED;
}
void cord_get_and_transmitt_result(void)
{
//	uint32_t error_cnt=0;
	char temp_buff[MAX_ADDITIONAL_COMMANDS_LENGTH];
	if(connection_reg & (~PASS_MASK))	//preveri ce je kaj prekrizano
	{
		if(!(connection_reg & L1_L1))//ce L1 ni na L1 pogledamo kje je
		{
			switch(connection_reg & L1_CROSSED_MASK)//pogledamo s katero je prekrizana
			{
				case L1_L2_CROSSED_MASK: connection_status |= L1_L2_CROSSED; break;
				case L1_L3_CROSSED_MASK: connection_status |= L1_L3_CROSSED; break;
				case L1_N_CROSSED_MASK: connection_status |= L1_N_CROSSED; break;
				case L1_PE_CROSSED_MASK: connection_status |= L1_PE_CROSSED; break;
				case 0: connection_status |= L1_OPEN; break;
				default: 
					connection_status2 |= MULTI_FAULT;	//ce pridemo sem je najverjetneje crossed in shorted
					connection_status2 |= CROSSED_AND_SHORTED;
			}
		}
		else	//ce je L1 na L1 preverimo se ce je slucajno shorted
		{
			switch(connection_reg & L1_CROSSED_MASK)//pogledamo s katero je po moznosti v stiku
			{
				case L1_L2_CROSSED_MASK: connection_status |= L1_L2_SHORTED; break;
				case L1_L3_CROSSED_MASK: connection_status |= L1_L3_SHORTED; break;
				case L1_N_CROSSED_MASK: connection_status |= L1_N_SHORTED; break;
				case L1_PE_CROSSED_MASK: connection_status |= L1_PE_SHORTED; break;
				case 0: connection_status |= L1_OK; break;	//vse ok
				default: 
					connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
					connection_status2 |= MULTI_SHORTED;
			}
		}
		if(!(connection_reg & L2_L2))//ce L2 ni na L2 pogledamo kje je
		{
			switch(connection_reg & L2_CROSSED_MASK)//pogledamo s katero je prekrizana
			{
				case L2_L3_CROSSED_MASK: connection_status |= L2_L3_CROSSED; break;
				case L2_N_CROSSED_MASK: connection_status |= L2_N_CROSSED; break;
				case L2_PE_CROSSED_MASK: connection_status |= L2_PE_CROSSED; break;
				case 0: connection_status |= L2_OPEN; break;
				default: 
					connection_status2 |= MULTI_FAULT;	//ce pridemo sem je najverjetneje crossed in shorted
					connection_status2 |= CROSSED_AND_SHORTED;
			}
		}
		else	//ce je L2 na L2 preverimo se ce je slucajno shorted
		{
			switch(connection_reg & L2_CROSSED_MASK)//pogledamo s katero je po moznosti v stiku
			{
				case L2_L3_CROSSED_MASK: connection_status |= L2_L3_SHORTED; break;
				case L2_N_CROSSED_MASK: connection_status |= L2_N_SHORTED; break;
				case L2_PE_CROSSED_MASK: connection_status |= L2_PE_SHORTED; break;
				case 0: connection_status |= L2_OK; break;	//vse ok
				default: 
					connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
					connection_status2 |= MULTI_SHORTED;
			}
		}
		if(!(connection_reg & L3_L3))//ce L3 ni na L3 pogledamo kje je
		{
			switch(connection_reg & L3_CROSSED_MASK)//pogledamo s katero je prekrizana
			{
				case L3_N_CROSSED_MASK: connection_status |= L3_N_CROSSED; break;
				case L3_PE_CROSSED_MASK: connection_status |= L3_PE_CROSSED; break;
				case 0: connection_status |= L3_OPEN; break;
				default: 
					connection_status2 |= MULTI_FAULT;	//ce pridemo sem je najverjetneje crossed in shorted
					connection_status2 |= CROSSED_AND_SHORTED;
			}
		}
		else	//ce je L3 na L3 preverimo se ce je slucajno shorted
		{
			switch(connection_reg & L3_CROSSED_MASK)//pogledamo s katero je po moznosti v stiku
			{
				case L3_N_CROSSED_MASK: connection_status |= L3_N_SHORTED; break;
				case L3_PE_CROSSED_MASK: connection_status |= L3_PE_SHORTED; break;
				case 0: connection_status |= L3_OK; break;	//vse ok
				default: 
					connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
					connection_status2 |= MULTI_SHORTED;
			}
		}
		if(!(connection_reg & N_N))//ce N ni na N pogledamo kje je
		{
			if(connection_reg & N_PE)
				connection_status |= N_PE_CROSSED;
			else
				connection_status |= N_OPEN;
		}
		else
		{
			if(connection_reg & N_PE)
				connection_status |= N_PE_SHORTED;
			else
				connection_status |= N_OK;
		}
		if(!(connection_reg & PE_PE))
		{
			connection_status |= PE_OPEN;
		}
		//ce vse skupaj nima veze z L2 in L3 gre za enofazni kabel
		if(!(connection_status & (~(CABLE_1P_MASK | L2_OPEN | L3_OPEN))))
		{
			if(connection_status2 & MULTI_FAULT)
			{
				
			}
		}
	}
	else	//ce ni nic prekrizano pogledamo ce so vse povezane
	{
		if(connection_reg == PASS_MASK)
			strcat(temp_buff,__PASS__);
		else if(connection_reg == L1_OPEN_MASK)
			strcat(temp_buff,__L1_OPEN__);
		else if(connection_reg == L2_OPEN_MASK)
			strcat(temp_buff,__L2_OPEN__);
		else if(connection_reg == L3_OPEN_MASK)
			strcat(temp_buff,__L3_OPEN__);
		else if(connection_reg == N_OPEN_MASK)
			strcat(temp_buff,__N_OPEN__);
		else if(connection_reg == PE_OPEN_MASK)
			strcat(temp_buff,__PE_OPEN__);
		//enofazni kabel
		else if(connection_reg == PASS_1P_MASK)
			strcat(temp_buff,__PASS_1P__);
		else if(connection_reg == L1_OPEN_1P_MASK)
			strcat(temp_buff,__L1_1P_OPEN__);
		else if(connection_reg == N_OPEN_1P_MASK)
			strcat(temp_buff,__N_1P_OPEN__);
		else if(connection_reg == PE_OPEN_1P_MASK)
			strcat(temp_buff,__PE_1P_OPEN__);
		else		
			strcat(temp_buff,__MULTIFAULT__);
	}
}



void SetCordLimit(uint32_t limit, char * lim_str)
{
	switch(limit)
	{
		case SET_CORD_RPE_LIM:
			CordRpeLimit = atof(lim_str);
			break;
		case SET_CORD_RISO_LIM:
			CordRisoLimit = atof(lim_str);
			break;
		case SET_CORD_CONT_LIM:
			RpeContinuityLimit = atof(lim_str);
			break;
		default:
			break;
	}
}

//++++++++++++++++++++++++++++++++++++++++++++++RPE++++++++++++++++++++++++++++++++++++++++
void CordRPEStart(void)
{
	if(checkAndChangeMeasurement(__CORD))
	{
		set_REL(8);
		cord_task_control |= __CORD_RPE_IN_PROGRESS;
		transmitCommFunc(__RPE,__START,__HIGH,"",device.device_dir);
		//SendComMessage(_ON,_ID_TFA,device.device_ID,__MACHINES__,__MECH_RPE_STARTED__,"","",device.device_dir);
	}
	else 
		set_event(CORD_RPE_START,CordRPEStart);
}

void CordRPEStop(void)
{
	rst_REL(8);
	cord_task_control &= ~__CORD_RPE_IN_PROGRESS;
	cord_task_control |= __CORD_RPE_IN_PROGRESS;
	//SendComMessage(_ON,_ID_TFA,device.device_ID,__MACHINES__,__MECH_RPE_STOPPED__,"","",device.device_dir);
}
//+++++++++++++++++++++++++++++++++++++++++++++++END RPE++++++++++++++++++++++++++++++++++

