  //******************************************************************************
  //* File Name          : comunication.c
  //* Description        : Comunication initialization
  //* Created			 : Jan Koprivec
  //* Date				 : 13.7.2017
  //******************************************************************************
  
 #include "comunication.h"
// #include "stm32f3xx_hal_spi.h"
// #include "stm32f3xx_hal_uart.h"
 #include <string.h>
 #include "usbd_cdc_if.h"
 #include "tasks.h"
 #include "os.h"
 #include "defines.h"
 #include "test.h"
 #include "sdadc.h"
 #include "serial_com.h"
 #include "usb_device.h"
 #include "cord.h"
 #include "machines.h"
 #include "calibration.h"
 #include "com_meas_tasks.h"
 #include "do_task.h"
 #include "rel_driver.h"
 #include "welding.h"

extern uint32_t connection_control;
extern uint8_t ser_ID_sender;
extern char transmitter_ID;
extern uint32_t global_control;
 SPI_HandleTypeDef hspi1;
 UART_HandleTypeDef huart3;
 uint32_t meas_control = 0;
extern uint32_t connection_control;
extern USBD_HandleTypeDef hUsbDeviceFS;
static struct connected_device device;
extern uint32_t result_transmitt_control;

bool dtr_pin = false;
bool temp_ind=false;

//------------command analyze--------------------------------------
//------------command analyze--------------------------------------
//------------------------------------KOSNTANSTE--------------------------------------
//																									0				1				2					3						4				5					6			7				8				9				10				11				12					13					14					15				16
const char FUNCTION_COMMUNICATON_NAMES[20][13] = {"RPE", "C_W" , "CONT", "CABLE_TYPE","ALL-PE","ONE-PE","PH-PH","RISO","EVENT","M_RPE","M_URES","POWER","MAINS-WELD","MAINS-CLASS2","WELD-PE","UNL_RMS","UNL_PEAK"};
//																									0				1					2				3			4			5				6
const char COMMAND_COMMUNICATON_NAMES[10][10] = {"START", "STOP" , "CONT", "SET", "GET","OPEN","INIT"};
//																									0					1					2					3					4
const char COMMAND_COMMUNICATON_NAMES2[10][10] = {"VALUE","RESULT","INITIATED","OPENED","STARTED"};
//																								0						1				2				3				4					5				6
const char POWER_COMMUNICATON_NAMES[10][10] = {"VOLTAGE","CURRENT","THD_C","THD_V","POWER_R","POWER_A","PF"};
//																							0			1			2				3				4
const char ADD_COMMUNICATON_NAMES2[10][10] = {"0.2", "10", "25","RESULT","500"};
//
const char STD_COMMUNICATION_NAMES[10][20]={"CORD","WELD","MACH"};
//																									0
const char EVENT_COMMUNICATON_NAMES[10][20] = {"STOPPED"};
//-----------ZUNANJE SPREMENLJIVKE-----------------------
extern uint8_t event_status; 
extern uint32_t connection_control;
extern uint32_t meas_task_control;
extern uint32_t current_URES_measurement;
extern USBD_HandleTypeDef hUsbDeviceFS;
extern uint32_t start_mach_count;
extern uint32_t mach_RISO_count;
extern uint32_t mach_task_control;
extern int start_cord_count;
extern uint32_t cord_RISO_count;
extern uint32_t cord_task_control;
extern uint32_t start_weld_count;
extern uint32_t weld_RISO_count;
extern uint32_t weld_task_control;
extern uint32_t meas_control;
extern uint32_t calib_task_control;
extern uint32_t cord_cw_count;
extern uint32_t cord_continuity_count;

extern char m_start_tag[5];
extern char m_msg_ID[5];
extern char m_function[50];
extern char m_command[50];
extern char m_additional_code[150];
extern char m_value[50];
extern char m_is_crc[5];
extern char m_crc_value[5];
extern char m_leftover[10];
extern char additionalCode[2][MAX_ADDITIONAL_COMMANDS][MAX_ADDITIONAL_COMMANDS_LENGTH]; //0 so funkcije 1 pa vrednosti : http://stackoverflow.com/questions/19863417/how-to-declare-and-initialize-in-a-4-dimensional-array-in-c
//-----------------------------------------------

 /* USART3 init function */
void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 38400;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_RS485Ex_Init(&huart3, UART_DE_POLARITY_HIGH, 0, 0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}

/* SPI1 init function */
void MX_SPI1_Init(void)
{

  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

void SPI1_Send_data(uint8_t* data)
{	
	HAL_SPI_Transmit(&hspi1,data,1,0);
}
void USBConnected_Handler(void)
{	
	static char usb_connected=_NO;
	if(HAL_GPIO_ReadPin(USB_VBUS_GPIO_Port, USB_VBUS_Pin))
	{
		usb_connected=_YES;
		connection_control |= __USB_IN_SOCKET;
		USB_PU_ON; //VKLOPI PU na USB liniji (na racunalniku mi z usb B konektorjem brez tega ni delal)
		//serial_com_init();
		//dtr_pin = true;
		connection_control |= __USB_PLUG_CHANGED;
		restart_timer(USB_CHECK_TASK,5,usbCheckTask);
	}
	else 
	{
		//povezava ni vec vzpostavljena (lahko je vzpostavljena samo z simulacijo, sepravi racunalnikom in PATOM preko USB-ja
//		if(connection_control & __ESTABLISHED_TO_SIM_USB)
//			disconnect_function(__ESTABLISHED_TO_SIM_USB);
//		else if(connection_control & __ESTABLISHED_TO_PAT_USB)
//			disconnect_function(__ESTABLISHED_TO_PAT_USB);
		usb_connected = _NO;
		connection_control &= ~__USB_IN_SOCKET;
		connection_control |= __USB_PLUG_CHANGED;
		restart_timer(USB_CHECK_TASK,5,usbCheckTask);
		//dtr_pin = false;
		//dtr_pin = false;
		USB_PU_OFF; //VKLOPI PU na USB liniji (na racunalniku mi z usb B konektorjem brez tega ni delal)
	}
		//set_timer(WELCOME_MSG,30,Welcome_msg);
}
void Welcome_msg(void)
{
	uint8_t	str_out[] = "JES SM TRIFAZNI ADAPTER";
	CDC_Transmit_FS(str_out, ((sizeof str_out)));
}
void Transmit_results_task(void)	
{	
	if(meas_task_control & __POWER_MEAS_IN_PROG)
	{
		if(numberOfSetBits(result_transmitt_control & TRANSMMITT_VOLTAGE_READY_MASKS)>=5)
		{
			meas_task_control |= __RETURN_VOLTAGE;
			result_transmitt_control &= ~TRANSMMITT_VOLTAGE_READY_MASKS;
		}
		if(numberOfSetBits(result_transmitt_control & TRANSMMITT_CURRENT_READY_MASKS)>=4)
		{
			meas_task_control |= __RETURN_CURRENT;
			result_transmitt_control &= ~TRANSMMITT_CURRENT_READY_MASKS;
		}
		if(numberOfSetBits(result_transmitt_control & TRANSMMITT_THD_I_READY_MASKS)>=3)
		{
			meas_task_control |= __RETURN_THD_C;
			result_transmitt_control &= ~TRANSMMITT_THD_I_READY_MASKS;
		}
		if(numberOfSetBits(result_transmitt_control & TRANSMMITT_THD_U_READY_MASKS)>=3)
		{
			meas_task_control |= __RETURN_THD_V;
			result_transmitt_control &= ~TRANSMMITT_THD_U_READY_MASKS;
		}
		if(numberOfSetBits(result_transmitt_control & TRANSMMITT_POWER_R_READY_MASKS)>=3)
		{
			meas_task_control |= __RETURN_POWER_R;
			result_transmitt_control &= ~TRANSMMITT_POWER_R_READY_MASKS;
		}
		if(numberOfSetBits(result_transmitt_control & TRANSMMITT_POWER_A_READY_MASKS)>=3)
		{
			meas_task_control |= __RETURN_POWER_A;
			result_transmitt_control &= ~TRANSMMITT_POWER_A_READY_MASKS;
		}
		if(numberOfSetBits(result_transmitt_control & TRANSMMITT_PF_READY_MASKS)>=3)
		{
			meas_task_control |= __RETURN_PF;
			result_transmitt_control &= ~TRANSMMITT_PF_READY_MASKS;
		}
		restart_timer(RETURN_MEASURED_RESULT,1,returnMeasuredTask);
		restart_timer(TRANSMIT_RESULTS,TRANSMITT_RESULT_TIME,Transmit_results_task);	
	}
}
//ce je kaj narobe z vhodno povezavo ne pustimo izvajanja ukazov in vrnemo string, ki pove kaj je narobe
void send_warning_MSG(void)
{
	uint8_t direction;
	if(global_control & __INIT_TEST_FAIL)
	{
		if(transmitter_ID==_ID_MT) direction=_UART_DIR_485;
		else if(transmitter_ID==_ID_PAT) direction=_UART_DIR_USB;
		else if(transmitter_ID==_ID_DEBUG) direction=_UART_DIR_USB;
		if(global_control & __PE_DISCONNECTED) SendComMessage(_ON,_ID_TFA,0,__WARNING__,"PE DISCONNECTED","","",direction);
		else if(global_control & __N_DISCONECTED) SendComMessage(_ON,_ID_TFA,0,__WARNING__,"N DISCONNECTED","","",direction);
		else if(global_control & __WRONG_CONNECTION) SendComMessage(_ON,_ID_TFA,0,__WARNING__,"WRONG CONNECTION","","",direction);
	}
}

//v intervalu preverjamo vzpostavljeno komunikacijo tako, da nam naprava vraca --se ne uporablja
void check_connection(void)
{
	if(connection_control & __CON_TO_MT310)
	{
		//ce odgovor ne pobrise zastavice povezava ni vec vzpostavljena
		if(connection_control & __CHECK_TO_MT310)
		{
			disconnect_function(__CON_TO_MT310);
		}
		else
		{
			connection_control |= __CHECK_TO_MT310;
			SendComMessage(_ON,_ID_TFA,_ID_MT,__MT_300__,__CONNECTION__,__CHECK_CONNECTION__,"",_UART_DIR_USB);
		}
	}
	if(connection_control & __CONNECTION_ESTABLISHED)
	{
		set_timer(CHECK_CONNECTION,CONNECTION_CHECK_INTERVAL,check_connection);
	}
	else
	{
		connection_control &= (~__CHECK_TO_MT310);
		connection_control &= (~__CHECK_TO_TERMINAL);
		connection_control &= (~__CHECK_TO_PAT);
	}
}

void disconnect_function(uint32_t temp_connection_control)
{
	if(temp_connection_control & __ESTABLISHED_TO_SIM_USB)
	{	
		if(temp_connection_control & __CON_TO_MT310)
			connection_control &= (~__CON_TO_MT310);//zaenkrat je mt310 simuliran v racunalniku, zato je povezava preko USB, kasneje bo RS 485
		else if(temp_connection_control & __CON_TO_PAT)
			connection_control &= (~__CON_TO_PAT);
		connection_control &= (~__ESTABLISHED_TO_SIM_USB);
	}
	else if(temp_connection_control & __ESTABLISHED_TO_PAT_USB)
	{	
		if(temp_connection_control & __CON_TO_MT310)
			connection_control &= (~__CON_TO_MT310);//zaenkrat je mt310 simuliran v racunalniku, zato je povezava preko USB, kasneje bo RS 485
		else if(temp_connection_control & __CON_TO_PAT)
			connection_control &= (~__CON_TO_PAT);
		connection_control &= (~__ESTABLISHED_TO_PAT_USB);
	}
	if(!(connection_control & (__ESTABLISHED_TO_PAT_USB|__ESTABLISHED_TO_MT310|__ESTABLISHED_TO_SIM_USB)))//preverimo se ce je vzpostavljena kaksna druga povezava cene zapremo komunikacijo
	{
		connection_control &= (~__CONNECTION_ESTABLISHED);
		serial_com_deinit();
	}
	
}
void checkUSBconnected(void)
{
	if((hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED) && (dtr_pin==true))
	{   
		if(!(connection_control & __CONNECTION_ESTABLISHED))
		{  
			if((connection_control & __USB_IN_SOCKET) && (!(connection_control & __SERIAL_INITIATED)))
			{
				serial_com_init();
			}
		}
	}
	else if(dtr_pin == false)
	{
		if(connection_control & __CONNECTION_ESTABLISHED)
		{  
			if(connection_control & __SERIAL_INITIATED)
			{
				if(connection_control & __ESTABLISHED_TO_SIM_USB)
					disconnect_function(__ESTABLISHED_TO_SIM_USB);
				else if(connection_control & __ESTABLISHED_TO_PAT_USB)
					disconnect_function(__ESTABLISHED_TO_PAT_USB);
			}
		}
	}
}
void usbCheckTask(void)
{
	checkUSBconnected();
	connection_control &= ~__USB_PLUG_CHANGED;
	if(connection_control &__USB_IN_SOCKET)
		restart_timer(USB_CHECK_TASK,USB_CHECK_INTERVAL,usbCheckTask);
}
//function for analysing commands 
//returns true if command is found
//returns false if command is not found or it cant be executed
bool commandAnalzye(uint8_t dir)
{
	uint8_t direction;
/*******************************************************************************/
/**													INPUT CONNECTION NOT OK														**/
/*******************************************************************************/
	if(global_control & __INIT_TEST_FAIL)
	{
		set_event(SEND_WARNING_MSG,send_warning_MSG);
	}
/*******************************************************************************/
/**															MEASURING METODS															**/
/*******************************************************************************/
//	else if(!strcmp(m_function,__POWER__))//tle more bit pol obvezno else if !!!!!!!!!!!!!!!!!
//	{     
//		if(!strcmp(m_command,__START__))
//		{
//			start_measure();
//		}
//		else if(!strcmp(m_command,__STOP__))
//		{
//			stop_measure();
//		}
//		else if(!strcmp(m_command,__START_NO_THD__))
//		{
//			start_measure_no_THD();
//		}
//		
//	} 
/*********************************************************************************/
/**									RELAYS										**/
/*********************************************************************************/
	else if(!strcmp(m_function,__RELAY__))
	{ 
		RCC->CSR |= 0x01000000;//resetiramo reset zastavice;
		test4_on;
		if(!strcmp(m_command,__1_38_ON__))				set_REL(1);
		else if(!strcmp(m_command,__1_38_OFF__))	rst_REL(1);
		else if(!strcmp(m_command,__2_ON__))	set_REL(2);
		else if(!strcmp(m_command,__2_OFF__))	rst_REL(2);
		else if(!strcmp(m_command,__3_ON__))	set_REL(3);		
		else if(!strcmp(m_command,__3_OFF__))	rst_REL(3);
		else if(!strcmp(m_command,__4_ON__))	set_REL(4);
		else if(!strcmp(m_command,__4_OFF__))	rst_REL(4);
		else if(!strcmp(m_command,__5_ON__))	set_REL(5);
		else if(!strcmp(m_command,__5_OFF__))	rst_REL(5);
		else if(!strcmp(m_command,__6_ON__))	set_REL(6);		
		else if(!strcmp(m_command,__6_OFF__))	rst_REL(6);
		else if(!strcmp(m_command,__7_ON__))	set_REL(7);
		else if(!strcmp(m_command,__7_OFF__))	rst_REL(7);
		else if(!strcmp(m_command,__8_ON__))	set_REL(8);
		else if(!strcmp(m_command,__8_OFF__))	rst_REL(8);
		else if(!strcmp(m_command,__9_ON__))	set_REL(9);		
		else if(!strcmp(m_command,__9_OFF__))	rst_REL(9);
		else if(!strcmp(m_command,__10_ON__))	set_REL(10);
		else if(!strcmp(m_command,__10_OFF__))	rst_REL(10);
		else if(!strcmp(m_command,__11_ON__))	set_REL(11);
		else if(!strcmp(m_command,__11_OFF__))	rst_REL(11);
		else if(!strcmp(m_command,__12_ON__))	set_REL(12);		
		else if(!strcmp(m_command,__12_OFF__))	rst_REL(12);
		else if(!strcmp(m_command,__13_ON__))	set_REL(13);
		else if(!strcmp(m_command,__13_OFF__))	rst_REL(13);
		else if(!strcmp(m_command,__14_ON__))	set_REL(14);
		else if(!strcmp(m_command,__14_OFF__))	rst_REL(14);
		else if(!strcmp(m_command,__15_ON__))	set_REL(15);		
		else if(!strcmp(m_command,__15_OFF__))	rst_REL(15);
		else if(!strcmp(m_command,__16_ON__))	set_REL(16);
		else if(!strcmp(m_command,__16_OFF__))	rst_REL(16);
		else if(!strcmp(m_command,__17_ON__))	set_REL(17);
		else if(!strcmp(m_command,__17_OFF__))	rst_REL(17);
		else if(!strcmp(m_command,__18_ON__))	set_REL(18);		
		else if(!strcmp(m_command,__18_OFF__))	rst_REL(18);
		else if(!strcmp(m_command,__19_ON__))	set_REL(19);
		else if(!strcmp(m_command,__19_OFF__))	rst_REL(19);
		else if(!strcmp(m_command,__20_ON__))	set_REL(20);
		else if(!strcmp(m_command,__20_OFF__))	rst_REL(20);
		else if(!strcmp(m_command,__21_43_ON__))	set_REL(21);		
		else if(!strcmp(m_command,__21_43_OFF__))	rst_REL(21);
		else if(!strcmp(m_command,__22_ON__))	set_REL(22);
		else if(!strcmp(m_command,__22_OFF__))	rst_REL(22);
		else if(!strcmp(m_command,__23_ON__))	set_REL(23);
		else if(!strcmp(m_command,__23_OFF__))	rst_REL(23);
		else if(!strcmp(m_command,__24_ON__))	set_REL(24);		
		else if(!strcmp(m_command,__24_OFF__))	rst_REL(24);
		else if(!strcmp(m_command,__25_ON__))	set_REL(25);
		else if(!strcmp(m_command,__25_OFF__))	rst_REL(25);
		else if(!strcmp(m_command,__26_ON__))	set_REL(26);
		else if(!strcmp(m_command,__26_OFF__))	rst_REL(26);
		else if(!strcmp(m_command,__27_ON__))	set_REL(27);		
		else if(!strcmp(m_command,__27_OFF__))	rst_REL(27);
		else if(!strcmp(m_command,__28_ON__))	set_REL(28);
		else if(!strcmp(m_command,__28_OFF__))	rst_REL(28);
		else if(!strcmp(m_command,__29_ON__))	set_REL(29);
		else if(!strcmp(m_command,__29_OFF__))	rst_REL(29);
		else if(!strcmp(m_command,__30_ON__))	set_REL(30);
		else if(!strcmp(m_command,__30_OFF__))	rst_REL(30);
		else if(!strcmp(m_command,__31_ON__))	set_REL(31);
		else if(!strcmp(m_command,__31_OFF__))	rst_REL(31);
		else if(!strcmp(m_command,__32_ON__))	set_REL(32);
		else if(!strcmp(m_command,__32_OFF__))	rst_REL(32);
		else if(!strcmp(m_command,__33_ON__))	set_REL(33);
		else if(!strcmp(m_command,__33_OFF__))	rst_REL(33);
		else if(!strcmp(m_command,__34_ON__))	set_REL(34);
		else if(!strcmp(m_command,__34_OFF__))	rst_REL(34);
		else if(!strcmp(m_command,__35_ON__))	set_REL(35);
		else if(!strcmp(m_command,__35_OFF__))	rst_REL(35);
		else if(!strcmp(m_command,__36_ON__))	set_REL(36);
		else if(!strcmp(m_command,__36_OFF__))	rst_REL(36);
		else if(!strcmp(m_command,__37_ON__))	set_REL(37);
		else if(!strcmp(m_command,__37_OFF__))	rst_REL(37);
		else if(!strcmp(m_command,__39_ON__))	set_REL(39);
		else if(!strcmp(m_command,__39_OFF__))	rst_REL(39);
		else if(!strcmp(m_command,__40_ON__))	set_REL(40);
		else if(!strcmp(m_command,__40_OFF__))	rst_REL(40);
		else if(!strcmp(m_command,__41_ON__))	set_REL(41);
		else if(!strcmp(m_command,__41_OFF__))	rst_REL(41);
		else if(!strcmp(m_command,__42_ON__))	set_REL(42);
		else if(!strcmp(m_command,__42_OFF__))	rst_REL(42);
		else if(!strcmp(m_command,__RESET_ALL__))	reset_all_REL();
		HAL_Delay(1);
		test4_off;
	}
/*********************************************************************************/
/**									TEST										**/
/*********************************************************************************/
	else if(!strcmp(m_function,__TEST__))
	{ 
		if(!strcmp(m_command,__PROTOCOL_TEST__))
		{
			SendComMessage(_ON,_ID_TFA,transmitter_ID,__TEST__,__PROTOCOL_TEST__,"","",dir);
			SendComMessage(_ON,_ID_TFA,transmitter_ID,__TEST__,__PROTOCOL_TEST__,"","KRNEKI",dir);
			SendComMessage(_ON,_ID_TFA,transmitter_ID,__TEST__,__PROTOCOL_TEST__,"","KRNEKI2",dir);
			SendComMessage(_ON,_ID_TFA,transmitter_ID,__TEST__,__PROTOCOL_TEST__,"","KRNEKI3",dir);
			SendComMessage(_ON,_ID_TFA,transmitter_ID,__TEST__,__PROTOCOL_TEST__,"","KRNEKI4",dir);
			SendComMessage(_ON,_ID_TFA,transmitter_ID,__TEST__,__PROTOCOL_TEST__,"","KRNEKI5",dir);
		}
	}
/*********************************************************************************/
/**									CONNECTION									**/
/*********************************************************************************/
	else if(!strcmp(m_function,__CONNECTION__))
	{ 
		if(!strcmp(m_command,__CONNECTION_REQUEST__))
		{
			if(!strcmp(&additionalCode[0][0][0],__MT_310__))
			{
				if(connection_control & __CON_TO_PAT)
				{
					SendComMessage(_ON,_ID_TFA,transmitter_ID,__CONNECTION__,__CONNECTION_DENIED__,__DIFF_DEVICE_CONNECTED__,"",dir);
				}
				else if(connection_control & __CON_TO_MT310)
				{
					SendComMessage(_ON,_ID_TFA,transmitter_ID,__CONNECTION__,__CONNECTION_DENIED__,__ALREADY_CONNECTED__,"",dir);
				}
				else
				{
					connection_control |= __CON_TO_MT310;
					connection_control |= __CONNECTION_ESTABLISHED;
					switch(transmitter_ID)
					{
						case _ID_SIMULATION:
							connection_control |= __ESTABLISHED_TO_SIM_USB;
							break;
						case _ID_MT:
							connection_control |= __ESTABLISHED_TO_MT310;
							break;
						case _ID_PAT:
							connection_control |= __ESTABLISHED_TO_PAT_USB;
							break;
						default:
							break;
					}
					set_event(SEND_TFA_MAINS_STATUS,send_mains_status);
					SendComMessage(_ON,_ID_TFA,transmitter_ID,__CONNECTION__,__CONNECTION_ESTABLISHED__,__MT_300__,"",dir);
					if(CONNECTION_CHECK ==_ON)
						restart_timer(CHECK_CONNECTION,CONNECTION_CHECK_INTERVAL,check_connection);	//preverjanje komunikacije po intervalih
				}
			}
			if(!strcmp(&additionalCode[0][0][0],__PAT__))
			{
				if(connection_control & __CON_TO_MT310)
				{
					SendComMessage(_ON,_ID_TFA,transmitter_ID,__CONNECTION__,__CONNECTION_DENIED__,__DIFF_DEVICE_CONNECTED__,"",dir);
				}
				else if(connection_control & __CON_TO_PAT)
				{
					SendComMessage(_ON,_ID_TFA,transmitter_ID,__CONNECTION__,__CONNECTION_DENIED__,__ALREADY_CONNECTED__,"",dir);
				}
				else
				{
					connection_control |= __CON_TO_PAT;
					connection_control |= __CONNECTION_ESTABLISHED;
					set_event(SEND_TFA_MAINS_STATUS,send_mains_status);
					SendComMessage(_ON,_ID_TFA,transmitter_ID,__CONNECTION__,__CONNECTION_ESTABLISHED__,__PAT__,"",dir);
					switch(transmitter_ID)
					{
						case _ID_SIMULATION:
							connection_control |= __ESTABLISHED_TO_SIM_USB;
							break;
						case _ID_MT:
							connection_control |= __ESTABLISHED_TO_MT310;
							break;
						case _ID_PAT:
							connection_control |= __ESTABLISHED_TO_PAT_USB;
							break;
						default:
							break;
					}
					if(CONNECTION_CHECK ==_ON)
						restart_timer(CHECK_CONNECTION,CONNECTION_CHECK_INTERVAL,check_connection);	//preverjanje komunikacije po intervalih
				}
			}
		}
		else if(!strcmp(m_command,__CHECK_CONNECTION__))
		{
			if(!strcmp(m_value,__CON_OK_))
			{
					if(transmitter_ID == _ID_MT)
					{
						connection_control &= (~__CHECK_TO_MT310);
					}
			}
			else if(!strcmp(m_value,__CON_NOK_))
			{
					if(transmitter_ID == _ID_MT)
					{
						disconnect_function(__CON_TO_MT310);
					}
			}
		}
	}
/*********************************************************************************/
/**																	ALL	FUNCTIONS																**/
/*********************************************************************************/
	//RPE
	else if(!strcmp(m_function,&FUNCTION_COMMUNICATON_NAMES[0][0]))
	{ 
		if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES2[0][0]))//VALUE
		{ 
			if((meas_task_control & __CORD_MEAS_IN_PROG) && (cord_task_control & __CORD_RPE_RES_REQUESTED))	//se izvede samo ce je meritev v teku
					set_RPE_cord_resistance(&additionalCode[1][0][0]);
		}
	}
	//RISO
	else if(!strcmp(m_function,&FUNCTION_COMMUNICATON_NAMES[7][0]))
	{ 
		if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES2[0][0]))//VALUE
		{ 
			if((meas_task_control & __CORD_MEAS_IN_PROG) && (cord_task_control & __CORD_RISO_RES_REQUESTED))	//se izvede samo ce je meritev v teku
				set_RISO_cord_resistance(&additionalCode[1][0][0]);
			else if((meas_task_control & __MACH_MEAS_IN_PROG) && (mach_task_control & __MACH_RISO_RES_REQUESTED))	//se izvede samo ce je meritev v teku
				set_RISO_mach_resistance(&additionalCode[1][0][0]);
			else if((meas_task_control & __WELD_MEAS_IN_PROG) && (weld_task_control & __WELD_RISO_RES_REQUESTED))	//se izvede samo ce je meritev v teku
				set_RISO_weld_resistance(&additionalCode[1][0][0]);
		}
		
	}
	//POWER
	else if(!strcmp(m_function,&FUNCTION_COMMUNICATON_NAMES[11][0]))	
	{ 
		if(!strcmp(&additionalCode[1][0][0],&STD_COMMUNICATION_NAMES[2][0]))	//MACH
		{	
			if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[0][0]))//START
			{
				if((!(meas_task_control & __POWER_MEAS_IN_PROG)))
				{	
					meas_task_control |= __POWER_MEAS_IN_PROG;
					mach_task_control |= __MACH_MAINS_ANALYZE_IN_PROGRESS;
					mach_task_control |= __MACH_POWER_START_REQUESTED;
					set_event(MAINS_ANALYZE_MEAS_START,startMainsMeasurement);
				}
			}
			else if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[1][0]))//STOP
			{
				if((meas_task_control & __POWER_MEAS_IN_PROG))
				{	
					meas_task_control &= ~__POWER_MEAS_IN_PROG;
					weld_task_control &= ~__WELD_MAINS_ANALYZE_IN_PROGRESS;
					mach_task_control &= ~__MACH_MAINS_ANALYZE_IN_PROGRESS;
					set_event(MAINS_ANALYZE_MEAS_STOP,stopMainsMeasurement);
					WeldMachMt310_RelInit();
				}
			}
		}
		if(!strcmp(&additionalCode[1][0][0],&STD_COMMUNICATION_NAMES[1][0]))	//WELD
		{	
			if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[0][0]))//START
			{
				if((!(meas_task_control & __POWER_MEAS_IN_PROG)))
				{	
					meas_task_control |= __POWER_MEAS_IN_PROG;
					weld_task_control |= __WELD_MAINS_ANALYZE_IN_PROGRESS;
					weld_task_control |= __WELD_POWER_START_REQUESTED;
					set_event(MAINS_ANALYZE_MEAS_START,startMainsMeasurement);
				}
			}
			else if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[1][0]))//STOP
			{
				if((meas_task_control & __WELD_MEAS_IN_PROG))
				{	
					meas_task_control &= ~__POWER_MEAS_IN_PROG;
					mach_task_control &= ~__MACH_MAINS_ANALYZE_IN_PROGRESS;
					weld_task_control &= ~__WELD_MAINS_ANALYZE_IN_PROGRESS;
					set_event(MAINS_ANALYZE_MEAS_STOP,stopMainsMeasurement);
					WeldMachMt310_RelInit();
				}
			}
		}
	}
/*********************************************************************************/
/**									CORD										**/
/*********************************************************************************/
	else if(!strcmp(&additionalCode[1][0][0],&STD_COMMUNICATION_NAMES[0][0]))	//CORD
	{ 
		//NEW
		//nastavimo tip kabla, potrebno samo, ce je drugacen kot v inicializaciji
		//STOP COMMAND - velja za vse, zato je na zacetk
		if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[1][0]))
		{ 
			if(meas_task_control & __CORD_MEAS_IN_PROG)	//se izvede samo ce je meritev v teku
				set_event(STOP_CORD,stop_cord);
		}
		//SET
		else if(!strcmp(m_function,&FUNCTION_COMMUNICATON_NAMES[3][0]))
		{
			if(!strcmp(m_command,&FUNCTION_COMMUNICATON_NAMES[3][0]))
			{
				if(!strcmp(&additionalCode[0][1][0],&FUNCTION_COMMUNICATON_NAMES[3][0]))
				{
					if(!strcmp(&additionalCode[1][1][0],__1_PHASE__))
					{
						set_phase_num(1);
					}
					else if(!strcmp(&additionalCode[1][1][0],__3_PHASE__))
					{
						set_phase_num(3);
					}
				}
			}
		}
		//ce CORD se ni inicializiran ga najprej inicializira nato pa zacne meritev
		//C_W
		else if(!strcmp(m_function,&FUNCTION_COMMUNICATON_NAMES[1][0]))
		{
			if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[0][0]))//START
			{
				//ce je katerakoli meritev v delu se ne izvede
				if(!(cord_task_control & CORD_MEAS_IN_PROG_MASK))
				{
					cord_cw_count = 0;
					//SetCordLimit(SET_CORD_RPE_LIM, &additionalCode[1][2][0]);//nastavimo limito
					if(!strcmp(&additionalCode[1][1][0],__1_PHASE__))
					{
						set_phase_num(1);
					}
					else if(!strcmp(&additionalCode[1][1][0],__3_PHASE__))
					{
						set_phase_num(3);
					}
					set_event(CORD_MEAS_CORRECT_WIRING,cord_meas_correct_wiring);
				}
			}
		}
		//CONT
		else if(!strcmp(m_function,&FUNCTION_COMMUNICATON_NAMES[2][0]))
		{
			if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[0][0]))//START
			{
				//ce je katerakoli meritev v delu se ne izvede
				if(!(cord_task_control & CORD_MEAS_IN_PROG_MASK))
				{
					if(!strcmp(&additionalCode[1][1][0],__1_PHASE__))
					{
						set_phase_num(1);
					}
					else if(!strcmp(&additionalCode[1][1][0],__3_PHASE__))
					{
						set_phase_num(3);
					}
					cord_continuity_count=0;
					SetCordLimit(SET_CORD_CONT_LIM, &additionalCode[1][2][0]);//nastavimo limito
					set_event(CORD_MEAS_CONTINUITY,cord_continuity_test);
				}
			}
		}
		//ALL-PE
		else if(!strcmp(m_function,&FUNCTION_COMMUNICATON_NAMES[4][0]))
		{
			if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[0][0]))//START
			{
				//ce je katerakoli meritev v delu se ne izvede
				if(!(cord_task_control & CORD_MEAS_IN_PROG_MASK))
				{
					if(!strcmp(&additionalCode[1][1][0],__1_PHASE__))
					{
						set_phase_num(1);
					}
					else if(!strcmp(&additionalCode[1][1][0],__3_PHASE__))
					{
						set_phase_num(3);
					}
					cord_RISO_count=0;	//da zacne od zacetka
					SetCordLimit(SET_CORD_RISO_LIM, &additionalCode[1][2][0]);//nastavimo limito
					set_event(CORD_RISO_PHASES_TO_PE,cord_RISO_phasesToPE);
				}
			}
		}
		//ONE-PE
		else if(!strcmp(m_function,&FUNCTION_COMMUNICATON_NAMES[5][0]))
		{
			if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[0][0]))//START
			{
				//ce je katerakoli meritev v delu se ne izvede
				if(!(cord_task_control & CORD_MEAS_IN_PROG_MASK))
				{
					if(!strcmp(&additionalCode[1][1][0],__1_PHASE__))
					{
						set_phase_num(1);
					}
					else if(!strcmp(&additionalCode[1][1][0],__3_PHASE__))
					{
						set_phase_num(3);
					}
					cord_RISO_count = 3;
					SetCordLimit(SET_CORD_RISO_LIM, &additionalCode[1][2][0]);//nastavimo limito
					set_event(CORD_RISO_ONE_PHASE_TO_PE,cord_RISO_onePhaseToPE);
				}
			}
		}
		//PH-PH
		else if(!strcmp(m_function,&FUNCTION_COMMUNICATON_NAMES[6][0]))
		{
			if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[0][0]))//START
			{
				//ce je katerakoli meritev v delu se ne izvede
				if(!(cord_task_control & CORD_MEAS_IN_PROG_MASK))
				{
					if(!strcmp(&additionalCode[1][1][0],__1_PHASE__))
					{
						set_phase_num(1);
					}
					else if(!strcmp(&additionalCode[1][1][0],__3_PHASE__))
					{
						set_phase_num(3);
					}
					SetCordLimit(SET_CORD_RISO_LIM, &additionalCode[1][2][0]);//nastavimo limito
					set_event(CORD_RISO_PHASE_TO_PHASE,cord_RISO_phaseToPhase);
				}
			}
		}
		//RPE
		else if(!strcmp(m_function,&FUNCTION_COMMUNICATON_NAMES[9][0]))
		{
			if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[0][0]))//START
			{
				if((!(cord_task_control & CORD_MEAS_IN_PROG_MASK)))
					set_event(CORD_RPE_START,CordRPEStart);
			}
			if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[1][0]))//STOP
			{
				if((meas_task_control & __CORD_MEAS_IN_PROG)&&(cord_task_control & __CORD_RPE_IN_PROGRESS))
					set_event(CORD_RPE_STOP,CordRPEStop);
			}
		}
	}
	//OLD
	else if(!strcmp(m_function,__CORD__))
	{ 
		if(!strcmp(m_command,__INIT_CORD__))
		{
			if(!(meas_task_control & __CORD_MEAS_IN_PROG))	//ce je meritev ze v teku se ne zgodi nic
			{
				meas_task_control |= __CORD_MEAS_IN_PROG;
				if(!strcmp(m_value,__1_PHASE__))
					set_phase_num(1);
				else if(!strcmp(m_value,__3_PHASE__))
					set_phase_num(3);
				start_cord_count = 0;
				set_event(INIT_CORD,init_cord);
				cord_task_control |= __CORD_INIT_RECIEVED;
			}
		}
		else if(!strcmp(m_command,__DEINIT_CORD__))
		{
			if(cord_task_control & __CORD_INITIATED)	
			{
				start_cord_count = 0;
				set_event(DEINIT_CORD,deinitCord);
			}
		}
		//nastavimo tip kabla, potrebno samo, ce je drugacen kot v inicializaciji
		else if(!strcmp(m_command,__SET__))
		{
			if(!strcmp(&additionalCode[0][0][0],__CABLE_TYPE__))
			{
				if(!strcmp(&additionalCode[1][0][0],__1_PHASE__))
				{
					set_phase_num(1);
				}
				else if(!strcmp(&additionalCode[1][0][0],__3_PHASE__))
				{
					set_phase_num(3);
				}
			}
		}
		//ce CORD se ni inicializiran ga najprej inicializira nato pa zacne meritev
		else if(!strcmp(m_command,__START_CORRECT_WIRING__))
		{
			//ce je katerakoli meritev v delu se ne izvede
			if(!(cord_task_control & CORD_MEAS_IN_PROG_MASK))
			{
				if(!(cord_task_control & __CORD_CORRECT_WIRING_IN_PROGRESS))
				{
					if(cord_task_control & __CORD_INITIATED)
					{
						set_event(CORD_MEAS_CORRECT_WIRING,cord_meas_correct_wiring);
					}
					else if(!(meas_task_control & __CORD_MEAS_IN_PROG))	//ce je meritev ze v teku se ne zgodi nic
					{
						meas_task_control |= __CORD_MEAS_IN_PROG;
						start_cord_count = 0;
						set_event(INIT_CORD,init_cord);
					}
				}
			}
		}
		else if(!strcmp(m_command,__START_CONTINUITY__))
		{
			//ce je katerakoli meritev v delu se ne izvede
			if(!(cord_task_control & CORD_MEAS_IN_PROG_MASK))
			{
				if(!(cord_task_control & __CORD_CONTINUITY_IN_PROGRESS))
				{
					if(cord_task_control & __CORD_INITIATED)
					{
						set_event(CORD_MEAS_CONTINUITY,cord_continuity_test);
					}
					else if(!(meas_task_control & __CORD_MEAS_IN_PROG))	//ce je meritev ze v teku se ne zgodi nic
					{
						meas_task_control |= __CORD_MEAS_IN_PROG;
						start_cord_count = 0;
						set_event(INIT_CORD,init_cord);
					}
				}
			}
		}
		else if(!strcmp(m_command,__START_PHASES_TO_PE__))
		{
			//ce je katerakoli meritev v delu se ne izvede
			if(!(cord_task_control & CORD_MEAS_IN_PROG_MASK))
			{
				if(!(cord_task_control & __CORD_RISO_PHASES_TO_PE_IN_PROGRESS))
				{
					if(cord_task_control & __CORD_INITIATED)
					{
						cord_RISO_count=0;	//da zacne od zacetka
						set_event(CORD_RISO_PHASES_TO_PE,cord_RISO_phasesToPE);
					}
					else if(!(meas_task_control & __CORD_MEAS_IN_PROG))	//ce je meritev ze v teku se ne zgodi nic
					{
						cord_RISO_count=0;
						meas_task_control |= __CORD_MEAS_IN_PROG;
						start_cord_count = 0;
						set_event(INIT_CORD,init_cord);
					}
				}
			}
		}
		else if(!strcmp(m_command,__START_ONE_PHASE_TO_PE__))
		{
			//ce je katerakoli meritev v delu se ne izvede
			if(!(cord_task_control & CORD_MEAS_IN_PROG_MASK))
			{
				if(!(cord_task_control & __CORD_RISO_ONE_PHASE_TO_PE_IN_PROGRESS))
				{
					if(cord_task_control & __CORD_INITIATED)
					{
						set_event(CORD_RISO_ONE_PHASE_TO_PE,cord_RISO_onePhaseToPE);
					}
					else if(!(meas_task_control & __CORD_MEAS_IN_PROG))	//ce je meritev ze v teku se ne zgodi nic
					{
						meas_task_control |= __CORD_MEAS_IN_PROG;
						start_cord_count = 0;
						set_event(INIT_CORD,init_cord);
					}
				}
			}
		}
		else if(!strcmp(m_command,__START_PHASE_TO_PHASE__))
		{
			//ce je katerakoli meritev v delu se ne izvede
			if(!(cord_task_control & CORD_MEAS_IN_PROG_MASK))
			{
				if(!(cord_task_control & __CORD_RISO_PHASE_TO_PHASE_IN_PROGRESS))
				{
					if(cord_task_control & __CORD_INITIATED)
					{
						set_event(CORD_RISO_PHASE_TO_PHASE,cord_RISO_phaseToPhase);
					}
					else if(!(meas_task_control & __CORD_MEAS_IN_PROG))	//ce je meritev ze v teku se ne zgodi nic
					{
						meas_task_control |= __CORD_MEAS_IN_PROG;
						start_cord_count = 0;
						set_event(INIT_CORD,init_cord);
					}
				}
			}
		}
		else if(!strcmp(m_command,__STOP_C__))
		{
			if(meas_task_control & __CORD_MEAS_IN_PROG)	//se izvede samo ce je meritev v teku
				set_event(STOP_CORD,stop_cord);
		}
		else if(!strcmp(m_command,__INIT_C__))
		{
			//cord_correct_wiring_init();
		}
		else if(!strcmp(m_command,__RPE_RESISTANCE__))
		{
			if((meas_task_control & __CORD_MEAS_IN_PROG) && (cord_task_control & __CORD_RPE_RES_REQUESTED))	//se izvede samo ce je meritev v teku
				set_RPE_cord_resistance(&additionalCode[1][0][0]);
		}
		else if(!strcmp(m_command,__RISO_RESISTANCE__))
		{
			if((meas_task_control & __CORD_MEAS_IN_PROG) && (cord_task_control & __CORD_RISO_RES_REQUESTED))	//se izvede samo ce je meritev v teku
				set_RISO_cord_resistance(&additionalCode[1][0][0]);
		}
		else if(!strcmp(m_command,__RISO_STARTED__))
		{
			if(meas_task_control & __CORD_MEAS_IN_PROG)
				cord_task_control |= __CORD_RISO_STARTED;
		}
		else if(!strcmp(m_command,__RPE_LOW_STARTED__))
		{
			if(meas_task_control & __CORD_MEAS_IN_PROG)
				cord_task_control |= __CORD_RPE_L_STARTED;
		}
		else if(!strcmp(m_command,__RPE_HIGH_STARTED__))
		{
			if(meas_task_control & __CORD_MEAS_IN_PROG)
				cord_task_control |= __CORD_RPE_H_STARTED;
		}
		else if(!strcmp(m_command,__RISO_STARTED__))
		{
			if(meas_task_control & __CORD_MEAS_IN_PROG)
				cord_task_control |= __CORD_RISO_STARTED;
		}

		else if(!strcmp(m_command,__RISO_STOPPED__))
		{
			if(meas_task_control & __CORD_MEAS_IN_PROG)
				cord_task_control &= (~__CORD_RISO_STARTED);
		}
		else if(!strcmp(m_command,__RPE_STOPPED__))
		{
			if(meas_task_control & __CORD_MEAS_IN_PROG)
				cord_task_control &=~(__CORD_RPE_L_STARTED|__CORD_RPE_H_STARTED);
		}
		
	}
/*********************************************************************************/
/**																	MACHINES																		**/
/*********************************************************************************/
	else if(!strcmp(&additionalCode[1][0][0],&STD_COMMUNICATION_NAMES[2][0]))	//MACH
	{ 
		//nastavimo tip kabla, potrebno samo, ce je drugacen kot v inicializaciji
		//STOP COMMAND - velja za vse, zato je na zacetk
		if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[1][0]))//STOP
		{ 
			if(meas_task_control & __MACH_MEAS_IN_PROG)	//se izvede samo ce je meritev v teku
				set_event(STOP_MACH,stop_mach);
		}
		//SET
		else if(!strcmp(m_function,&FUNCTION_COMMUNICATON_NAMES[3][0]))
		{
			if(!strcmp(m_command,&FUNCTION_COMMUNICATON_NAMES[3][0]))
			{
				if(!strcmp(&additionalCode[0][1][0],&FUNCTION_COMMUNICATON_NAMES[3][0]))
				{
					if(!strcmp(&additionalCode[1][1][0],__1_PHASE__))
					{
						set_phase_num_mach(1);
					}
					else if(!strcmp(&additionalCode[1][1][0],__3_PHASE__))
					{
						set_phase_num_mach(3);
					}
				}
			}
		}
		//ALL-PE
		else if(!strcmp(m_function,&FUNCTION_COMMUNICATON_NAMES[4][0]))
		{
			if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[0][0]))//START
			{
				//ce je katerakoli meritev v delu se ne izvede
				if(!(mach_task_control & MACH_IN_PROG_MASKS))
				{
					if(!strcmp(&additionalCode[1][1][0],__1_PHASE__))
					{
						set_phase_num_mach(1);
					}
					else if(!strcmp(&additionalCode[1][1][0],__3_PHASE__))
					{
						set_phase_num_mach(3);
					}
					mach_RISO_count=0;	//da zacne od zacetka
					SetMachLimit(&additionalCode[1][2][0]);//nastavimo limito
					set_event(MACH_RISO_PHASES_TO_PE,mach_RISO_phasesToPE);
				}
			}
		}
		//ONE-PE
		else if(!strcmp(m_function,&FUNCTION_COMMUNICATON_NAMES[5][0]))
		{
			if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[0][0]))//START
			{
				//ce je katerakoli meritev v delu se ne izvede
				if(!(mach_task_control & MACH_IN_PROG_MASKS))
				{
					if(!strcmp(&additionalCode[1][1][0],__1_PHASE__))
					{
						set_phase_num_mach(1);
					}
					else if(!strcmp(&additionalCode[1][1][0],__3_PHASE__))
					{
						set_phase_num_mach(3);
					}
					mach_RISO_count = 3;
					SetMachLimit(&additionalCode[1][2][0]);//nastavimo limito
					set_event(MACH_RISO_ONE_PHASE_TO_PE,mach_RISO_onePhaseToPE);
				}
			}
		}
		//RPE
		else if(!strcmp(m_function,&FUNCTION_COMMUNICATON_NAMES[9][0]))
		{
			if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[0][0]))//START
			{
				if((!(mach_task_control & MACH_IN_PROG_MASKS)))
					set_event(MACH_RPE_START,MachinesRPEStart);
			}
			if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[1][0]))//STOP
			{
				if((meas_task_control & __MACH_MEAS_IN_PROG)&&(mach_task_control & __MACH_RPE_IN_PROGRESS))
					set_event(MACH_RPE_STOP,MachinesRPEStop);
			}
		}
		//URES
		else if(!strcmp(m_function,&FUNCTION_COMMUNICATON_NAMES[10][0]))
		{
			if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[6][0]))//INIT
			{
				if((!(mach_task_control & MACH_IN_PROG_MASKS)))
				{
					if(!strcmp(&additionalCode[1][1][0],__MACH_L1_PE__))
						current_URES_measurement = __L1_PE;
					else if(!strcmp(&additionalCode[1][1][0],__MACH_L2_PE__))
						current_URES_measurement = __L2_PE;
					else if(!strcmp(&additionalCode[1][1][0],__MACH_L3_PE__))
						current_URES_measurement = __L3_PE;
					else if(!strcmp(&additionalCode[1][1][0],__MACH_L1_N__))
						current_URES_measurement = __L1_N;
					else if(!strcmp(&additionalCode[1][1][0],__MACH_L2_N__))
						current_URES_measurement = __L2_N;
					else if(!strcmp(&additionalCode[1][1][0],__MACH_L3_N__))
						current_URES_measurement = __L3_N;
					else if(!strcmp(&additionalCode[1][1][0],__MACH_L1_L2__))
						current_URES_measurement = __L1_L2;
					else if(!strcmp(&additionalCode[1][1][0],__MACH_L2_L3__))
						current_URES_measurement = __L2_L3;
					else if(!strcmp(&additionalCode[1][1][0],__MACH_L1_L3__))
						current_URES_measurement = __L1_L3;
					set_event(MACH_URES,mach_URES);
				}
			}
			if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[5][0]))//OPEN
			{
				if((meas_task_control & __MACH_MEAS_IN_PROG)&&(mach_task_control & __MACH_URES_IN_PROGRESS))
					machOPENcontactors();
			}
		}	
	}
	//OLD
	else if(!strcmp(m_function,__MACHINES__))
	{ 
		if(!strcmp(m_command,__INIT_MACHINES__))
		{
			if(!(meas_task_control & __MACH_MEAS_IN_PROG))	//ce je meritev ze v teku se ne zgodi nic
			{
				meas_task_control |= __MACH_MEAS_IN_PROG;
				if(!strcmp(m_value,__1_PHASE__))
					set_phase_num_mach(1);
				else
					set_phase_num_mach(3);
				start_mach_count = 0;
				set_event(INIT_MACH,init_mach);
				mach_task_control |= __MACH_INIT_RECIEVED;
			}
		}
		else if(!strcmp(m_command,__DEINIT_MACHINES__))
		{
			if(mach_task_control & __MACH_INITIATED)
			{
				start_mach_count = 0;
				set_event(DEINIT_MACH,deinitMachines);
			}
		}
		else if(!strcmp(m_command,__SET__))
		{
			if(!strcmp(&additionalCode[0][0][0],__CABLE_TYPE__))
			{
				if(!strcmp(&additionalCode[1][0][0],__1_PHASE__))
				{
					set_phase_num_mach(1);
				}
				else if(!strcmp(&additionalCode[1][0][0],__3_PHASE__))
				{
					set_phase_num_mach(3);
				}
			}
		}
		else if(!strcmp(m_command,__START_PHASES_TO_PE__))
		{
			//ce je katerakoli meritev v delu se ne izvede
			if((meas_task_control & __MACH_MEAS_IN_PROG))
			{
				if(!(mach_task_control &__MACH_RISO_PHASES_TO_PE_IN_PROGRESS))
				{
					if(mach_task_control & __MACH_INITIATED)
					{
						mach_RISO_count = 0;			//nastavimo na 0, ker moramo ponovno izmeriti vse faze proti PE
						set_event(MACH_RISO_PHASES_TO_PE,mach_RISO_phasesToPE);
					}
					else if(!(meas_task_control & __MACH_MEAS_IN_PROG))	//ce je meritev ze v teku se ne zgodi nic
					{
						meas_task_control |= __MACH_MEAS_IN_PROG;
						start_cord_count = 0;
						mach_RISO_count = 0;			//nastavimo na 0, ker moramo ponovno izmeriti vse faze proti PE
						set_event(INIT_MACH,init_mach);
					}
				}
			}
		}
		else if(!strcmp(m_command,__START_ONE_PHASE_TO_PE__))
		{
			//ce je katerakoli meritev v delu se ne izvede
			if((meas_task_control & __MACH_MEAS_IN_PROG))
			{
				if(!(mach_task_control &__MACH_RISO_ONE_PHASE_TO_PE_IN_PROGRESS))
				{
					if(mach_task_control & __MACH_INITIATED)
					{
						set_event(MACH_RISO_ONE_PHASE_TO_PE,mach_RISO_onePhaseToPE);
					}
					else if(!(meas_task_control & __MACH_MEAS_IN_PROG))	//ce je meritev ze v teku se ne zgodi nic
					{
						meas_task_control |= __MACH_MEAS_IN_PROG;
						start_mach_count = 0;
						set_event(INIT_MACH,init_mach);
					}
				}
			}
		}
		else if(!strcmp(m_command,__STOP_MACH__))
		{
			if(meas_task_control & __MACH_MEAS_IN_PROG)	//se izvede samo ce je meritev v teku
				set_event(STOP_MACH,stop_mach);
		}
		else if(!strcmp(m_command,__RISO_RESISTANCE__))
		{
			if((meas_task_control & __MACH_MEAS_IN_PROG) && (mach_task_control & __MACH_RISO_RES_REQUESTED))	//se izvede samo ce je meritev v teku
				set_RISO_mach_resistance(&additionalCode[1][0][0]);
		}
		else if(!strcmp(m_command,__RISO_STARTED__))
		{
			if(meas_task_control & __MACH_MEAS_IN_PROG)
				mach_task_control |= __MACH_RISO_STARTED;
		}

		else if(!strcmp(m_command,__RISO_STOPPED__))
		{
			if(meas_task_control & __MACH_MEAS_IN_PROG)
				mach_task_control &= (~__MACH_RISO_STARTED);
		}
		else if(!strcmp(m_command,__MECH_RPE_START__))
		{
			if((meas_task_control & __MACH_MEAS_IN_PROG)&&(!(mach_task_control & __MACH_RPE_IN_PROGRESS)))
				set_event(MACH_RPE_START,MachinesRPEStart);
		}
		else if(!strcmp(m_command,__MECH_RPE_STOP__))
		{
			if((meas_task_control & __MACH_MEAS_IN_PROG)&&(mach_task_control & __MACH_RPE_IN_PROGRESS))
				set_event(MACH_RPE_STOP,MachinesRPEStop);
		}
		else if(!strcmp(m_command,__MACH_URES_START__))
		{
			if((meas_task_control & __MACH_MEAS_IN_PROG)&&(!(mach_task_control & __MACH_URES_IN_PROGRESS)))
			{
				if(!strcmp(&additionalCode[0][0][0],__MACH_L1_PE__))
					current_URES_measurement = __L1_PE;
				else if(!strcmp(&additionalCode[0][0][0],__MACH_L2_PE__))
					current_URES_measurement = __L2_PE;
				else if(!strcmp(&additionalCode[0][0][0],__MACH_L3_PE__))
					current_URES_measurement = __L3_PE;
				else if(!strcmp(&additionalCode[0][0][0],__MACH_L1_N__))
					current_URES_measurement = __L1_N;
				else if(!strcmp(&additionalCode[0][0][0],__MACH_L2_N__))
					current_URES_measurement = __L2_N;
				else if(!strcmp(&additionalCode[0][0][0],__MACH_L3_N__))
					current_URES_measurement = __L3_N;
				else if(!strcmp(&additionalCode[0][0][0],__MACH_L1_L2__))
					current_URES_measurement = __L1_L2;
				else if(!strcmp(&additionalCode[0][0][0],__MACH_L2_L3__))
					current_URES_measurement = __L2_L3;
				else if(!strcmp(&additionalCode[0][0][0],__MACH_L1_L3__))
					current_URES_measurement = __L1_L3;
				set_event(MACH_URES,mach_URES);
			}
		}
		else if(!strcmp(m_command,__MACH_URES_FINSHED__))
		{
			if((meas_task_control & __MACH_MEAS_IN_PROG) &&(mach_task_control & __MACH_URES_IN_PROGRESS))
			{
				set_event(MACH_URES_STOP,mach_URES_Stop);
			}
			
		}
		else if(!strcmp(m_command,__MACH_URES_OPEN__))
		{
			if((meas_task_control & __MACH_MEAS_IN_PROG)&&(mach_task_control & __MACH_URES_IN_PROGRESS))
				machOPENcontactors();
		}
		else if(!strcmp(m_command,__START_ANALYZE__))
		{
			if((meas_task_control & __MACH_MEAS_IN_PROG)&&(!(mach_task_control & __MACH_MAINS_ANALYZE_IN_PROGRESS)))
			{	
				mach_task_control |= __MACH_MAINS_ANALYZE_IN_PROGRESS;
				set_event(MAINS_ANALYZE_MEAS_START,startMainsMeasurement);
			}
		}
		else if(!strcmp(m_command,__STOP_ANALYZE__))
		{
			if((meas_task_control & __MACH_MEAS_IN_PROG)&&((mach_task_control & __MACH_MAINS_ANALYZE_IN_PROGRESS)))
			{	
				mach_task_control &= ~__MACH_MAINS_ANALYZE_IN_PROGRESS;
				set_event(MAINS_ANALYZE_MEAS_STOP,stopMainsMeasurement);
				WeldMachMt310_RelInit();
			}
		}
		else if(!strcmp(m_command,__GET_VOLTAGE__))
		{
			if((meas_task_control & __MACH_MEAS_IN_PROG)&&((mach_task_control & __MACH_MAINS_ANALYZE_IN_PROGRESS)))
			{	
				meas_task_control |= __RETURN_VOLTAGE;
				restart_timer(RETURN_MEASURED_RESULT,1,returnMeasuredTask);
			}
		}
		else if(!strcmp(m_command,__GET_CURRENT__))
		{
			if((meas_task_control & __MACH_MEAS_IN_PROG)&&((mach_task_control & __MACH_MAINS_ANALYZE_IN_PROGRESS)))
			{	
				meas_task_control |= __RETURN_CURRENT;
				restart_timer(RETURN_MEASURED_RESULT,1,returnMeasuredTask);
			}
		}
		else if(!strcmp(m_command,__GET_THD_C__))
		{
			if((meas_task_control & __MACH_MEAS_IN_PROG)&&((mach_task_control & __MACH_MAINS_ANALYZE_IN_PROGRESS)))
			{	
				meas_task_control |= __RETURN_THD_C;
				restart_timer(RETURN_MEASURED_RESULT,1,returnMeasuredTask);
			}
		}
		else if(!strcmp(m_command,__GET_THD_V__))
		{
			if((meas_task_control & __MACH_MEAS_IN_PROG)&&((mach_task_control & __MACH_MAINS_ANALYZE_IN_PROGRESS)))
			{	
				meas_task_control |= __RETURN_THD_V;
				restart_timer(RETURN_MEASURED_RESULT,1,returnMeasuredTask);
			}
		}
		else if(!strcmp(m_command,__GET_POWER_R__))
		{
			if((meas_task_control & __MACH_MEAS_IN_PROG)&&((mach_task_control & __MACH_MAINS_ANALYZE_IN_PROGRESS)))
			{	
				meas_task_control |= __RETURN_POWER_R;
				restart_timer(RETURN_MEASURED_RESULT,1,returnMeasuredTask);
			}
		}
		else if(!strcmp(m_command,__GET_POWER_A__))
		{
			if((meas_task_control & __MACH_MEAS_IN_PROG)&&((mach_task_control & __MACH_MAINS_ANALYZE_IN_PROGRESS)))
			{	
				meas_task_control |= __RETURN_POWER_A;
				restart_timer(RETURN_MEASURED_RESULT,1,returnMeasuredTask);
			}
		}
		else if(!strcmp(m_command,__GET_PF__))
		{
			if((meas_task_control & __MACH_MEAS_IN_PROG)&&((mach_task_control & __MACH_MAINS_ANALYZE_IN_PROGRESS)))
			{	
				meas_task_control |= __RETURN_PF;
				restart_timer(RETURN_MEASURED_RESULT,1,returnMeasuredTask);
			}
		}
		else if(!strcmp(m_command,"TEST"))
		{
				test4_on;
				if(temp_ind==false)
				{
					SET_L1_CONTACTOR;
					SET_L2_CONTACTOR;
					SET_L3_CONTACTOR;
					SET_N_CONTACTOR;
					temp_ind=true;
				}
				else
				{
					RST_L1_CONTACTOR;
					RST_L2_CONTACTOR;
					RST_L3_CONTACTOR;
					RST_N_CONTACTOR;
					HAL_Delay(1);
					temp_ind=false;
				}
				test4_off;
		}
		//namenjen testu s simulatorjem
		if(!strcmp(m_value,__MACH_TEST__))
			mach_task_control |= __MACH_TEST_RECIEVED;
		else
			mach_task_control &= ~__MACH_TEST_RECIEVED;
		
	}
/*********************************************************************************/
/**																	WELDING																			**/
/*********************************************************************************/
	//NEW
	else if(!strcmp(&additionalCode[1][0][0],&STD_COMMUNICATION_NAMES[1][0]))	//WELD
	{ 
		//STOP COMMAND - velja za vse, zato je na zacetk
		if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[1][0]))//STOP
		{ 
			if(meas_task_control & __WELD_MEAS_IN_PROG)	//se izvede samo ce je meritev v teku
				set_event(STOP_WELD,stop_weld);
		}
		//SET
		else if(!strcmp(m_function,&FUNCTION_COMMUNICATON_NAMES[3][0]))
		{
			if(!strcmp(m_command,&FUNCTION_COMMUNICATON_NAMES[3][0]))
			{
				if(!strcmp(&additionalCode[0][1][0],&FUNCTION_COMMUNICATON_NAMES[3][0]))
				{
					if(!strcmp(&additionalCode[1][1][0],__1_PHASE__))
					{
						set_phase_num_weld(1);
					}
					else if(!strcmp(&additionalCode[1][1][0],__3_PHASE__))
					{
						set_phase_num_weld(3);
					}
				}
			}
		}
		//ALL-PE
		else if(!strcmp(m_function,&FUNCTION_COMMUNICATON_NAMES[4][0]))
		{
			if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[0][0]))//START
			{
				//ce je katerakoli meritev v delu se ne izvede
				if(!(weld_task_control & WELD_IN_PROG_MASKS))
				{
					if(!strcmp(&additionalCode[1][1][0],__1_PHASE__))
					{
						set_phase_num_weld(1);
					}
					else if(!strcmp(&additionalCode[1][1][0],__3_PHASE__))
					{
						set_phase_num_weld(3);
					}
					weld_RISO_count=0;	//da zacne od zacetka
					SetWeldLimit(&additionalCode[1][2][0]);//nastavimo limito
					set_event(WELD_RISO_PHASES_TO_PE,weld_RISO_phasesToPE);
				}
			}
		}
		//ONE-PE
		else if(!strcmp(m_function,&FUNCTION_COMMUNICATON_NAMES[5][0]))
		{
			if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[0][0]))//START
			{
				//ce je katerakoli meritev v delu se ne izvede
				if(!(weld_task_control & MACH_IN_PROG_MASKS))
				{
					if(!strcmp(&additionalCode[1][1][0],__1_PHASE__))
					{
						set_phase_num_weld(1);
					}
					else if(!strcmp(&additionalCode[1][1][0],__3_PHASE__))
					{
						set_phase_num_weld(3);
					}
					SetWeldLimit(&additionalCode[1][2][0]);//nastavimo limito
					
					set_event(WELD_RISO_ONE_PHASE_TO_PE,weld_RISO_onePhaseToPE);
				}
			}
		}
		//RPE
		else if(!strcmp(m_function,&FUNCTION_COMMUNICATON_NAMES[9][0]))
		{
			if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[0][0]))//START
			{
				if((!(weld_task_control & WELD_IN_PROG_MASKS)))
					set_event(WELD_RPE_START,WeldingRPEStart);
			}
			if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[1][0]))//STOP
			{
				if((meas_task_control & __WELD_MEAS_IN_PROG)&&(weld_task_control & __WELD_RPE_IN_PROGRESS))
					set_event(WELD_RPE_STOP,WeldingRPEStop);
			}
		}
		//MAINS-WELD
		else if(!strcmp(m_function,&FUNCTION_COMMUNICATON_NAMES[12][0]))
		{
			if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[0][0]))//START
			{
				if((!(weld_task_control & WELD_IN_PROG_MASKS)))
				{
					set_event(WELD_RISO_MAINS_TO_WELD,weld_RISO_MainsToWeld);
				}
			}
		}
		//WELD-PE
		else if(!strcmp(m_function,&FUNCTION_COMMUNICATON_NAMES[14][0]))
		{
			if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[0][0]))//START
			{
				if((!(weld_task_control & WELD_IN_PROG_MASKS)))
				{	
					set_event(WELD_RISO_WELD_TO_PE,weld_RISO_WeldToPE);
				}
			}
		}
		//MAINS-CLASS2
		else if(!strcmp(m_function,&FUNCTION_COMMUNICATON_NAMES[13][0]))
		{
			if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[0][0]))//START
			{
				if((!(weld_task_control & WELD_IN_PROG_MASKS)))
				{	
					set_event(WELD_RISO_MAINS_TO_CLASS2,weld_RISO_MainsToClass2);
				}
			}
		}
		//UNL_RMS
		else if(!strcmp(m_function,&FUNCTION_COMMUNICATON_NAMES[15][0]))
		{
			if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[0][0]))//START
			{
				if((!(weld_task_control & WELD_IN_PROG_MASKS)))
				{	
					set_event(WELD_UNL_START_RMS,weld_UnlStart_RMS);
				}
			}
		}
		//UNL_PEAK
		else if(!strcmp(m_function,&FUNCTION_COMMUNICATON_NAMES[16][0]))
		{
			if(!strcmp(m_command,&COMMAND_COMMUNICATON_NAMES[0][0]))//START
			{
				if((!(weld_task_control & WELD_IN_PROG_MASKS)))
				{	
					set_event(WELD_UNL_START_PEAK,weld_UnlStart_peak);
				}
			}
		}
	}
	//OLD
	else if(!strcmp(m_function,__WELDING__))
	{ 
		if(!strcmp(m_command,__INIT_WELDING__))
		{
			if(!(meas_task_control & __WELD_MEAS_IN_PROG))	//ce je meritev ze v teku se ne zgodi nic
			{
				meas_task_control |= __WELD_MEAS_IN_PROG;
				if(!strcmp(m_value,__1_PHASE__))
					set_phase_num_weld(1);
				else
					set_phase_num_weld(3);
				start_weld_count = 0;
				set_event(INIT_WELD,init_weld);
				weld_task_control |= __WELD_INIT_RECIEVED;
			}
		}
		else if(!strcmp(m_command,__DEINIT_WELDING__))
		{
			if(weld_task_control & __WELD_INITIATED)
			{
				start_weld_count = 0;
				set_event(DEINIT_WELD,deinitWelding);
			}
		}
		else if(!strcmp(m_command,__SET__))
		{
			if(!strcmp(&additionalCode[0][0][0],__CABLE_TYPE__))
			{
				if(!strcmp(&additionalCode[1][0][0],__1_PHASE__))
				{
					set_phase_num_weld(1);
				}
				else if(!strcmp(&additionalCode[1][0][0],__3_PHASE__))
				{
					set_phase_num_weld(3);
				}
			}
		}
		else if(!strcmp(m_command,__START_PHASES_TO_PE__))
		{
			//ce je katerakoli meritev v delu se ne izvede
			if((meas_task_control & __WELD_MEAS_IN_PROG))
			{
				if(!(weld_task_control &__WELD_RISO_PHASES_TO_PE_IN_PROGRESS))
				{
					if(weld_task_control & __WELD_INITIATED)
					{
						weld_RISO_count = 0;			//nastavimo na 0, ker moramo ponovno izmeriti vse faze proti PE
						set_event(WELD_RISO_PHASES_TO_PE,weld_RISO_phasesToPE);
					}
					else if(!(meas_task_control & __WELD_MEAS_IN_PROG))	//ce je meritev ze v teku se ne zgodi nic
					{
						meas_task_control |= __WELD_MEAS_IN_PROG;
						start_cord_count = 0;
						weld_RISO_count = 0;			//nastavimo na 0, ker moramo ponovno izmeriti vse faze proti PE
						set_event(INIT_WELD,init_weld);
					}
				}
			}
		}
		else if(!strcmp(m_command,__START_ONE_PHASE_TO_PE__))
		{
			//ce je katerakoli meritev v delu se ne izvede
			if((meas_task_control & __WELD_MEAS_IN_PROG))
			{
				if(!(weld_task_control &__WELD_RISO_ONE_PHASE_TO_PE_IN_PROGRESS))
				{
					if(weld_task_control & __WELD_INITIATED)
					{
						set_event(WELD_RISO_ONE_PHASE_TO_PE,weld_RISO_onePhaseToPE);
					}
					else if(!(meas_task_control & __WELD_MEAS_IN_PROG))	//ce je meritev ze v teku se ne zgodi nic
					{
						meas_task_control |= __WELD_MEAS_IN_PROG;
						start_weld_count = 0;
						set_event(INIT_WELD,init_weld);
					}
				}
			}
		}
		else if(!strcmp(m_command,__STOP_WELD__))
		{
			if(meas_task_control & __WELD_MEAS_IN_PROG)	//se izvede samo ce je meritev v teku
				set_event(STOP_WELD,stop_weld);
		}
		else if(!strcmp(m_command,__RISO_RESISTANCE__))
		{
			if((meas_task_control & __WELD_MEAS_IN_PROG) && (weld_task_control & __WELD_RISO_RES_REQUESTED))	//se izvede samo ce je meritev v teku
				set_RISO_weld_resistance(&additionalCode[1][0][0]);
		}
		else if(!strcmp(m_command,__RISO_STARTED__))
		{
			if(meas_task_control & __WELD_MEAS_IN_PROG)
				weld_task_control |= __WELD_RISO_STARTED;
		}

		else if(!strcmp(m_command,__RISO_STOPPED__))
		{
			if(meas_task_control & __WELD_MEAS_IN_PROG)
				weld_task_control &= (~__WELD_RISO_STARTED);
		}
		else if(!strcmp(m_command,__WELD_RPE_START__))
		{
			if((meas_task_control & __WELD_MEAS_IN_PROG)&&(!(weld_task_control & __WELD_RPE_IN_PROGRESS)))
				set_event(WELD_RPE_START,WeldingRPEStart);
		}
		else if(!strcmp(m_command,__WELD_RPE_STOP__))
		{
			if((meas_task_control & __WELD_MEAS_IN_PROG)&&(weld_task_control & __WELD_RPE_IN_PROGRESS))
				set_event(WELD_RPE_STOP,WeldingRPEStop);
		}
		else if(!strcmp(m_command,__MAINS_TO_WELD_START__))
		{
			if((meas_task_control & __WELD_MEAS_IN_PROG)&&(!(weld_task_control & __WELD_RISO_MAINS_WELD_IN_PROGRESS)))
			{
				if(!strcmp(&additionalCode[0][0][0],__CONTINIOUS__))
					weld_task_control |= __WELD_RISO_CONTINIOUS_MEAS;
				else
					weld_task_control &= ~__WELD_RISO_CONTINIOUS_MEAS;
				set_event(WELD_RISO_MAINS_TO_WELD,weld_RISO_MainsToWeld);
			}
		}
		else if(!strcmp(m_command,__MAINS_TO_WELD_STOP__))
		{
			if((meas_task_control & __WELD_MEAS_IN_PROG)&&((weld_task_control & __WELD_RISO_MAINS_WELD_IN_PROGRESS)))
			{	
				set_event(WELD_RISO_MAINS_TO_WELD_STOP,weld_RISO_MainsToWeld_Stop);
			}
		}
		else if(!strcmp(m_command,__WELD_TO_PE_START__))
		{
			if((meas_task_control & __WELD_MEAS_IN_PROG)&&(!(weld_task_control & __WELD_RISO_WELD_PE_IN_PROGRESS)))
			{	
				if(!strcmp(&additionalCode[0][0][0],__CONTINIOUS__))
					weld_task_control |= __WELD_RISO_CONTINIOUS_MEAS;
				else
					weld_task_control &= ~__WELD_RISO_CONTINIOUS_MEAS;
				set_event(WELD_RISO_WELD_TO_PE,weld_RISO_WeldToPE);
			}
		}
		else if(!strcmp(m_command,__WELD_TO_PE_STOP__))
		{
			if((meas_task_control & __WELD_MEAS_IN_PROG)&&((weld_task_control & __WELD_RISO_WELD_PE_IN_PROGRESS)))
			{	
				set_event(WELD_RISO_WELD_TO_PE_STOP,weld_RISO_WeldToPE_Stop);
			}
		}
		else if(!strcmp(m_command,__MAINS_TO_CLASS2_START__))
		{
			if((meas_task_control & __WELD_MEAS_IN_PROG)&&(!(weld_task_control & __WELD_RISO_MAINS_CLASS2_IN_PROGRESS)))
			{	
				if(!strcmp(&additionalCode[0][0][0],__CONTINIOUS__))
					weld_task_control |= __WELD_RISO_CONTINIOUS_MEAS;
				else
					weld_task_control &= ~__WELD_RISO_CONTINIOUS_MEAS;
				set_event(WELD_RISO_MAINS_TO_CLASS2,weld_RISO_MainsToClass2);
			}
		}
		else if(!strcmp(m_command,__MAINS_TO_CLASS2_STOP__))
		{
			if((meas_task_control & __WELD_MEAS_IN_PROG)&&((weld_task_control & __WELD_RISO_MAINS_CLASS2_IN_PROGRESS)))
			{	
				set_event(WELD_RISO_MAINS_TO_CLASS2_STOP,weld_RISO_MainsToClass2_Stop);
			}
		}
		else if(!strcmp(m_command,__WELD_UNL_RMS_START_))
		{
			if((meas_task_control & __WELD_MEAS_IN_PROG)&&(!(weld_task_control & __WELD_UNL_RMS_IN_PROGRESS)))
			{	
				set_event(WELD_UNL_START_RMS,weld_UnlStart_RMS);
			}
		}

		else if(!strcmp(m_command,__WELD_UNL_RMS_STOP_))
		{
			if((meas_task_control & __WELD_MEAS_IN_PROG)&&((weld_task_control & __WELD_UNL_RMS_IN_PROGRESS)))
			{	
				set_event(WELD_UNL_STOP_RMS,weld_UnlStop_RMS);
			}
		}
		else if(!strcmp(m_command,__WELD_UNL_PEAK_START_))
		{
			if((meas_task_control & __WELD_MEAS_IN_PROG)&&(!(weld_task_control & __WELD_UNL_PEAK_IN_PROGRESS)))
			{	
				set_event(WELD_UNL_START_PEAK,weld_UnlStart_peak);
			}
		}

		else if(!strcmp(m_command,__WELD_UNL_PEAK_STOP_))
		{
			if((meas_task_control & __WELD_MEAS_IN_PROG)&&((weld_task_control & __WELD_UNL_PEAK_IN_PROGRESS)))
			{	
				set_event(WELD_UNL_STOP_PEAK,weld_UnlStop_peak);
			}
		}
		else if(!strcmp(m_command,__START_ANALYZE__))
		{
			if((meas_task_control & __WELD_MEAS_IN_PROG)&&(!(weld_task_control & __WELD_MAINS_ANALYZE_IN_PROGRESS)))
			{	
				weld_task_control |= __WELD_MAINS_ANALYZE_IN_PROGRESS;
				set_event(MAINS_ANALYZE_MEAS_START,startMainsMeasurement);
			}
		}
		else if(!strcmp(m_command,__STOP_ANALYZE__))
		{
			if((meas_task_control & __WELD_MEAS_IN_PROG)&&((weld_task_control & __WELD_MAINS_ANALYZE_IN_PROGRESS)))
			{	
				weld_task_control &= ~__WELD_MAINS_ANALYZE_IN_PROGRESS;
				set_event(MAINS_ANALYZE_MEAS_STOP,stopMainsMeasurement);
				WeldMachMt310_RelInit();
			}
		}
		else if(!strcmp(m_command,__GET_VOLTAGE__))
		{
			if((meas_task_control & __WELD_MEAS_IN_PROG)&&((weld_task_control & __WELD_MAINS_ANALYZE_IN_PROGRESS)))
			{	
				meas_task_control |= __RETURN_VOLTAGE;
				restart_timer(RETURN_MEASURED_RESULT,1,returnMeasuredTask);
			}
		}
		else if(!strcmp(m_command,__GET_CURRENT__))
		{
			if((meas_task_control & __WELD_MEAS_IN_PROG)&&((weld_task_control & __WELD_MAINS_ANALYZE_IN_PROGRESS)))
			{	
				meas_task_control |= __RETURN_CURRENT;
				restart_timer(RETURN_MEASURED_RESULT,1,returnMeasuredTask);
			}
		}
		else if(!strcmp(m_command,__GET_THD_C__))
		{
			if((meas_task_control & __WELD_MEAS_IN_PROG)&&((weld_task_control & __WELD_MAINS_ANALYZE_IN_PROGRESS)))
			{	
				meas_task_control |= __RETURN_THD_C;
				restart_timer(RETURN_MEASURED_RESULT,1,returnMeasuredTask);
			}
		}
		else if(!strcmp(m_command,__GET_THD_V__))
		{
			if((meas_task_control & __WELD_MEAS_IN_PROG)&&((weld_task_control & __WELD_MAINS_ANALYZE_IN_PROGRESS)))
			{	
				meas_task_control |= __RETURN_THD_V;
				restart_timer(RETURN_MEASURED_RESULT,1,returnMeasuredTask);
			}
		}
		else if(!strcmp(m_command,__GET_POWER_R__))
		{
			if((meas_task_control & __WELD_MEAS_IN_PROG)&&((weld_task_control & __WELD_MAINS_ANALYZE_IN_PROGRESS)))
			{	
				meas_task_control |= __RETURN_POWER_R;
				restart_timer(RETURN_MEASURED_RESULT,1,returnMeasuredTask);
			}
		}
		else if(!strcmp(m_command,__GET_POWER_A__))
		{
			if((meas_task_control & __WELD_MEAS_IN_PROG)&&((weld_task_control & __WELD_MAINS_ANALYZE_IN_PROGRESS)))
			{	
				meas_task_control |= __RETURN_POWER_A;
				restart_timer(RETURN_MEASURED_RESULT,1,returnMeasuredTask);
			}
		}
		else if(!strcmp(m_command,__GET_PF__))
		{
			if((meas_task_control & __WELD_MEAS_IN_PROG)&&((weld_task_control & __WELD_MAINS_ANALYZE_IN_PROGRESS)))
			{	
				meas_task_control |= __RETURN_PF;
				restart_timer(RETURN_MEASURED_RESULT,1,returnMeasuredTask);
			}
		}
		else if(!strcmp(m_command,"TEST"))
		{
				test4_on;
				if(temp_ind==false)
				{
					SET_L1_CONTACTOR;
					SET_L2_CONTACTOR;
					SET_L3_CONTACTOR;
					SET_N_CONTACTOR;
					temp_ind=true;
				}
				else
				{
					RST_L1_CONTACTOR;
					RST_L2_CONTACTOR;
					RST_L3_CONTACTOR;
					RST_N_CONTACTOR;
					HAL_Delay(1);
					temp_ind=false;
				}
				test4_off;
		}
		//namenjen testu s simulatorjem
		if(!strcmp(m_value,__WELD_TEST__))
			weld_task_control |= __WELD_TEST_RECIEVED;
		else
			weld_task_control &= ~__WELD_TEST_RECIEVED;
		
	}
/*********************************************************************************/
/**																		CALIBRATION																**/
/*********************************************************************************/
	else if(!strcmp(m_function,__CALIB__))
	{ 
		if(!strcmp(m_command,__MEASURE_ULN1__))
		{
			if((!(meas_task_control &__MEAS_TASKS_IN_PROG_MASK))&&(!(meas_control &__MEAS_IN_PROGRESS)))
			{
				calib_task_control |= __CALIB_ULN1_REQUESTED;
				set_event(CALIBRATION_MEAS_TASK,calibMeasure);
			}
		}
		else if(!strcmp(m_command,__MEASURE_ULN2__))
		{
			if((!(meas_task_control &__MEAS_TASKS_IN_PROG_MASK))&&(!(meas_control &__MEAS_IN_PROGRESS)))
			{
				calib_task_control |= __CALIB_ULN2_REQUESTED;
				set_event(CALIBRATION_MEAS_TASK,calibMeasure);
			}
		}
		else if(!strcmp(m_command,__MEASURE_ULN3__))
		{
			if((!(meas_task_control &__MEAS_TASKS_IN_PROG_MASK))&&(!(meas_control &__MEAS_IN_PROGRESS)))
			{
				calib_task_control |= __CALIB_ULN3_REQUESTED;
				set_event(CALIBRATION_MEAS_TASK,calibMeasure);
			}
		}
		else if(!strcmp(m_command,__MEASURE_UL1PE__))
		{
			if((!(meas_task_control &__MEAS_TASKS_IN_PROG_MASK))&&(!(meas_control &__MEAS_IN_PROGRESS)))
			{
				calib_task_control |= __CALIB_UL1PE_REQUESTED;
				set_event(CALIBRATION_MEAS_TASK,calibMeasure);
			}
		}
		else if(!strcmp(m_command,__MEASURE_UNPE__))
		{
			if((!(meas_task_control &__MEAS_TASKS_IN_PROG_MASK))&&(!(meas_control &__MEAS_IN_PROGRESS)))
			{
				calib_task_control |= __CALIB_UNPE_REQUESTED;
				set_event(CALIBRATION_MEAS_TASK,calibMeasure);
			}
		}
		else if(!strcmp(m_command,__MEASURE_IL1__))
		{
			if((!(meas_task_control &__MEAS_TASKS_IN_PROG_MASK))&&(!(meas_control &__MEAS_IN_PROGRESS)))
			{
				calib_task_control |= __CALIB_IL1_REQUESTED;
				set_event(CALIBRATION_MEAS_TASK,calibMeasure);
			}
		}
		else if(!strcmp(m_command,__MEASURE_IL2__))
		{
			if((!(meas_task_control &__MEAS_TASKS_IN_PROG_MASK))&&(!(meas_control &__MEAS_IN_PROGRESS)))
			{
				calib_task_control |= __CALIB_IL2_REQUESTED;
				set_event(CALIBRATION_MEAS_TASK,calibMeasure);
			}
		}
		else if(!strcmp(m_command,__MEASURE_IL3__))
		{
			if((!(meas_task_control &__MEAS_TASKS_IN_PROG_MASK))&&(!(meas_control &__MEAS_IN_PROGRESS)))
			{
				calib_task_control |= __CALIB_IL3_REQUESTED;
				set_event(CALIBRATION_MEAS_TASK,calibMeasure);
			}
		}
		else if(!strcmp(m_command,__CALIB_GET_CONSTANTS__))
		{
			set_event(TRANSMIT_CALIB_CONSTANTS,transmittCalibConstants);
		}
		else if(!strcmp(m_command,__CONSTANTS__))
		{
			setConstant(&additionalCode[0][0][0],&additionalCode[1][0][0]);
			setConstant(&additionalCode[0][1][0],&additionalCode[1][1][0]);
		}
	}
/*******************************************************************************/
/**															ADITIONAL SETTINGS														**/
/*******************************************************************************/
	else
	{
		return false;
	}		
	
	
}

void transmitCommFunc(uint32_t func, uint32_t command, uint32_t add,char* leftover, int  dir)
{
	char temp_str1[10];
	char temp_str2[10];
	char temp_str3[20];
	switch(func)
	{
		case __RPE:
			snprintf(temp_str1,9,"%s",&FUNCTION_COMMUNICATON_NAMES[0][0]);
			break;
		case __RISO:
			snprintf(temp_str1,9,"%s",&FUNCTION_COMMUNICATON_NAMES[7][0]);
			break;
		default:
			snprintf(temp_str1,9,"");
	}
	switch(command)
	{
		case __START:
			snprintf(temp_str2,9,"%s",&COMMAND_COMMUNICATON_NAMES[0][0]);
			break;
		case __STOP:
			snprintf(temp_str2,9,"%s",&COMMAND_COMMUNICATON_NAMES[1][0]);
			break;
		case __GET:
			snprintf(temp_str2,9,"%s",&COMMAND_COMMUNICATON_NAMES[4][0]);
			break;
		default:
			snprintf(temp_str2,9,"");
	}
	switch(add)
	{
		case __LOW:
			snprintf(temp_str3,19,"CURRENT|%s",&ADD_COMMUNICATON_NAMES2[0][0]);
			break;
		case __MID:
			snprintf(temp_str3,19,"CURRENT|%s",&ADD_COMMUNICATON_NAMES2[1][0]);
			break;
		case __HIGH:
			snprintf(temp_str3,19,"CURRENT|%s",&ADD_COMMUNICATON_NAMES2[2][0]);
			break;
		case __RESULT:
			snprintf(temp_str3,19,"%s",&ADD_COMMUNICATON_NAMES2[3][0]);
			break;
		case __500V:
			snprintf(temp_str3,19,"VOLTAGE|%s",&ADD_COMMUNICATON_NAMES2[4][0]);
			break;
		default:
			snprintf(temp_str3,9,"");
	}
	SendComMessage(_ON,_ID_TFA,device.device_ID,temp_str1,temp_str2,temp_str3,leftover,dir);
}

void transmitFunc(uint32_t func, uint32_t command,char * add,char* leftover, int  dir)
{
	char temp_str1[10];
	char temp_str2[10];
	switch(func)
	{
		case __C_W:
			snprintf(temp_str1,9,"%s",&FUNCTION_COMMUNICATON_NAMES[1][0]);
			break;
		case __ALL_PE:
			snprintf(temp_str1,9,"%s",&FUNCTION_COMMUNICATON_NAMES[4][0]);
			break;
		case __ONE_PE:
			snprintf(temp_str1,9,"%s",&FUNCTION_COMMUNICATON_NAMES[5][0]);
			break;
		case __PH_PH:
			snprintf(temp_str1,9,"%s",&FUNCTION_COMMUNICATON_NAMES[6][0]);
			break;
		case __CONT:
			snprintf(temp_str1,9,"%s",&FUNCTION_COMMUNICATON_NAMES[2][0]);
			break;
		case __M_URES:
			snprintf(temp_str1,9,"%s",&FUNCTION_COMMUNICATON_NAMES[10][0]);
			break;
		case __UNL_RMS:
			snprintf(temp_str1,9,"%s",&FUNCTION_COMMUNICATON_NAMES[15][0]);
			break;
		case __UNL_PEAK:
			snprintf(temp_str1,9,"%s",&FUNCTION_COMMUNICATON_NAMES[16][0]);
			break;
		default:
			snprintf(temp_str1,9,"");
	}
	switch(command)
	{
		case __RESULT:
			snprintf(temp_str2,10,"%s",&COMMAND_COMMUNICATON_NAMES2[1][0]);
			break;
		case __INITIATED:
			snprintf(temp_str2,10,"%s",&COMMAND_COMMUNICATON_NAMES2[2][0]);
			break;
		case __OPENED:
			snprintf(temp_str2,10,"%s",&COMMAND_COMMUNICATON_NAMES2[3][0]);
			break;
		case __STARTED:
			snprintf(temp_str2,10,"%s",&COMMAND_COMMUNICATON_NAMES2[4][0]);
			break;
		default:
			snprintf(temp_str2,9,"");
	}
	SendComMessage(_ON,_ID_TFA,device.device_ID,temp_str1,temp_str2,add,leftover,dir);
}
void transmitEvent(uint32_t event,uint32_t std, char * add, int dir)
{
	char temp_str1[10];
	char temp_str2[10];
	char temp_str3[10];
	snprintf(temp_str3,9,"%s",&FUNCTION_COMMUNICATON_NAMES[8][0]);
	switch(event)
	{
		case __STOPPED_EN:
			snprintf(temp_str1,9,"%s",&EVENT_COMMUNICATON_NAMES[0][0]);
			break;
		default:
			snprintf(temp_str1,9,"");
	}
	switch(std)
	{
		case __CORD_EN:
			snprintf(temp_str2,9,"STD|%s%s",&STD_COMMUNICATION_NAMES[0][0],add);
			break;
		case __MACHINES_EN:
			snprintf(temp_str2,9,"STD|%s%s",&STD_COMMUNICATION_NAMES[2][0],add);
			break;
		case __WELDING_EN:
			snprintf(temp_str2,9,"STD|%s%s",&STD_COMMUNICATION_NAMES[1][0],add);
			break;
		default:
			snprintf(temp_str2,9,"");
	}
	SendComMessage(_ON,_ID_TFA,device.device_ID,temp_str3,temp_str1,temp_str2,"",dir);
}