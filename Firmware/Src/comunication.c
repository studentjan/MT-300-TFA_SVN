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

extern uint32_t connection_control;
extern uint8_t ser_ID_sender;
extern char transmitter_ID;
extern uint32_t global_control;
 SPI_HandleTypeDef hspi1;
 UART_HandleTypeDef huart3;
 uint32_t meas_control = 0;
extern uint32_t connection_control;

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
		USB_PU_ON; //VKLOPI PU na USB liniji (na racunalniku mi z usb B konektorjem brez tega ni delal)
		serial_com_init();
	}
	else 
	{
		//povezava ni vec vzpostavljena
		disconnect_function(__CON_TO_MT310);
		usb_connected = _NO;
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
	char str[30];
	char str_out[50];
	static uint32_t cnt=0;
	if(cnt>=5)
	{
		if(meas_control & __IDIFF_MASK)
		{
			float temp;
			temp=get_value(__IDIFF);
			if(!(temp != temp)){
				sprintf(str, "%.4f",temp);	//PAZI NEVARNO
			}
			strcat(str_out,__IDIFF__);
			str_out[strlen(__IDIFF__)]= '|';
			strcat(str_out,str);
			SendComMessage(_ON,_ID_TFA,0,__MT_300__,__POWER__,str_out,"",_UART_DIR_USB);
		}
		if(meas_control & __ULN1_MASK)
		{
			float temp;
			temp=get_value(__ULN1);
			if(!(temp != temp)){
				sprintf(str, "%.4f",temp);	//PAZI NEVARNO
			}
			strcat(str_out,__ULN1__);
			str_out[strlen(__ULN1__)]= '|';
			strcat(str_out,str);
			SendComMessage(_ON,_ID_TFA,0,__MT_300__,__POWER__,str_out,"",_UART_DIR_USB);
		}
		if(meas_control & __ULN2_MASK)
		{
			float temp;
//			char Msg[] = "ULN2  = ";
			temp=get_value(__ULN2);
			if(!(temp != temp)){
				sprintf(str, "%.4f",temp);	//PAZI NEVARNO
			}
			strcat(str_out,__ULN2__);
			str_out[strlen(__ULN2__)]= '|';
			strcat(str_out,str);
			SendComMessage(_ON,_ID_TFA,0,__MT_300__,__POWER__,str_out,"",_UART_DIR_USB);
//			snprintf(str_out, sizeof str_out,"%s%8s V\r\n", Msg, str);
//			//CDC_Transmit_FS(Msg, strlen(Msg));
//			CDC_Transmit_FS((uint8_t*)str_out, 20);
		}
		if(meas_control & __ULN3_MASK)
		{
			float temp;
			temp=get_value(__ULN3);
			if(!(temp != temp)){
				sprintf(str, "%.4f",temp);	//PAZI NEVARNO
			}
			strcat(str_out,__ULN3__);
			str_out[strlen(__ULN3__)]= '|';
			strcat(str_out,str);
			SendComMessage(_ON,_ID_TFA,0,__MT_300__,__POWER__,str_out,"",_UART_DIR_USB);
		}
		if(meas_control & __IL1_MASK)
		{
			float temp;
			temp=get_value(__IL1);
			if(!(temp != temp)){
				sprintf(str, "%.5f",temp);	//PAZI NEVARNO
			}
			strcat(str_out,__IL1__);
			str_out[strlen(__IL1__)]= '|';
			strcat(str_out,str);
			SendComMessage(_ON,_ID_TFA,0,__MT_300__,__POWER__,str_out,"",_UART_DIR_USB);
		}
		if(meas_control & __IL2_MASK)
		{
			float temp;
			temp=get_value(__IL2);
			if(!(temp != temp)){
				sprintf(str, "%.5f",temp);	//PAZI NEVARNO
			}
			strcat(str_out,__IL2__);
			str_out[strlen(__IL2__)]= '|';
			strcat(str_out,str);
			SendComMessage(_ON,_ID_TFA,0,__MT_300__,__POWER__,str_out,"",_UART_DIR_USB);			
		}
		if(meas_control & __IL3_MASK)
		{
			float temp;
			temp=get_value(__IL3);
			if(!(temp != temp)){
				sprintf(str, "%.5f",temp);	//PAZI NEVARNO
			}
			strcat(str_out,__IL3__);
			str_out[strlen(__IL3__)]= '|';
			strcat(str_out,str);
			SendComMessage(_ON,_ID_TFA,0,__MT_300__,__POWER__,str_out,"",_UART_DIR_USB);
		}
		if(meas_control & __UL1PE_MASK)
		{
			float temp;
			temp=get_value(__UL1PE);
			if(!(temp != temp)){
				sprintf(str, "%.4f",temp);	//PAZI NEVARNO
			}
			strcat(str_out,__UL1PE__);
			str_out[strlen(__UL1PE__)]= '|';
			strcat(str_out,str);
			SendComMessage(_ON,_ID_TFA,0,__MT_300__,__POWER__,str_out,"",_UART_DIR_USB);
		}
		if(meas_control & __UNPE_MASK)
		{
			float temp;
			temp=get_value(__UNPE);
			if(!(temp != temp)){
				sprintf(str, "%.4f",temp);	//PAZI NEVARNO
			}
			strcat(str_out,__UNPE__);
			str_out[strlen(__UNPE__)]= '|';
			strcat(str_out,str);
			SendComMessage(_ON,_ID_TFA,0,__MT_300__,__POWER__,str_out,"",_UART_DIR_USB);
		}
		cnt=0;
	}
	else cnt++;
	restart_timer(TRANSMIT_RESULTS,30,Transmit_results_task);	
	//za 6 posiljanj rab 1,28 ms torej za eno 0,213 ms
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
		if(global_control & __PE_DISCONNECTED) SendComMessage(_ON,_ID_TFA,0,__MT_300__,__WARNING__,"PE DISCONNECTED"," ",direction);
		else if(global_control & __N_DISCONECTED) SendComMessage(_ON,_ID_TFA,0,__MT_300__,__WARNING__,"N DISCONNECTED"," ",direction);
		else if(global_control & __WRONG_CONNECTION) SendComMessage(_ON,_ID_TFA,0,__MT_300__,__WARNING__,"WRONG CONNECTION"," ",direction);
	}
}

//v intervalu preverjamo vzpostavljeno komunikacijo tako, da nam naprava vraca
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
	if(temp_connection_control & __CON_TO_MT310)
		connection_control &= (~__CON_TO_MT310);//zaenkrat je mt310 simuliran v racunalniku, zato je povezava preko USB, kasneje bo RS 485
	else if(temp_connection_control & __CON_TO_TERMINAL)
		connection_control &= (~__CON_TO_TERMINAL);
	else if(temp_connection_control & __CON_TO_PAT)
		connection_control &= (~__CON_TO_PAT);
	
	if(!(connection_control & (__CON_TO_MT310|__CON_TO_TERMINAL|__CON_TO_PAT)))//preverimo se ce je vzpostavljena kaksna druga povezava cene zapremo komunikacijo
			connection_control &= (~__CONNECTION_ESTABLISHED);
}
