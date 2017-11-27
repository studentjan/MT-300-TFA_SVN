#include "do_task.h"
#include "defines.h"
#include "os.h"
#include "tasks.h"
#include "rel_driver.h"
#include "comunication.h"
#include "meas_task.h"
#include "serial_com.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

//--------------spremenljivke--------------------
extern uint32_t global_control;
extern uint32_t connection_control;
uint32_t meas_task_control=0;
float cord_rpe_resistance;
static uint32_t connection_reg=0;
static uint32_t connection_status =0;
static uint32_t connection_status2=0;

bool cord_check_resistance(void);
void cord_get_and_transmitt_result(void);
bool set_next_cord_task_case=false;

//spremenljivke za nastavitve
bool manual_set_phase_num = false;
bool CORD_PHASE_NUM_SETTING = _3_PHASE;

void start_cord_normal(void)
{
	static int count=0;
	if(count==0)
	{
		global_control &= (~__INIT_TEST_PASS);
		set_event(POWER_ON_TEST,power_on_test);
		count++;
	}
	if(count==1)
	{
		if(global_control & __INIT_TEST_PASS)
			count++;
		else if(global_control & __INIT_TEST_FAIL)
		{;}
		else 
			set_timer(START_CORD_NORMAL,5,start_cord_normal);
	}
	if(count == 2)
	{
		count=0;
		set_cord_init();
		meas_task_control |= __CORD_MEAS_IN_PROG;
		set_event(SEND_TFA_MAINS_STATUS,send_mains_status);
		set_event(CORD_MEAS_NORMAL,cord_meas_normal);
	}
}
void cord_meas_normal(void)
{
	static uint32_t count=0;
//	static uint32_t count2=0;
	static struct connected_device device;
	switch(count)
	{
		case 0:
		{
			//posljemo ukaz za start
			SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__START_RPE_5A__,"",device.device_dir);
			count++;
			break;
		}
		case 1:
		{
			device = get_connected_device();
			set_REL(10);
			set_REL(33);
			SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
			meas_task_control |= __CORD_RES_REQUESTED;
			count++;
			break;
		}
		case 2://L1 L1 POVEZANA?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
					//ce je L1 na L1 preverimo ce ni slucajn se kje drgje
					connection_reg |= L1_L1;
				else
					//ce ni L1 direktno na L1 pogledamo ce je mogoce povezan kam drugam
					connection_reg &= (~L1_L1);
				rst_REL(33);
				set_REL(30);
				count++;
				SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"L1_L1",device.device_dir);
				meas_task_control |= __CORD_RES_REQUESTED;
			}
			break;
		}
		case 3://L1 PE POVEZANA?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
				{
					if(connection_reg & L1_L1)	//ce ne ustreza temu pogoju je lahko crossed
					{
						;
					}
					else
					{
						//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
						if(connection_reg & L1_CROSS_CON_MASK)
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
//							count++;
//							rst_REL(30);
//							set_REL(36);
//						}
//						else 
//							count =100;	//skocmmo vn iz zanke ko zaznamo multifault
//					}
//					else
//					{
						rst_REL(30);
						set_REL(36);
						count++;
//					}
					connection_reg |= L1_PE;
				}
				else
				{
//					if(connection_status2 & MULTI_FAULT)
//					{
//						if(MULTI_FAULT_CONTINUE)
//						{
//							count++;
//							rst_REL(30);
//							set_REL(36);
//						}
//						else 
//							count =100;	//skocmmo vn iz zanke ko zaznamo multifault
//					}
//					else
//					{
						rst_REL(30);
						set_REL(36);
						count++;
//					}
					connection_reg &= (~L1_PE);
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"L1_PE",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 4://L1 N POVEZANA?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
							if(manual_set_phase_num && (CORD_PHASE_NUM_SETTING==_1_PHASE))
							{
								rst_REL(10);
								set_REL(13);
								count=14;//gremo naprej na N-N
							}
							else
							{
								rst_REL(36);
								set_REL(35);
								count++;
							}
						}
						else
							count =100;
					}
					else
					{
						if(manual_set_phase_num && (CORD_PHASE_NUM_SETTING==_1_PHASE))
							{
								rst_REL(10);
								set_REL(13);
								count=14;//gremo naprej na N-N
							}
							else
							{
								rst_REL(36);
								set_REL(35);
								count++;
							}
					}
					connection_reg |= L1_N;
				}
				else
				{
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							if(manual_set_phase_num && (CORD_PHASE_NUM_SETTING==_1_PHASE))
							{
								rst_REL(10);
								set_REL(13);
								count=14;//gremo naprej na N-N
							}
							else
							{
								rst_REL(36);
								set_REL(35);
								count++;
							}
						}
						else
							count =100;
					}
					else
					{
						if(manual_set_phase_num && (CORD_PHASE_NUM_SETTING==_1_PHASE))
							{
								rst_REL(10);
								set_REL(13);
								count=14;//gremo naprej na N-N
							}
							else
							{
								rst_REL(36);
								set_REL(35);
								count++;
							}
					}
					connection_reg &= (~L1_N);
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"L1_N",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 5: //L1 L3 POVEZANA?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						if(connection_reg & L1_CROSS_CON_MASK)
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
							count++;
						}
						else
							count =100;
					}
					else
					{
						rst_REL(35);
						set_REL(34);
						count++;
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
							count++;
						}
						else
							count =100;
					}
					else
					{
						rst_REL(35);
						set_REL(34);
						count++;
					}
					connection_reg &= (~L1_L3);
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"L1_L3",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 6://L1 L2 POVEZANA?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						if(connection_reg & L1_CROSS_CON_MASK)
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
							count++;
						}
						else
							count =100;
					}
					else
					{
						rst_REL(10);
						set_REL(11);
						count++;
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
							count++;
						}
						else
							count =100;
					}
					else
					{
						rst_REL(10);
						set_REL(11);
						count++;
					}
					connection_reg &= (~L1_L2);
				}
				//preverjanje za crossed
				if((!(connection_reg & L1_L1)))
				{
					switch (connection_reg & L1_CROSSED_MASK)
          {
          	case L1_L2_CROSSED_MASK: connection_status |= L1_L2_CROSSED; break;
						case L1_L3_CROSSED_MASK: connection_status |= L1_L3_CROSSED; break;
						case L1_N_CROSSED_MASK: connection_status |= L1_N_CROSSED; break;
						case L1_PE_CROSSED_MASK: connection_status |= L1_PE_CROSSED; break;
						case 0: connection_status |= L1_OPEN; break;	//vse ok
						default: 
							connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
							connection_status2 |= CROSSED_AND_SHORTED;
          }
				}
				//ce ni crossed in shorted potem pomeni, da je OK
				else if((connection_reg & L1_L1)&(!(connection_reg & (L1_CROSSED_MASK|L1_CROSSED_MASK2))))
				{
					connection_status |= L1_OK;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"L1_L2",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 7://L2 L2 POVEZANA?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
				{
					connection_reg |= L2_L2;
					//--------pogoji ki se jih da razbrati zaradi prejsnih merjenj za shorted-------------
//					if((connection_status & L1_L2_SHORTED)&&(connection_status & L1_PE_SHORTED))
//					{connection_status |= L2_PE_SHORTED; connection_reg |= L2_PE;}
//					if((connection_status & L1_L2_SHORTED)&&(connection_status & L1_N_SHORTED))
//					{connection_status |= L2_N_SHORTED;connection_reg |= L2_N;}
//					if((connection_status & L1_L2_SHORTED)&&(connection_status & L1_L3_SHORTED))
//					{connection_status |= L2_L3_SHORTED;connection_reg |= L2_L3;}
//					
					//------------pogoji za crossed (crossed in shorted ne morta bit skup-------
//					if(connection_status & L1_L2_CROSSED)
//					{
//						//L1 sigurno ni povezan z L2 na tej strani
//						connection_status2 |= CROSSED_AND_OPEN;
//						connection_status2 |= MULTI_FAULT;
//					}
					
//					if((connection_status & L2_PE_SHORTED)&&(connection_status & L2_N_SHORTED)&&(connection_status & L2_L3_SHORTED))
//					{
//						if(connection_status2 & MULTI_FAULT)
//						{
//							if(MULTI_FAULT_CONTINUE)
//							{
//								//Gremo na L3 - L3
//								rst_REL(11);
//								set_REL(12);
//								rst_REL(34);
//								set_REL(35);
//								count=11;
//							}
//							else count = 100;
//						}
//						else
//						{
//							//Gremo na L3 - L3
//							rst_REL(11);
//							set_REL(12);
//							rst_REL(34);
//							set_REL(35);
//							count=11;
//						}
//					}
//					else if((connection_status & L2_PE_SHORTED)&&(connection_status & L2_N_SHORTED))
//					{
//						if(connection_status2 & MULTI_FAULT)
//						{
//							if(MULTI_FAULT_CONTINUE)
//							{
//								//Gremo na L2 - L3
//								rst_REL(34);
//								set_REL(35);
//								count=10;
//							}
//							else count = 100;
//						}
//						else
//						{
//							//Gremo na L2 - L3
//							rst_REL(34);
//							set_REL(35);
//							count=10;
//						}
//					}
//					else if((connection_status & L2_PE_SHORTED))
//					{
//						if(connection_status2 & MULTI_FAULT)
//						{
//							if(MULTI_FAULT_CONTINUE)
//							{
//								//Gremo na L2 - N
//								rst_REL(34);
//								set_REL(36);
//								count=9;
//							}
//							else count = 100;
//						}
//						else
//						{
//							//Gremo na L2 - N
//							rst_REL(34);
//							set_REL(36);
//							count=9;
//						}
//					}
//					else //gremo za ena naprej torej na L2 - PE
//					{
						if((!(connection_reg & L1_L1))&&(connection_reg & L1_L2))	
						{
							connection_status2 |= CROSSED_AND_OPEN;
							connection_status2 |= MULTI_FAULT;
						}
						//-----------
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
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
									count++;
								}
								else //Gremo na L2 - L1
								{
									rst_REL(34);
									set_REL(30);
									count=47;
								}
							}
							else count = 100;
						}
						else
						{
							if((connection_reg & L1_L1)&&(connection_reg & L1_L2)) //gremo na L2 - PE
							{
								if(connection_reg & L2_L2)
									connection_reg |= L2_L1;
								else
									connection_reg &= (~L2_L1);
								rst_REL(34);
								set_REL(33);
								count++;
							}
							else //Gremo na L2 - L1
							{
								rst_REL(34);
								set_REL(30);
								count=47;
							}
						}
//					}

				}
				else
				{
					if((connection_reg & L1_L1)&&(connection_reg & L1_L2))	
						{
							connection_status2 |= CROSSED_AND_SHORTED;
							connection_status2 |= MULTI_FAULT;
						}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							if((connection_reg & L1_L1)&&(connection_reg & L1_L2)) //gremo na L2 - PE
							{
								if(connection_reg & L2_L2)
									connection_reg |= L2_L1;
								else
									connection_reg &= (~L2_L1);
								rst_REL(34);
								set_REL(33);
								count++;
							}
							else //Gremo na L2 - L1
							{
								rst_REL(34);
								set_REL(30);
								count=47;
							}
						}
						else count = 100;
					}
					else
					{
						if((connection_reg & L1_L1)&&(connection_reg & L1_L2)) //gremo na L2 - PE
						{
							if(connection_reg & L2_L2)
								connection_reg |= L2_L1;
							else
								connection_reg &= (~L2_L1);
							rst_REL(34);
							set_REL(33);
							count++;
						}
						else //Gremo na L2 - L1
						{
							rst_REL(34);
							set_REL(30);
							count=47;
						}
					}
					connection_reg &= (~L2_L2);
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"L2_L2",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 8://L2 PE POVEZANA?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						if(connection_reg & L2_CROSS_CON_MASK)
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
								rst_REL(30);
								set_REL(36);
								count++;
						}
						else
							count =100;
					}
					else
					{
							rst_REL(30);
							set_REL(36);
							count++;
					}
					connection_reg |= L2_PE;
				}
				else
				{
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							rst_REL(30);
							set_REL(36);
							count++;
						}
						else count = 100;
					}
					else
					{
						rst_REL(30);
						set_REL(36);
						count++;
					}
					connection_reg &= (~L2_PE);
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"L2_PE",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 9://L2 N POVEZANA?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				rst_REL(36);
				set_REL(35);
				if(cord_check_resistance())
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
						if(connection_reg & L2_CROSS_CON_MASK)
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
								rst_REL(36);
								set_REL(35);
								count++;
						}
						else
							count =100;
					}
					else
					{
							rst_REL(36);
							set_REL(35);
							count++;
					}
					connection_reg |= L2_N;
				}
				else
				{
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							rst_REL(36);
							set_REL(35);
							count++;
						}
						else count = 100;
					}
					else
					{
						rst_REL(36);
						set_REL(35);
						count++;
					}
					connection_reg &= (~L2_N);
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"L2_N",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 10://L2 L3 POVEZANA?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						if(connection_reg & L2_CROSS_CON_MASK)
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
							rst_REL(11);
							set_REL(12);
							count++;
						}
						else
							count =100;
					}
					else
					{
						//Gremo na L3 - L3
						rst_REL(11);
						set_REL(12);
						count++;
					}
					connection_reg |= L2_L3;
				}
				else
				{
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							rst_REL(11);
							set_REL(12);
							count++;
						}
						else count = 100;
					}
					else
					{
						rst_REL(11);
						set_REL(12);
						count++;
					}
					connection_reg &= (~L2_L3);
				}
				if(!(connection_reg & L2_L2))
				{
					if((connection_reg & L1_L2)&&(connection_reg & L2_L1))
						connection_status2 |= L1_L2_CRIS_CROSSED;
					else
					{
						switch (connection_reg & (L2_CROSSED_MASK | L2_L1))
						{
						  case L2_L1: connection_status2 |= L2_L1_CROSSED; break;	
							case L2_N_CROSSED_MASK: connection_status |= L2_N_CROSSED; break;
							case L2_L3_CROSSED_MASK: connection_status |= L2_L3_CROSSED; break;
							case L2_PE_CROSSED_MASK: connection_status |= L2_PE_CROSSED; break;
							case 0: 
								if(connection_status & L1_OPEN)
								{
									connection_status2 |= MULTI_OPENED;
									connection_status2 |= MULTI_FAULT;
								}
								connection_status |= L2_OPEN; 
								break;	//vse ok
							default: 
								connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
								connection_status2 |= CROSSED_AND_SHORTED;
						}
					}
				}
				//ce ni crossed in shorted potem pomeni, da je OK
				else if((connection_reg & L2_L2)&(!(connection_reg & (L2_CROSSED_MASK|L2_CROSSED_MASK2))))
				{
					connection_status |= L2_OK;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"L2_L3",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 11://L3 L3 POVEZANA?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
									count++;
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
									count=48;
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
									count=49;
								}
								else //Gremo na L2 - L1
								{
									rst_REL(35);
									set_REL(33);
									count=50;
								}
//								rst_REL(12);
//								set_REL(13);
//								rst_REL(35);
//								set_REL(36);
//								count=14;
							}
							else count = 100;
						}
						else 
						{
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
									count++;
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
									count=48;
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
									count=49;
								}
								else //Gremo na L2 - L1
								{
									rst_REL(35);
									set_REL(33);
									count=50;
								}
//							rst_REL(12);
//							set_REL(13);
//							rst_REL(35);
//							set_REL(36);
//							count=14;
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
//								count=13;
//							}
//							else count = 100;
//						}
//						else
//						{
//							//Gremo na L3 - N
//							rst_REL(35);
//							set_REL(36);
//							count=13;
//						}
						
					}
					else //gremo za ena naprej torej na L3 - PE
					{
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
							{
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
									count++;
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
									count=48;
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
									count=49;
								}
								else //Gremo na L2 - L1
								{
									rst_REL(35);
									set_REL(33);
									count=50;
								}
//								rst_REL(35);
//								set_REL(30);
//								count++;
							}
							else count = 100;
						}
						else
						{
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
								count++;
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
								count=48;
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
								count=49;
							}
							else //Gremo na L2 - L1
							{
								rst_REL(35);
								set_REL(33);
								count=50;
							}
//							rst_REL(35);
//							set_REL(30);
//							count++;
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
////								count=18;
////							}
////							else count = 100;
////						}
////						else
////						{
////							rst_REL(35);
////							set_REL(33);
////							count=18;
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
////								count=19;
////							}
////							else count = 100;
////						}
////						else
////						{
////							rst_REL(35);
////							set_REL(34);
////							count=19;
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
////								count++;
////							}
////							else count = 100;
////						}
////						else
////						{
//							rst_REL(34);
//							set_REL(30);
//							count++;
////						}
//					}
//					connection_reg &= (~L3_L3);
//				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"L3_L3",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 12://L3 PE POVEZANA?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						if(connection_reg & L3_CROSS_CON_MASK)
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
//								count=14;
//							}
//							else
//							{
								rst_REL(30);
								set_REL(36);
								count++;
//							}
						}
						else
							count =100;
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
//							count=14;
//						}
//						else
//						{
							rst_REL(30);
							set_REL(36);
							count++;
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
							rst_REL(30);
							set_REL(36);
							count++;
						}
						else count = 100;
					}
					else
					{
						rst_REL(30);
						set_REL(36);
						count++;
					}
					connection_reg &= (~L3_PE);
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"L3_PE",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 13://L3 N POVEZANA?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						if(connection_reg & L3_CROSS_CON_MASK)
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
							rst_REL(12);
							set_REL(13);
							count++;
						}
						else
							count =100;
					}
					else
					{
						//Gremo na N - N
						rst_REL(12);
						set_REL(13);
						count++;
					}
					connection_reg |= L3_N;
				}
				else
				{
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							rst_REL(12);
							set_REL(13);
							count++;
						}
						else count = 100;
					}
					else
					{
						rst_REL(12);
						set_REL(13);
						count++;
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
						switch (connection_reg & (L3_CROSSED_MASK | L3_L1 | L3_L2))
						{
						  case L3_L1: connection_status2 |= L3_L1_CROSSED; break;	
							case L3_L2: connection_status2 |= L3_L1_CROSSED; break;
							case L3_N_CROSSED_MASK: connection_status |= L3_N_CROSSED; break;
							case L3_PE_CROSSED_MASK: connection_status |= L3_PE_CROSSED; break;
							case 0: 
								if(connection_status & (L1_OPEN | L2_OPEN))
								{
									connection_status2 |= MULTI_FAULT;
									connection_status2 |= MULTI_OPENED;
								}
								connection_status |= L3_OPEN; 
								break;	//vse ok
							default: 
								connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
								connection_status2 |= CROSSED_AND_SHORTED;
						}
					}
				}
				//ce ni crossed in shorted potem pomeni, da je OK
				else if((connection_reg & L3_L3)&(!(connection_reg & (L3_CROSSED_MASK|L3_CROSSED_MASK2))))
				{
					connection_status |= L3_OK;
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
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"L3_N",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 14:	//N N POVEZANA?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				rst_REL(36);
				set_REL(30);
				if(cord_check_resistance())
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
							else count = 100;
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
//								count=22;
//							}
//							else count = 100;
//						}
//						else
//						{
//							rst_REL(36);
//							set_REL(33);
//							count=22;
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
//								count=23;
//							}
//							else count = 100;
//						}
//						else
//						{
//							rst_REL(36);
//							set_REL(34);
//							count=23;
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
//								count=24;
//							}
//							else count = 100;
//						}
//						else
//						{
//							rst_REL(36);
//							set_REL(35);
//							count=24;
//						}
//					}
//					else
//					{
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
								set_next_cord_task_case = true;
							else count = 100;
						}
						else
							set_next_cord_task_case = true;
//					}
					connection_reg &= (~N_N);
				}
				if(set_next_cord_task_case)
				{
					set_next_cord_task_case = false;
					if(!((connection_reg & L1_L1)&&(connection_reg & L1_N))) //gremo na L2 - PE
					{
						rst_REL(36);
						set_REL(33);
						count=52;
					}
					else if((!((connection_reg & L2_L2)&&(connection_reg & L2_N)))&&(!(manual_set_phase_num && CORD_PHASE_NUM_SETTING==_1_PHASE))) //gremo na L2 - PE
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
						count=53;
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
						count=54;
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
						count++;
					}
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"N_N",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 15://N PE POVEZANA?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						if(connection_reg & N_CROSS_CON_MASK)
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= CROSSED_AND_SHORTED;
						}
					}
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							rst_REL(13);
							set_REL(8);
							count++;
						}
						else
							count =100;
					}
					else
					{
						//Gremo na N - N
						rst_REL(13);
						set_REL(8);
						count++;
					}
					connection_reg |= N_PE;
				}
				else
				{
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							rst_REL(13);
							set_REL(8);
							count++;
						}
						else count = 100;
					}
					else
					{
						rst_REL(13);
						set_REL(8);
						count++;
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
						switch (connection_reg & (L2_CROSSED_MASK | N_L1 | N_L2 | N_L3))
						{
						  case N_L1: connection_status2 |= N_L1_CROSSED; break;	
							case N_L2: connection_status2 |= N_L2_CROSSED; break;
							case N_L3: connection_status2 |= N_L3_CROSSED; break;
							case N_PE_CROSSED_MASK: connection_status |= N_PE_CROSSED; break;
							case 0: 
								if(connection_status & (L1_OPEN | L2_OPEN | L3_OPEN))
								{
									connection_status2 |= MULTI_FAULT;
									connection_status2 |= MULTI_OPENED;
								}
								connection_status |= N_OPEN; 
								break;	//vse ok
							default: 
								connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
								connection_status2 |= CROSSED_AND_SHORTED;
						}
					}
				}
				//ce ni crossed in shorted potem pomeni, da je OK
				else if((connection_reg & N_N)&(!(connection_reg & (N_CROSSED_MASK|N_CROSSED_MASK2))))
				{
					connection_status |= N_OK;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"N_PE",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 16://PE PE POVEZANA?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
							else count = 100;
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
//								count=31;
//							}
//							else count = 100;
//						}
//						else
//						{
//							rst_REL(30);
//							set_REL(33);
//							count=31;
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
//								count=35;
//							}
//							else count = 100;
//						}
//						else
//						{
//							rst_REL(30);
//							set_REL(34);
//							count=35;
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
//								count=39;
//							}
//							else count = 100;
//						}
//						else
//						{
//							rst_REL(30);
//							set_REL(35);
//							count=39;
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
//								count=43;
//							}
//							else count = 100;
//						}
//						else
//						{
//							rst_REL(30);
//							set_REL(36);
//							count=43;
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
//								count=98;
//							}
//							else count = 100;
//						}
//						else
//						{
//							rst_REL(36);
//							set_REL(30);
//							count=98;
//						}
//					}
						if(connection_status2 & MULTI_FAULT)
						{
							if(MULTI_FAULT_CONTINUE)
								set_next_cord_task_case = true;
							else count = 100;
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
						count=55;
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
						count=56;
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
						count=57;
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
						count=58;
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
						count=98;
					}
				}
				if(count!=98)
				{
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"N_PE",device.device_dir);
							meas_task_control |= __CORD_RES_REQUESTED;
						}
					}
				}
			}
			break;
		}
		//dodatno
		case 17://L2_L1 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=8;
					}
					else count = 100;
				}
				else
				{
					rst_REL(33);
					set_REL(30);
					count=8;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 18://L3_L1 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=20;
					}
					else count = 100;
				}
				else
				{
					rst_REL(33);
					set_REL(34);
					count=20;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 19://L3_L2 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=21;
					}
					else count = 100;
				}
				else
				{
					rst_REL(34);
					set_REL(33);
					count=21;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 20://L3_L2 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=12;
					}
					else count = 100;
				}
				else
				{
					rst_REL(33);
					set_REL(30);
					count=12;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 21://L3_L1 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=12;
					}
					else count = 100;
				}
				else
				{
					rst_REL(33);
					set_REL(30);
					count=12;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 22://N_L1 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=25;
					}
					else count = 100;
				}
				else
				{
					rst_REL(33);
					set_REL(34);
					count=25;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 23://N_L2 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=26;
					}
					else count = 100;
				}
				else
				{
					rst_REL(34);
					set_REL(33);
					count=26;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 24://N_L3 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=27;
					}
					else count = 100;
				}
				else
				{
					rst_REL(35);
					set_REL(33);
					count=27;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 25://N_L2 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=28;
					}
					else count = 100;
				}
				else
				{
					rst_REL(34);
					set_REL(35);
					count=28;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 26://N_L1 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=29;
					}
					else count = 100;
				}
				else
				{
					rst_REL(33);
					set_REL(35);
					count=29;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 27://N_L1 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=30;
					}
					else count = 100;
				}
				else
				{
					rst_REL(33);
					set_REL(34);
					count=30;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 28://N_L3 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=15;
					}
					else count = 100;
				}
				else
				{
					rst_REL(35);
					set_REL(30);
					count=15;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 29://N_L3 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=15;
					}
					else count = 100;
				}
				else
				{
					rst_REL(35);
					set_REL(30);
					count=15;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 30://N_L2 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=15;
					}
					else count = 100;
				}
				else
				{
					rst_REL(34);
					set_REL(30);
					count=15;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 31://PE_L1 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=32;
					}
					else count = 100;
				}
				else
				{
					rst_REL(33);
					set_REL(34);
					count=32;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 32://PE_L2 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=33;
					}
					else count = 100;
				}
				else
				{
					rst_REL(34);
					set_REL(35);
					count=33;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 33://PE_L3 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=34;
					}
					else count = 100;
				}
				else
				{
					rst_REL(35);
					set_REL(36);
					count=34;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 34://PE_N povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=98;
					}
					else count = 100;
				}
				else
				{
					rst_REL(36);
					set_REL(30);
					count=98;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 35://PE_L2 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=36;
					}
					else count = 100;
				}
				else
				{
					rst_REL(34);
					set_REL(33);
					count=36;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 36://PE_L1 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=37;
					}
					else count = 100;
				}
				else
				{
					rst_REL(33);
					set_REL(35);
					count=37;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 37://PE_L3 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=38;
					}
					else count = 100;
				}
				else
				{
					rst_REL(35);
					set_REL(36);
					count=38;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 38://PE_N povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=98;
					}
					else count = 100;
				}
				else
				{
					rst_REL(36);
					set_REL(30);
					count=98;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 39://PE_L3 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=40;
					}
					else count = 100;
				}
				else
				{
					rst_REL(35);
					set_REL(33);
					count=40;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 40://PE_L1 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=41;
					}
					else count = 100;
				}
				else
				{
					rst_REL(33);
					set_REL(34);
					count=41;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 41://PE_L2 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=42;
					}
					else count = 100;
				}
				else
				{
					rst_REL(34);
					set_REL(36);
					count=42;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 42://PE_N povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=98;
					}
					else count = 100;
				}
				else
				{
					rst_REL(36);
					set_REL(30);
					count=98;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 43://PE_N povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=44;
					}
					else count = 100;
				}
				else
				{
					rst_REL(36);
					set_REL(33);
					count=44;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 44://PE_L1 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=45;
					}
					else count = 100;
				}
				else
				{
					rst_REL(33);
					set_REL(34);
					count=45;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 45://PE_L2 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=46;
					}
					else count = 100;
				}
				else
				{
					rst_REL(34);
					set_REL(35);
					count=46;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 46://PE_L3 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
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
						count=98;
					}
					else count = 100;
				}
				else
				{
					rst_REL(35);
					set_REL(30);
					count=98;
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		///od tle naprej je nova koda
		case 47://L2_L1 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
				{
					if(connection_reg & L2_L2)
					{
						if((connection_status & ALL_SHORTED_MASKS))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= MULTI_SHORTED;
						}
//						connection_status |= L1_L2_SHORTED;
					}
					else
					{
						//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
						if(connection_reg & L2_CROSS_CON_MASK)
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
								rst_REL(33);
								set_REL(30);
								count=8;
						}
						else
							count =100;
					}
					else
					{
							rst_REL(33);
							set_REL(30);
							count=8;
					}
					connection_reg |= L2_L1;
				}
				else
				{
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							rst_REL(33);
							set_REL(30);
							count=8;
						}
						else count = 100;
					}
					else
					{
						rst_REL(33);
						set_REL(30);
						count=8;
					}
					connection_reg &= (~L2_L1);
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"L2_L1",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 48://L3_L2 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
				{
					if(connection_reg & L3_L3)
					{
						if((connection_status & ALL_SHORTED_MASKS))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= MULTI_SHORTED;
						}
//						connection_status |= L1_L2_SHORTED;
					}
					else
					{
						//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
						if(connection_reg & L3_CROSS_CON_MASK)
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
								rst_REL(34);
								set_REL(30);
								count=12;
						}
						else
							count =100;
					}
					else
					{
							rst_REL(34);
							set_REL(30);
							count=12;
					}
					connection_reg |= L3_L2;
				}
				else
				{
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							rst_REL(34);
							set_REL(30);
							count=12;
						}
						else count = 100;
					}
					else
					{
						rst_REL(34);
						set_REL(30);
						count=12;
					}
					connection_reg &= (~L3_L2);
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"L3_L2",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 49://L3_L1 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
				{
					if(connection_reg & L3_L3)
					{
						if((connection_status & ALL_SHORTED_MASKS))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= MULTI_SHORTED;
						}
					}
					else
					{
						//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
						if(connection_reg & L3_CROSS_CON_MASK)
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
								rst_REL(33);
								set_REL(30);
								count=12;
						}
						else
							count =100;
					}
					else
					{
							rst_REL(33);
							set_REL(30);
							count=12;
					}
					connection_reg |= L3_L1;
				}
				else
				{
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							rst_REL(33);
							set_REL(30);
							count=12;
						}
						else count = 100;
					}
					else
					{
						rst_REL(33);
						set_REL(30);
						count=12;
					}
					connection_reg &= (~L3_L1);
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"L3_L1",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 50://L3_L1 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
				{
					if(connection_reg & L3_L3)
					{
						if((connection_status & ALL_SHORTED_MASKS))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= MULTI_SHORTED;
						}
					}
					else
					{
						//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
						if(connection_reg & L3_CROSS_CON_MASK)
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
								rst_REL(33);
								set_REL(34);
								count=51;
						}
						else
							count =100;
					}
					else
					{
							rst_REL(33);
							set_REL(34);
							count=51;
					}
					connection_reg |= L3_L1;
				}
				else
				{
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							rst_REL(33);
							set_REL(34);
							count=51;
						}
						else count = 100;
					}
					else
					{
						rst_REL(33);
						set_REL(34);
						count=51;
					}
					connection_reg &= (~L3_L1);
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"L3_L1",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 51://L3_L2 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
				{
					if(connection_reg & L3_L3)
					{
						if((connection_status & ALL_SHORTED_MASKS)||(connection_reg & L3_L1))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= MULTI_SHORTED;
						}
					}
					else
					{
						//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
						if(connection_reg & L3_CROSS_CON_MASK)
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
								rst_REL(34);
								set_REL(30);
								count=12;
						}
						else
							count =100;
					}
					else
					{
							rst_REL(34);
							set_REL(30);
							count=12;
					}
					connection_reg |= L3_L2;
				}
				else
				{
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							rst_REL(34);
							set_REL(30);
							count=12;
						}
						else count = 100;
					}
					else
					{
						rst_REL(34);
						set_REL(30);
						count=12;
					}
					connection_reg &= (~L3_L2);
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"L3_L2",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 52://N_L1 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
				{
					if(connection_reg & N_N)
					{
						if((connection_status & ALL_SHORTED_MASKS))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= MULTI_SHORTED;
						}
					}
					else
					{
						//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
						if(connection_reg & N_CROSS_CON_MASK)
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
							set_next_cord_task_case = true;
						else
							count =100;
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
						else count = 100;
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
						count=15;
					}
					else
					{
						if(!((connection_reg & L2_L2)&&(connection_reg & L2_N))) //gremo na L2 - PE
						{
							rst_REL(33);
							set_REL(34);
							count=53;
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
							count=54;
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
							count=15;
						}
					}
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"N_L1",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 53://N_L2 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
				{
					if(connection_reg & N_N)
					{
						if((connection_status & ALL_SHORTED_MASKS)||(connection_reg & N_L1))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= MULTI_SHORTED;
						}
					}
					else
					{
						//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
						if(connection_reg & N_CROSS_CON_MASK)
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

								if(!((connection_reg & L3_L3)&&(connection_reg & L3_N))) //gremo na L2 - PE
								{
									rst_REL(33);
									set_REL(35);
									count=54;
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
									count=15;
								}
						}
						else
							count =100;
					}
					else
					{
						if(!((connection_reg & L3_L3)&&(connection_reg & L3_N))) //gremo na L2 - PE
								{
									rst_REL(33);
									set_REL(35);
									count=54;
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
									count=15;
								}
					}
					connection_reg |= N_L2;
				}
				else
				{
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							if(!((connection_reg & L3_L3)&&(connection_reg & L3_N))) //gremo na L2 - PE
								{
									rst_REL(34);
									set_REL(35);
									count=54;
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
									rst_REL(34);
									set_REL(30);
									count=15;
								}
						}
						else count = 100;
					}
					else
					{
							if(!((connection_reg & L3_L3)&&(connection_reg & L3_N))) //gremo na L2 - PE
								{
									rst_REL(34);
									set_REL(35);
									count=54;
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
									rst_REL(34);
									set_REL(30);
									count=15;
								}
					}
					connection_reg &= (~N_L2);
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"N_L2",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 54://N_L3 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
				{
					if(connection_reg & N_N)
					{
						if((connection_status & ALL_SHORTED_MASKS)||(connection_reg & N_L1))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= MULTI_SHORTED;
						}
					}
					else
					{
						//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
						if(connection_reg & N_CROSS_CON_MASK)
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
							rst_REL(35);
							set_REL(30);
							count=15;
						}
						else
							count =100;
					}
					else
					{
						rst_REL(35);
						set_REL(30);
						count=15;
					}
					connection_reg |= N_L3;
				}
				else
				{
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							rst_REL(35);
							set_REL(30);
							count=15;
						}
						else count = 100;
					}
					else
					{
						rst_REL(35);
						set_REL(30);
						count=15;
					}
					connection_reg &= (~N_L3);
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"N_L3",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 55://PE_L1 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
				{
					if(connection_reg & PE_PE)
					{
						if((connection_status & ALL_SHORTED_MASKS))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= MULTI_SHORTED;
						}
					}
					else
					{
						//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
						if(connection_reg & PE_CROSS_CON_MASK)
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
								if((!((connection_reg & L2_L2)&&(connection_reg & L2_PE)))&&(!(manual_set_phase_num && CORD_PHASE_NUM_SETTING==_1_PHASE))) //gremo na L2 - PE
								{
									rst_REL(33);
									set_REL(34);
									count=56;
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
									count=57;
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
									count=58;
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
										count=98;
									}
								}
						}
						else
							count =100;
					}
					else
					{
						if(!((connection_reg & L2_L2)&&(connection_reg & L2_PE))) //gremo na L2 - PE
								{
									rst_REL(33);
									set_REL(34);
									count=56;
								}
								else if(!((connection_reg & L3_L3)&&(connection_reg & L3_PE))) //gremo na L2 - PE
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
									count=57;
								}
								else if(!((connection_reg & N_N)&&(connection_reg & N_PE))) //gremo na L2 - PE
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
									rst_REL(33);
									set_REL(36);
									count=58;
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
									count=98;
								}
					}
					connection_reg |= PE_L1;
				}
				else
				{
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							if(!((connection_reg & L2_L2)&&(connection_reg & L2_PE))) //gremo na L2 - PE
								{
									rst_REL(33);
									set_REL(34);
									count=56;
								}
								else if(!((connection_reg & L3_L3)&&(connection_reg & L3_PE))) //gremo na L2 - PE
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
									count=57;
								}
								else if(!((connection_reg & N_N)&&(connection_reg & N_PE))) //gremo na L2 - PE
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
									rst_REL(33);
									set_REL(36);
									count=58;
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
									count=98;
								}
						}
						else count = 100;
					}
					else
					{
							if(!((connection_reg & L2_L2)&&(connection_reg & L2_PE))) //gremo na L2 - PE
								{
									rst_REL(33);
									set_REL(34);
									count=56;
								}
								else if(!((connection_reg & L3_L3)&&(connection_reg & L3_PE))) //gremo na L2 - PE
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
									count=57;
								}
								else if(!((connection_reg & N_N)&&(connection_reg & N_PE))) //gremo na L2 - PE
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
									rst_REL(33);
									set_REL(36);
									count=58;
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
									count=98;
								}
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
						count=56;
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
						count=57;
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
						count=58;
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
							count=98;
						}
					}
				}
				if(count!=98)
				{
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"PE_L1",device.device_dir);
							meas_task_control |= __CORD_RES_REQUESTED;
						}
					}
				}
			}
			break;
		}
		case 56://PE_L2 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
				{
					if(connection_reg & PE_PE)
					{
						if((connection_status & ALL_SHORTED_MASKS)||(connection_reg & PE_L1))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= MULTI_SHORTED;
						}
					}
					else
					{
						//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
						if(connection_reg & PE_CROSS_CON_MASK)
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
								if(!((connection_reg & L3_L3)&&(connection_reg & L3_PE))) //gremo na L2 - PE
								{
									rst_REL(34);
									set_REL(35);
									count=57;
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
									count=58;
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
									count=98;
								}
						}
						else
							count =100;
					}
					else
					{
						if(!((connection_reg & L3_L3)&&(connection_reg & L3_PE))) //gremo na L2 - PE
								{
									rst_REL(34);
									set_REL(35);
									count=57;
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
									count=58;
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
									count=98;
								}
					}
					connection_reg |= PE_L2;
				}
				else
				{
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							if(!((connection_reg & L3_L3)&&(connection_reg & L3_PE))) //gremo na L2 - PE
								{
									rst_REL(34);
									set_REL(35);
									count=57;
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
									count=58;
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
									count=98;
								}
						}
						else count = 100;
					}
					else
					{
							if(!((connection_reg & L3_L3)&&(connection_reg & L3_PE))) //gremo na L2 - PE
								{
									rst_REL(34);
									set_REL(35);
									count=57;
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
									count=58;
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
									count=98;
								}
					}
					connection_reg &= (~PE_L2);
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"PE_L2",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 57://PE_L3 povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
				{
					if(connection_reg & PE_PE)
					{
						if((connection_status & ALL_SHORTED_MASKS)||(connection_reg & (PE_L1 | PE_L2)))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= MULTI_SHORTED;
						}
					}
					else
					{
						//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
						if(connection_reg & PE_CROSS_CON_MASK)
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
								if(!((connection_reg & N_N)&&(connection_reg & N_PE))) //gremo na L2 - PE
								{
									rst_REL(35);
									set_REL(36);
									count=58;
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
									count=98;
								}
						}
						else
							count =100;
					}
					else
					{
						if(!((connection_reg & N_N)&&(connection_reg & N_PE))) //gremo na L2 - PE
								{
									rst_REL(35);
									set_REL(36);
									count=58;
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
									count=98;
								}
					}
					connection_reg |= PE_L3;
				}
				else
				{
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							if(!((connection_reg & N_N)&&(connection_reg & N_PE))) //gremo na L2 - PE
								{
									rst_REL(35);
									set_REL(36);
									count=58;
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
									count=98;
								}
						}
						else count = 100;
					}
					else
					{
							if(!((connection_reg & N_N)&&(connection_reg & N_PE))) //gremo na L2 - PE
								{
									rst_REL(35);
									set_REL(36);
									count=58;
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
									count=98;
								}
					}
					connection_reg &= (~PE_L3);
				}
				if(connection_status2 & MULTI_FAULT)
				{
					if(MULTI_FAULT_CONTINUE)
					{
						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"PE_L3",device.device_dir);
						meas_task_control |= __CORD_RES_REQUESTED;
					}
				}
			}
			break;
		}
		case 58://PE_N povezano?
		{
			if(!(meas_task_control & __CORD_RES_REQUESTED))
			{
				if(cord_check_resistance())
				{
					if(connection_reg & PE_PE)
					{
						if((connection_status & ALL_SHORTED_MASKS)||(connection_reg & (PE_L1 | PE_L2 | PE_L3)))//preveri ce je ze katera prej kratko sklenjena, ker to privede do multifault
						{
							connection_status2 |= MULTI_FAULT;
							connection_status2 |= MULTI_SHORTED;
						}
					}
					else
					{
						//preverimo ce je ze katera bila pred tem prekrizana, kar povzroci MULTI_FAULT
						if(connection_reg & PE_CROSS_CON_MASK)
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
									rst_REL(36);
									set_REL(30);
									count=98;
						}
						else
							count =100;
					}
					else
					{
						rst_REL(36);
						set_REL(30);
						count=98;
					}
					connection_reg |= PE_N;
				}
				else
				{
					if(connection_status2 & MULTI_FAULT)
					{
						if(MULTI_FAULT_CONTINUE)
						{
							rst_REL(36);
							set_REL(30);
							count=98;
						}
						else count = 100;
					}
					else
					{
							rst_REL(36);
							set_REL(30);
							count=98;
					}
					connection_reg &= (~PE_N);
				}
//				if(connection_status2 & MULTI_FAULT)
//				{
//					if(MULTI_FAULT_CONTINUE)
//					{
//						SendComMessage(_ON,_ID_TFA,device.device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"PE_N",device.device_dir);
//						meas_task_control |= __CORD_RES_REQUESTED;
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
						switch (connection_reg & (PE_CROSSED_MASK | PE_L1 | PE_L2 | PE_L3 | PE_N))
						{
						  case PE_L1: connection_status2 |= PE_L1_CROSSED; break;	
							case PE_L2: connection_status2 |= PE_L2_CROSSED; break;
							case PE_L3: connection_status2 |= PE_L3_CROSSED; break;	
							case PE_N: connection_status2 |= PE_N_CROSSED; break;
							case 0: 
								if(connection_status & (L1_OPEN | L2_OPEN | L3_OPEN | N_OPEN))
								{
									connection_status2 |= MULTI_FAULT;
									connection_status2 |= MULTI_OPENED;
								}
								connection_status |= PE_OPEN; 
								break;	//vse ok
							default: 
								connection_status2 |= MULTI_FAULT;	//prekrizana z vecimi
								connection_status2 |= CROSSED_AND_SHORTED;
						}
					}
				}
				//ce ni crossed in shorted potem pomeni, da je OK
				else if((connection_reg & PE_PE)&(!(connection_reg & (PE_CROSSED_MASK|PE_CROSSED_MASK2))))
				{
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
			count = 99;
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
	if(count != 100)
		restart_timer(CORD_MEAS_NORMAL,5,cord_meas_normal);
}
void stop_cord(void)
{
	meas_task_control &= (~__CORD_MEAS_IN_PROG);
	rst_REL(6);
}
void set_cord_init(void)
{
	connection_reg =0;
	if(connection_control & __CON_TO_MT310)
	{
		rst_REL(2);
		rst_REL(3);
		rst_REL(4);
		rst_REL(5);
		rst_REL(8);
		rst_REL(30);
		set_REL(6);
		set_REL(9);
		set_REL(27);
		set_REL(28);
	}
}
void set_cord_resistance(char* value)
{
	cord_rpe_resistance = (float)atof(value);
	meas_task_control &= (~__CORD_RES_REQUESTED);
}
bool cord_check_resistance(void)
{
	if(cord_rpe_resistance > CORD_RPE_LIMIT)
		return false;
	else
		return true;
}
void transmitt_cable_state(uint32_t dir, char device_ID)
{
	uint32_t msg_count=0;
	char temp_str[MAX_ADDITIONAL_COMMANDS_LENGTH];
		//+++++++++++++++++VRNEMO REZULTAT ZA ENOFAZNI KABEL+++++++++++++++++++++
	if(((connection_status & (~CABLE_1P_MASK))&&(!(manual_set_phase_num)))||//pogledamo ce gre za enofazni kabel
				(manual_set_phase_num && CORD_PHASE_NUM_SETTING == _1_PHASE))
	{
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
			if(connection_status2 & L1_OPEN)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L1_OPEN__);
					}
			}
			if(connection_status2 & L1_N_CROSSED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L1_N_CROSSED__);
					}
			}
			if(connection_status2 & L1_N_SHORTED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L1_N_SHORTED__);
					}
			}
			if(connection_status2 & L1_PE_CROSSED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L1_PE_CROSSED__);
					}
			}
			if(connection_status2 & L1_PE_SHORTED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L1_PE_SHORTED__);
					}
			}
			if(connection_status2 & N_OPEN)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__N_OPEN__);
					}
			}
			if(connection_status2 & PE_OPEN)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__PE_OPEN__);
					}
			}
			if(connection_status2 & N_PE_CROSSED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__PE_N_CROSSED__);
					}
			}
			if(connection_status2 & N_PE_SHORTED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__N_PE_SHORTED__);
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
										strcat(temp_str,__PASS_1P__);	//ce ni nic od prej nastetega potem je to pass
									}
									else
									{
										strcat(temp_str,__FAIL_1P__);
									}
									break;
							}
							break;
					}
					break;
			}
		}
	}
	//+++++++++++++++++VRNEMO REZULTAT ZA TROFAZNI KABEL+++++++++++++++++++++
	else
	{
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
			if(connection_status2 & L1_OPEN)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L1_OPEN__);
					}
			}
			if(connection_status2 & L1_L2_CROSSED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L1_L2_CROSSED__);
					}
			}
			if(connection_status2 & L1_L2_SHORTED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L1_L2_SHORTED__);
					}
			}
			if(connection_status2 & L1_L3_CROSSED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L1_L3_CROSSED__);
					}
				}
			if(connection_status2 & L1_L3_SHORTED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L1_L3_SHORTED__);
					}
			}
			if(connection_status2 & L1_N_CROSSED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L1_N_CROSSED__);
					}
			}
			if(connection_status2 & L1_N_SHORTED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L1_N_SHORTED__);
					}
			}
			if(connection_status2 & L1_PE_CROSSED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L1_PE_CROSSED__);
					}
			}
			if(connection_status2 & L1_PE_SHORTED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L1_PE_SHORTED__);
					}
			}
			if(connection_status2 & L2_L3_CROSSED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L2_L3_CROSSED__);
					}
			}
			if(connection_status2 & L2_L3_SHORTED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L2_L3_SHORTED__);
					}
			}
			if(connection_status2 & L2_N_CROSSED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L2_N_CROSSED__);
					}
			}
			if(connection_status2 & L2_N_SHORTED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L2_N_SHORTED__);
					}
			}
			if(connection_status2 & L2_PE_CROSSED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L1_PE_CROSSED__);
					}
			}
			if(connection_status2 & L2_PE_SHORTED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L2_PE_SHORTED__);
					}
			}
			if(connection_status2 &  L3_N_CROSSED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L3_N_CROSSED__);
					}
			}
			if(connection_status2 & L3_N_SHORTED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L3_N_SHORTED__);
					}
			}
			if(connection_status2 & L3_PE_CROSSED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L3_PE_CROSSED__);
					}
			}
			if(connection_status2 & L3_PE_SHORTED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L3_PE_SHORTED__);
					}
			}
			if(connection_status2 & L2_OPEN)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L2_OPEN__);
					}
			}
			if(connection_status2 & L3_OPEN)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__L3_OPEN__);
					}
			}
			if(connection_status2 & N_OPEN)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__N_OPEN__);
					}
			}
			if(connection_status2 & PE_OPEN)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__PE_OPEN__);
					}
			}
			if(connection_status2 & N_PE_CROSSED)
			{
					msg_count++;
					if(msg_count<MAX_ERROR_ADD_COMMAND)
					{
						strcat(temp_str,",");
						strcat(temp_str,__PE_N_CROSSED__);
					}
			}
			if(connection_status2 & N_PE_SHORTED)
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
									strcat(temp_str,__PASS__);	//ce ni nic od prej nastetega potem je to pass
									break;
							}
							break;
					}
					break;
			}
		}
	}
	SendComMessage(_ON,_ID_TFA,device_ID,__MT_300__,__CORD__,__GET_RPE_RES__,"PE_N",dir);
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

