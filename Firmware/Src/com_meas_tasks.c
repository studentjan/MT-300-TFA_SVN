
#include "com_meas_tasks.h"
#include "defines.h"
#include "rel_driver.h"
#include "stdint.h"
#include "defines.h"
#include "serial_com.h"
#include "do_task.h"
#include "sdadc.h"

extern uint32_t global_control;
extern uint32_t meas_control;
uint32_t current_URES_measurement=0;


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
	start_measure();
	
}
void stopMainsMeasurement(void)
{
	stop_measure();
}
void resultTransmittTask(void)
{
	
}
