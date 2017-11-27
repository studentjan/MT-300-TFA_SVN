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
 #include "serial_RX.h"


extern uint8_t ser_ID_sender;
extern uint8_t last_recieved_dir;
extern uint32_t global_control;
 SPI_HandleTypeDef hspi1;
 UART_HandleTypeDef huart3;
 uint32_t meas_control = 0;

uint8_t event_status; 	




USART2_RX_t USART2_queue_buffer[UART2_QUEUE_SIZE]; //array of UART3 RX commands
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
		init_command_queue();
	}
	else 
	{
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
	static uint32_t cnt=0;
	if(cnt>=5)
	{
		if(meas_control & __IDIFF_MASK)
		{
			float temp;
			temp=get_value(__IDIFF);
			if(!(temp != temp)){
				sprintf(str, "%.4f",temp);	//PAZI NEVARNO
			}							//sprintf vzame cca 200us
			send_com_message(__POWER__,__IDIFF__,_VALID,str,_UART_DIR_USB); //posiljanje vzame cca 600us
		}	//skupaj traja if stavek cca 800us
		if(meas_control & __ULN1_MASK)
		{
			float temp;
			temp=get_value(__ULN1);
			if(!(temp != temp)){
				sprintf(str, "%.4f",temp);	//PAZI NEVARNO
			}
			send_com_message(__POWER__,__ULN1__,_VALID,str,_UART_DIR_USB);
		}
		if(meas_control & __ULN2_MASK)
		{
			float temp;
//			char Msg[] = "ULN2  = ";
			temp=get_value(__ULN2);
			if(!(temp != temp)){
				sprintf(str, "%.4f",temp);	//PAZI NEVARNO
			}
			send_com_message(__POWER__,__ULN2__,_VALID,str,_UART_DIR_USB);
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
			send_com_message(__POWER__,__ULN3__,_VALID,str,_UART_DIR_USB);
		}
		if(meas_control & __IL1_MASK)
		{
			float temp;
			test2_on;
			temp=get_value(__IL1);
			if(!(temp != temp)){
				sprintf(str, "%.5f",temp);	//PAZI NEVARNO
			}
			test3_on;
			send_com_message(__POWER__,__IL1__,_VALID,str,_UART_DIR_USB);
			test3_off;
			test2_off;
		}
		if(meas_control & __IL2_MASK)
		{
			float temp;
			temp=get_value(__IL2);
			if(!(temp != temp)){
				sprintf(str, "%.5f",temp);	//PAZI NEVARNO
			}
			send_com_message(__POWER__,__IL2__,_VALID,str,_UART_DIR_USB);
			
		}
		if(meas_control & __IL3_MASK)
		{
			float temp;
			temp=get_value(__IL3);
			if(!(temp != temp)){
				sprintf(str, "%.5f",temp);	//PAZI NEVARNO
			}
			send_com_message(__POWER__,__IL3__,_VALID,str,_UART_DIR_USB);
		}
		if(meas_control & __UL1PE_MASK)
		{
			float temp;
			temp=get_value(__UL1PE);
			if(!(temp != temp)){
				sprintf(str, "%.4f",temp);	//PAZI NEVARNO
			}
			send_com_message(__POWER__,__UL1PE__,_VALID,str,_UART_DIR_USB);
		}
		if(meas_control & __UNPE_MASK)
		{
			float temp;
			temp=get_value(__UNPE);
			if(!(temp != temp)){
				sprintf(str, "%.4f",temp);	//PAZI NEVARNO
			}
			send_com_message(__POWER__,__UNPE__,_VALID,str,_UART_DIR_USB);
		}
		cnt=0;
	}
	else cnt++;
	restart_timer(TRANSMIT_RESULTS,30,Transmit_results_task);	
	//za 6 posiljanj rab 1,28 ms torej za eno 0,213 ms
}
//inicializacija cakalne vrste komand
void init_command_queue(void)
{
 uint8_t queue_cnt;
 
 event_status=QUEUE_PASS; //first do event is enabled 		
	
 queue_cnt=0;
 while(queue_cnt<UART2_QUEUE_SIZE)
  {
   USART2_queue_buffer[queue_cnt].command_ID=QUEUE_FREE;
   queue_cnt++;
  }
}
//commands do events
//gre po vseh slotih v polju in najde command z najmanjsim indeksom in izvede ta command string
//to ponavlja dokler je v polju prisotna vsaj en command
//ko sem prisel v to funkcijo je zanesljivo vsaj en command v polju
void command_do_events(void)
{
	  
 
 #define QUEUEUE_INDEX_MAX		255
	
 uint8_t queue_cnt;	 
 uint8_t queue_index;	
 uint8_t queue_index_min;		
 event_status=QUEUE_FAIL;
 while(event_status==QUEUE_FAIL)
   {
    
		//find pointer to the command with minimum index - oldest command, first to do event
		//--------------------------------------      
		queue_index=QUEUEUE_INDEX_MAX;    
    for(queue_cnt=0;queue_cnt<UART2_QUEUE_SIZE;queue_cnt++)
     {
      if(USART2_queue_buffer[queue_cnt].command_ID!=QUEUE_FREE) 
			 {
			  if(USART2_queue_buffer[queue_cnt].command_ID<queue_index)
				 {
					queue_index=USART2_queue_buffer[queue_cnt].command_ID;
					queue_index_min=queue_cnt; 
				 } 
			 }
     }
    //--------------------------------------      
	   	
		//ce je queue_index ostal na QUEUEUE_INDEX_MAX, potem v polju ni nobenega ukaza vec in lahko prekinemo while zanko
		//drugace pa izvedemo ukaz na katerega kaze pointer queue_index_min in polje nardimo prosto
    //--------------------------------------      
    if(queue_index!=QUEUEUE_INDEX_MAX)
		 {
		  recieved_command_analyze(USART2_queue_buffer[queue_index_min].command); 			
			USART2_queue_buffer[queue_index_min].command_ID=QUEUE_FREE; 
		 }
		else
		 {
		  event_status=QUEUE_PASS;
		 }			 
    //--------------------------------------       		 
		 		      
  }
}
//UART2 command analyze
void recieved_command_analyze(char *command)
{
	//zaenkat se zna bit problem, ce poslje hkrati po dveh vodilih npr PAT in MT skupaj
 uint8_t ans; 
 ans=command_parser(command,last_recieved_dir);  
 if(ans==_ANS_MESSAGE_ACK) command_analyze(last_recieved_dir);  //message analyze               
 command_return(ans,last_recieved_dir);   
	//ce je kaj narobe z vhodno povezavo ne pustimo izvajanja ukazov in vrnemo string, ki pove kaj je narobe
}	
void send_warning_MSG(void)
{
	uint8_t direction;
	if(global_control & __INIT_TEST_FAIL)
	{
		if(ser_ID_sender==_ID_MT) direction=_UART_DIR_485;
		else if(ser_ID_sender==_ID_PAT) direction=_UART_DIR_USB;
		else if(ser_ID_sender==_ID_DEBUG) direction=_UART_DIR_USB;
		if(global_control & __PE_DISCONNECTED) send_com_message("WARNING","PE DISCONNECTED",_NAN," ",direction);
		else if(global_control & __N_DISCONECTED) send_com_message("WARNING","N DISCONNECTED",_NAN," ",direction);
		else if(global_control & __WRONG_CONNECTION) send_com_message("WARNING","WRONG CONNECTION",_NAN," ",direction);
	}
}
