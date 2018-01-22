
#include "com_meas_tasks.h"
#include "defines.h"
#include "rel_driver.h"
#include "stdint.h"
#include "defines.h"
#include "serial_com.h"
#include "do_task.h"
#include "sdadc.h"
#include <stdio.h>

extern uint32_t global_control;
extern uint32_t meas_control;
uint32_t current_URES_measurement=0;
static struct connected_device device;
extern uint32_t meas_task_control;

static void returnMeasueredCurrent(void);
static void returnMeasueredVoltage(void);
static void returnMeasueredTHD_C(void);
static void returnMeasueredTHD_V(void);
static void returnMeasueredPowerS(void);
static void returnMeasueredPowerR(void);
static void returnMeasueredPF(void);


//funkcijo je potrebno klicati po power_on_testu
void setNormal(void)
{
	//nastavljanje relejev na zacetno vrednost
	RST_L1_CONTACTOR;
	RST_L2_CONTACTOR;
	RST_L3_CONTACTOR;
	RST_N_CONTACTOR;

	rst_REL(7);
	rst_REL(8);
	rst_REL(9);
	rst_REL(10);
	rst_REL(11);
	rst_REL(12);
	rst_REL(13);
	rst_REL(14);
	rst_REL(15);
	rst_REL(16);
	rst_REL(17);
	rst_REL(18);
	rst_REL(19);
	rst_REL(21);//*
	rst_REL(22);
	rst_REL(23);
	rst_REL(24);
	rst_REL(25);
	rst_REL(26);
	rst_REL(27);
	rst_REL(28);
	rst_REL(29);
	rst_REL(30);
	rst_REL(33);
	rst_REL(34);
	rst_REL(35);
	rst_REL(36);
	if(global_control & __INIT_TEST_PASS)
	{
		set_REL(37);
		set_REL(1);
		SET_PE_CONTACTOR;//RST_PE_CONTACTOR;//PE prikljucen v normalnih pogojih
		//obrnemo faze za enopolne vticnice v primeru da gre za enofazni sistem 
		if((global_control & __1P_CONNECTION)&&(global_control & __L_ON_N))
			set_REL(21);
	}
	else
	{
		rst_REL(37);	
		rst_REL(1);
		rst_REL(21);
		RST_PE_CONTACTOR;//SET_PE_CONTACTOR;			//ce meritev ne gre skoz odklopimo rele
	}
	rst_REL(39);
	rst_REL(40);
	rst_REL(41);
	rst_REL(42);
}

void WeldMachMt310_RelInit(void)
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
		SET_PE_CONTACTOR;//RST_PE_CONTACTOR;		//PE kontaktor ima NC kontakt, zato ga izklopimo
}

void startMainsMeasurement(void)
{
	meas_control = 0;
	synchroSetContactor(__SET_L1_CONTACTOR);
	synchroSetContactor(__SET_L2_CONTACTOR);
	synchroSetContactor(__SET_L3_CONTACTOR);
	synchroSetContactor(__SET_N_CONTACTOR);
	synchroSetContactor(__SET_PE_CONTACTOR);
	start_measure();
	device = get_connected_device();
	if(meas_task_control & __MACH_MEAS_IN_PROG)
		SendComMessage(_ON,_ID_TFA,device.device_ID,__MACHINES__,__ANALYZE_STARTED__,"","",device.device_dir);
	else if(meas_task_control & __CORD_MEAS_IN_PROG)
		SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__ANALYZE_STARTED__,"","",device.device_dir);
	else if(meas_task_control & __WELD_MEAS_IN_PROG)
		SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__ANALYZE_STARTED__,"","",device.device_dir);
}
void returnMeasuredTask(void)
{
	if(meas_task_control & __RETURN_CURRENT)
	{
		meas_task_control &= ~__RETURN_CURRENT;
		returnMeasueredCurrent();
	}
	if(meas_task_control & __RETURN_VOLTAGE)
	{
		meas_task_control &= ~__RETURN_VOLTAGE;
		returnMeasueredVoltage();
	}
	if(meas_task_control & __RETURN_THD_C)
	{
		meas_task_control &= ~__RETURN_THD_C;
		returnMeasueredTHD_C();
	}
	if(meas_task_control & __RETURN_THD_V)
	{
		meas_task_control &= ~__RETURN_THD_V;
		returnMeasueredTHD_V();
	}
	if(meas_task_control & __RETURN_POWER_R)
	{
		meas_task_control &= ~__RETURN_POWER_R;
		returnMeasueredPowerR();
	}
	if(meas_task_control & __RETURN_POWER_A)
	{
		meas_task_control &= ~__RETURN_POWER_A;
		returnMeasueredPowerS();
	}
	if(meas_task_control & __RETURN_PF)
	{
		meas_task_control &= ~__RETURN_PF;
		returnMeasueredPF();
	}
}
static void returnMeasueredCurrent(void)
{
	float IL1 = get_value(__IL1);
	float IL2 = get_value(__IL2);
	float IL3 = get_value(__IL3);
	char temp_buff[50];
	snprintf(temp_buff,50,"IL1|%.3f,IL2|%.3f,IL3|%.3f",IL1,IL2,IL3);
	if(meas_task_control & __MACH_MEAS_IN_PROG)
		SendComMessage(_ON,_ID_TFA,device.device_ID,__MACHINES__,__RETURN_CURRENT__,temp_buff,"",device.device_dir);
	else if(meas_task_control & __CORD_MEAS_IN_PROG)
		SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__RETURN_CURRENT__,temp_buff,"",device.device_dir);
	else if(meas_task_control & __WELD_MEAS_IN_PROG)
		SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__RETURN_CURRENT__,temp_buff,"",device.device_dir);
}
static void returnMeasueredVoltage(void)
{
	float ULN1 = get_value(__ULN1);
	float ULN2 = get_value(__ULN2);
	float ULN3 = get_value(__ULN3);
	float UNPE = get_value(__UNPE);
	char temp_buff[50];
	snprintf(temp_buff,50,"ULN1|%.3f,ULN2|%.3f,ULN3|%.3f,UNPE|%.3f",ULN1,ULN2,ULN3,UNPE);
	if(meas_task_control & __MACH_MEAS_IN_PROG)
		SendComMessage(_ON,_ID_TFA,device.device_ID,__MACHINES__,__RETURN_VOLTAGE__,temp_buff,"krneki",device.device_dir);
	else if(meas_task_control & __CORD_MEAS_IN_PROG)
		SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__RETURN_VOLTAGE__,temp_buff,"krneki",device.device_dir);
	else if(meas_task_control & __WELD_MEAS_IN_PROG)
		SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__RETURN_VOLTAGE__,temp_buff,"krneki",device.device_dir);//"0|0,0|0,0|0,0|0"
}
static void returnMeasueredTHD_V(void)
{
	float T_ULN1 = get_value(__T_ULN1);
	float T_ULN2 = get_value(__T_ULN2);
	float T_ULN3 = get_value(__T_ULN3);
	char temp_buff[50];
	snprintf(temp_buff,50,"T_ULN1|%.3f,T_ULN2|%.3f,T_ULN3|%.3f",T_ULN1,T_ULN2,T_ULN3);
	if(meas_task_control & __MACH_MEAS_IN_PROG)
		SendComMessage(_ON,_ID_TFA,device.device_ID,__MACHINES__,__RETURN_THD_V__,temp_buff,"",device.device_dir);
	else if(meas_task_control & __CORD_MEAS_IN_PROG)
		SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__RETURN_THD_V__,temp_buff,"",device.device_dir);
	else if(meas_task_control & __WELD_MEAS_IN_PROG)
		SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__RETURN_THD_V__,temp_buff,"",device.device_dir);
}
static void returnMeasueredTHD_C(void)
{
	float T_IL1 = get_value(__T_IL1);
	float T_IL2 = get_value(__T_IL2);
	float T_IL3 = get_value(__T_IL3);
	char temp_buff[50];
	snprintf(temp_buff,50,"T_IL1|%.3f,T_IL2|%.3f,T_IL3|%.3f",T_IL1,T_IL2,T_IL3);
	if(meas_task_control & __MACH_MEAS_IN_PROG)
		SendComMessage(_ON,_ID_TFA,device.device_ID,__MACHINES__,__RETURN_THD_C__,temp_buff,"",device.device_dir);
	else if(meas_task_control & __CORD_MEAS_IN_PROG)
		SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__RETURN_THD_C__,temp_buff,"",device.device_dir);
	else if(meas_task_control & __WELD_MEAS_IN_PROG)
		SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__RETURN_THD_C__,temp_buff,"",device.device_dir);
}
static void returnMeasueredPowerR(void)
{
	float PL1 = get_value(__PL1);
	float PL2 = get_value(__PL2);
	float PL3 = get_value(__PL3);
	char temp_buff[50];
	snprintf(temp_buff,50,"PL1|%.3f,PL2|%.3f,PL3|%.3f",PL1,PL2,PL3);
	if(meas_task_control & __MACH_MEAS_IN_PROG)
		SendComMessage(_ON,_ID_TFA,device.device_ID,__MACHINES__,__RETURN_POWER_R__,temp_buff,"",device.device_dir);
	else if(meas_task_control & __CORD_MEAS_IN_PROG)
		SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__RETURN_POWER_R__,temp_buff,"",device.device_dir);
	else if(meas_task_control & __WELD_MEAS_IN_PROG)
		SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__RETURN_POWER_R__,temp_buff,"",device.device_dir);
}
static void returnMeasueredPowerS(void)
{
	float SL1 = get_value(__SL1);
	float SL2 = get_value(__SL2);
	float SL3 = get_value(__SL3);
	char temp_buff[50];
	snprintf(temp_buff,50,"SL1|%.3f,SL2|%.3f,SL3|%.3f",SL1,SL2,SL3);
	if(meas_task_control & __MACH_MEAS_IN_PROG)
		SendComMessage(_ON,_ID_TFA,device.device_ID,__MACHINES__,__RETURN_POWER_A__,temp_buff,"",device.device_dir);
	else if(meas_task_control & __CORD_MEAS_IN_PROG)
		SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__RETURN_POWER_A__,temp_buff,"",device.device_dir);
	else if(meas_task_control & __WELD_MEAS_IN_PROG)
		SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__RETURN_POWER_A__,temp_buff,"",device.device_dir);
}
static void returnMeasueredPF(void)
{
	float PF1 = get_value(__PF1);
	float PF2 = get_value(__PF2);
	float PF3 = get_value(__PF3);
	char temp_buff[50];
	snprintf(temp_buff,50,"PF1|%.3f,PF2|%.3f,PF3|%.3f",PF1,PF2,PF3);
	if(meas_task_control & __MACH_MEAS_IN_PROG)
		SendComMessage(_ON,_ID_TFA,device.device_ID,__MACHINES__,__RETURN_PF__,temp_buff,"",device.device_dir);
	else if(meas_task_control & __CORD_MEAS_IN_PROG)
		SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__RETURN_PF__,temp_buff,"",device.device_dir);
	else if(meas_task_control & __WELD_MEAS_IN_PROG)
		SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__RETURN_PF__,temp_buff,"",device.device_dir);
}
void stopMainsMeasurement(void)
{
	stop_measure();
	meas_task_control &= ~(__TASK_RETURN_MASKS);
	if(meas_task_control & __MACH_MEAS_IN_PROG)
		SendComMessage(_ON,_ID_TFA,device.device_ID,__MACHINES__,__ANALYZE_STOPPED__,"","",device.device_dir);
	else if(meas_task_control & __CORD_MEAS_IN_PROG)
		SendComMessage(_ON,_ID_TFA,device.device_ID,__CORD__,__ANALYZE_STOPPED__,"","",device.device_dir);
	else if(meas_task_control & __WELD_MEAS_IN_PROG)
		SendComMessage(_ON,_ID_TFA,device.device_ID,__WELDING__,__ANALYZE_STOPPED__,"","",device.device_dir);
}

