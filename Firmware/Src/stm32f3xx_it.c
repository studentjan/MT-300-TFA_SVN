/**
  ******************************************************************************
  * @file    stm32f3xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"
#include "stm32f3xx.h"
#include "stm32f3xx_it.h"
#include "os.h"
#include "test.h"
#include "sdadc.h"
#include "timer.h"
#include "comunication.h"
#include "do_task.h"
#include "usbd_cdc.h"
#include "usbd_desc.h"
#include "usbd_ioreq.h"
#include "machines.h"
#include "serial_com.h"
#include "com_meas_tasks.h"
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern PCD_HandleTypeDef hpcd_USB_FS;
extern SDADC_HandleTypeDef hsdadc1;
extern SDADC_HandleTypeDef hsdadc2;
extern SDADC_HandleTypeDef hsdadc3;
extern uint32_t meas_control;
extern uint32_t global_control;
extern uint32_t compute_control;
extern uint32_t compute_control2;
extern uint32_t interrupt_control;
extern TIM_HandleTypeDef htim7;
extern USBD_HandleTypeDef hUsbDeviceFS;
extern uint32_t il_thd_channel;
extern uint32_t mach_task_control;
extern uint32_t current_URES_measurement;
extern uint32_t serialComErrorTimeout;
extern uint32_t synchro_interrupt_control;
uint32_t stevec3;
uint32_t global;
uint32_t global2;
	uint8_t HiMsg[100];

/******************************************************************************/
/*            Cortex-M4 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/
void Hard_Fault_Handler(uint32_t stack[]);
/**
* @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

void printErrorMsg(const char * errMsg)
{
	CDC_Transmit_FS((uint8_t *)errMsg, strlen(errMsg));
//   while(*errMsg != '\0'){
//      ITM_SendChar(*errMsg);
//      ++errMsg;
//   }
}

void printUsageErrorMsg(uint32_t CFSRValue)
{
   printErrorMsg("Usage fault: ");
		//CDC_Transmit_FS((uint8_t *)"Usage fault: ", 16);
   CFSRValue >>= 16;                  // right shift to lsb
   if((CFSRValue & (1 << 9)) != 0) {
		 //CDC_Transmit_FS((uint8_t *)"Divide by zero\n\r", 16);
      printErrorMsg("Divide by zero\r\n");
   }
}

enum { r0, r1, r2, r3, r12, lr, pc, psr};

void stackDump(uint32_t stack[])
{
   static char msg[80];
   //sprintf(msg, "r0  = 0x%08x\r\n", stack[r0]);  printErrorMsg(msg);
   //sprintf(msg, "r1  = 0x%08x\r\n", stack[r1]);  printErrorMsg(msg);
   //sprintf(msg, "r2  = 0x%08x\r\n", stack[r2]);  printErrorMsg(msg);
   //sprintf(msg, "r3  = 0x%08x\r\n", stack[r3]);  printErrorMsg(msg);
   //sprintf(msg, "r12 = 0x%08x\r\n", stack[r12]); printErrorMsg(msg);
   sprintf(msg, "lr  = 0x%08x\r\n", stack[lr]);  printErrorMsg(msg);
   //sprintf(msg, "pc  = 0x%08x\r\n", stack[pc]);  printErrorMsg(msg);
   //sprintf(msg, "psr = 0x%08x\r\n", stack[psr]); printErrorMsg(msg);
	 //sprintf(msg, "sp = 0x%08x\r\n", (uint32_t)(void*)&stack[r0]); printErrorMsg(msg);
}


/**
* @brief This function handles Hard fault interrupt.
*/
void Hard_Fault_Handler(uint32_t stack[])
{
  /* USER CODE BEGIN HardFault_IRQn 0 */
	static char msg[80];
   //printErrorMsg("In Hard Fault Handler\r\n");
   //sprintf(msg, "SCB->HFSR = 0x%08x\r\n", SCB->HFSR);
   //printErrorMsg(msg);
   if ((SCB->HFSR & (1 << 30)) != 0) {
       //printErrorMsg("Forced Hard Fault\r\n");
       //sprintf(msg, "SCB->CFSR = 0x%08x\r\n", SCB->CFSR );
       //printErrorMsg(msg);
       if((SCB->CFSR & 0xFFFF0000) != 0) {
         //printUsageErrorMsg(SCB->CFSR);
      }
   }
	 stackDump(stack);
	 test1_on;
   __ASM volatile("BKPT #01");
  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
		
  }
  /* USER CODE BEGIN HardFault_IRQn 1 */

  /* USER CODE END HardFault_IRQn 1 */
}

__asm void HardFault_Handler(void) 
{
  MRS r0, MSP
  B __cpp(Hard_Fault_Handler) 

}




/**
* @brief This function handles Memory management fault.
*/
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */
	test2_on;
  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
  }
  /* USER CODE BEGIN MemoryManagement_IRQn 1 */

  /* USER CODE END MemoryManagement_IRQn 1 */
}

/**
* @brief This function handles Prefetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
		test3_on;
  }
  /* USER CODE BEGIN BusFault_IRQn 1 */

  /* USER CODE END BusFault_IRQn 1 */
}

/**
* @brief This function handles Undefined instruction or illegal state.
*/
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
		test3_on;
  }
  /* USER CODE BEGIN UsageFault_IRQn 1 */

  /* USER CODE END UsageFault_IRQn 1 */
}

/**
* @brief This function handles System service call via SWI instruction.
*/
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
* @brief This function handles Debug monitor.
*/
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
* @brief This function handles Pendable request for system service.
*/
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
	serialComErrorTimeout++;
	TimerTick++;//spremenljivka, ki je potrebna za delovanje timerjev na OS
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F3xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f3xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles SDADC1 global interrupt.
*/
void SDADC1_IRQHandler(void)
{
  /* USER CODE BEGIN SDADC1_IRQn 0 */
  /* USER CODE END SDADC1_IRQn 0 */
  HAL_SDADC_IRQHandler(&hsdadc1);
	//HAL_SDADC_InjectedStart_IT(&hsdadc3);
  /* USER CODE BEGIN SDADC1_IRQn 1 */
	SDADC1_Handler();
  /* USER CODE END SDADC1_IRQn 1 */
	
}

/**
* @brief This function handles SDADC2 global interrupt.
*/
void SDADC2_IRQHandler(void)
{
  /* USER CODE BEGIN SDADC2_IRQn 0 */
  /* USER CODE END SDADC2_IRQn 0 */
	SDADC2_Handler();
  HAL_SDADC_IRQHandler(&hsdadc2);
  /* USER CODE BEGIN SDADC2_IRQn 1 */
  /* USER CODE END SDADC2_IRQn 1 */
}

/**
* @brief This function handles SDADC3 global interrupt.
*/
void SDADC3_IRQHandler(void)
{
  /* USER CODE BEGIN SDADC3_IRQn 0 */
  /* USER CODE END SDADC3_IRQn 0 */
  HAL_SDADC_IRQHandler(&hsdadc3);
  /* USER CODE BEGIN SDADC3_IRQn 1 */
	SDADC3_Handler();
  /* USER CODE END SDADC3_IRQn 1 */
}

/**
* @brief This function handles USB low priority global interrupt.
*/
void USB_LP_IRQHandler(void)
{

//	uint16_t rec_num=0;
	//USBD_CDC_ReceivePacket(&hUsbDeviceFS);
  /* USER CODE BEGIN USB_LP_IRQn 0 */
	//rec_num=USBD_GetRxCount (&hUsbDeviceFS , CDC_IN_EP);
	
  /* USER CODE END USB_LP_IRQn 0 */
  
	    /* USER CODE BEGIN 3 */
	HAL_PCD_IRQHandler(&hpcd_USB_FS);
  /* USER CODE BEGIN USB_LP_IRQn 1 */

  /* USER CODE END USB_LP_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
void TIM3_IRQHandler(void)
{	
	TIMER3_IRQHandler();
}
//timer namenjen merjenju casa
//	htim7.Instance -> CNT; - prednalozi count register
//	htim7.Instance -> ARR; - prednalozi preload register 1 cela stevilka pomeni 100us
void TIM7_IRQHandler(void)
{	
//	global = htim7.Instance -> CNT;
//	global2 = htim7.Instance -> ARR;
	HAL_TIM_IRQHandler(&htim7);
	HAL_TIM_Base_Stop_IT(&htim7);//tole more bit obvezno za hendlerjem
	TIMER7_IRQHandler();	
	//HAL_TIM_Base_Stop_IT(&htim7);//tole more bit obvezno za hendlerjem
}

void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */

  /* USER CODE END EXTI15_10_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */
	USBConnected_Handler();
  /* USER CODE END EXTI15_10_IRQn 1 */
}
//interrupt na ULN1 zero cross
void EXTI9_5_IRQHandler(void)
{
	static float perm_temp1=0;
	static float perm_temp2=0;
	static float perm_temp3=0;
	static uint32_t count=0;
	static uint32_t count2=0;
	if(((meas_control & __SDADC2_START_MASK) || (meas_control & __SDADC1_START_MASK) || (meas_control & __SDADC3_START_MASK))&&(!(meas_control & __START_TIMER_ON)))
	{
			//zakasnitev z timerjem 7
			//prvi ponavadi fali, zato pocakamo na drugi interrupt
			if(count2>=1)
			{
				if(THD_COMPUTATION_METHOD==FFT) {htim7.Instance -> ARR=SDADC_ON_DELAY;}
				else if(!(meas_control & __THD_MEASURING))//pazi tale ni logicna sm more tko bit..po logiki je lih obratn
				{
					if(il_thd_channel==IL1_CHANNEL)htim7.Instance -> ARR=SDADC_ON_IL1THD_DELAY;
					else if(il_thd_channel==IL2_CHANNEL)htim7.Instance -> ARR=SDADC_ON_IL2THD_DELAY;
					else if(il_thd_channel==IL3_CHANNEL)htim7.Instance -> ARR=SDADC_ON_IL3THD_DELAY;	
				}
				else {htim7.Instance -> ARR=SDADC_ON_DELAY;}
				htim7.Instance ->SR =0;
				htim7.Instance -> CNT=0;
				HAL_TIM_Base_Start_IT(&htim7);
				stevec3 = htim7.Instance -> CNT;
				meas_control |= __START_TIMER_ON;

				count2=0;
			}
			else count2++;
	}
	if((compute_control & __IL1_THD_START)&&(!(meas_control & __START_TIMER_ON)))
	{
		if(count2>=1)
		{
			htim7.Instance ->SR =0;
			htim7.Instance -> ARR=SDADC_ON_IL1THD_DELAY;
			htim7.Instance -> CNT=0;
			HAL_TIM_Base_Start_IT(&htim7);
			meas_control |= __START_TIMER_ON;
			count2=0;
		}
		else count2++;
	}
	else if((compute_control & __ULN1_THD_START)&&(!(meas_control & __START_TIMER_ON)))
	{
		if(count2>=1)
		{
			htim7.Instance ->SR =0;
			htim7.Instance -> ARR=SDADC_ON_IL1THD_DELAY;
			htim7.Instance -> CNT=0;
			HAL_TIM_Base_Start_IT(&htim7);
			meas_control |= __START_TIMER_ON;
			count2=0;
		}
		else count2++;
	}
	else if((compute_control & __ULN2_THD_START)&&(!(meas_control & __START_TIMER_ON)))
	{
		if(count2>=1)
		{
			htim7.Instance ->SR =0;
			htim7.Instance -> ARR=SDADC_ON_IL2THD_DELAY;
			htim7.Instance -> CNT=0;
			HAL_TIM_Base_Start_IT(&htim7);
			meas_control |= __START_TIMER_ON;
			count2=0;
		}
		else count2++;
	}
	else if((compute_control & __IL2_THD_START)&&(!(meas_control & __START_TIMER_ON)))
	{
		if(count2>=1)
		{
			htim7.Instance ->SR =0;
			htim7.Instance -> ARR=SDADC_ON_IL2THD_DELAY;
			htim7.Instance -> CNT=0;
			HAL_TIM_Base_Start_IT(&htim7);
			meas_control |= __START_TIMER_ON;
			count2=0;
		}
		else count2++;
	}
	else if((compute_control & __IL3_THD_START)&&(!(meas_control & __START_TIMER_ON)))
	{
		if(count2>=1)
		{
			htim7.Instance ->SR =0;
			htim7.Instance -> ARR=SDADC_ON_IL3THD_DELAY;
			htim7.Instance -> CNT=0;
			HAL_TIM_Base_Start_IT(&htim7);
			meas_control |= __START_TIMER_ON;
			count2=0;
		}
		else count2++;
	}
	else if((compute_control & __ULN3_THD_START)&&(!(meas_control & __START_TIMER_ON)))
	{
		if(count2>=1)
		{
			htim7.Instance ->SR =0;
			htim7.Instance -> ARR=SDADC_ON_IL3THD_DELAY;
			htim7.Instance -> CNT=0;
			HAL_TIM_Base_Start_IT(&htim7);
			meas_control |= __START_TIMER_ON;
			count2=0;
		}
		else count2++;
	}
	else if((mach_task_control & __MACH_URES_DISCONNECT_PS)&&(!(meas_control & __START_TIMER_ON)))
	{
		if(count2>=URES_PERIODS_TO_WAIT)
		{
			htim7.Instance ->SR =0;
			if((current_URES_measurement==__L1_PE)||(current_URES_measurement==__L1_N))
			{
				//pogledamo ce je enofazni sistem z L-jem na N vodniku - ce je dodamo pol periode k zakasnitvi
				if((global_control & __1P_CONNECTION)&&(global_control & __L_ON_N))
				{
					htim7.Instance -> ARR = URES_TIME_TO_DIS_L1-URES_TIME_L_ON_N_ADD;
				}
				else
				{
					htim7.Instance -> ARR = URES_TIME_TO_DIS_L1;
				}
			}
			else if((current_URES_measurement==__L2_PE)||(current_URES_measurement==__L2_N))
			{
				htim7.Instance -> ARR=URES_TIME_TO_DIS_L2;
			}
			else if((current_URES_measurement==__L3_PE)||(current_URES_measurement==__L3_N))
			{
				htim7.Instance -> ARR=URES_TIME_TO_DIS_L3;
			}
			else if(current_URES_measurement==__L1_L2)
			{
				htim7.Instance -> ARR=URES_TIME_TO_DIS_L1_L2;
			}
			else if(current_URES_measurement==__L1_L3)
			{
				htim7.Instance -> ARR=URES_TIME_TO_DIS_L1_L3;
			}
			else if(current_URES_measurement==__L2_L3)
			{
				htim7.Instance -> ARR=URES_TIME_TO_DIS_L2_L3;
			}
			else if(current_URES_measurement==__TIMER_INIT)
				htim7.Instance -> ARR=50;//uporablja se za inicializacijo
			htim7.Instance -> CNT=0;
			HAL_TIM_Base_Start_IT(&htim7);
			meas_control |= __START_TIMER_ON;
			count2=0;
		}
		else count2++;
	}
	else if((global_control & __TURN_ON_CONTACTOR)&&(!(meas_control & __START_TIMER_ON)))
	{
		htim7.Instance ->SR =0;
		if(synchro_interrupt_control & __SET_L1_CONTACTOR)
			htim7.Instance -> ARR=L1_CONTACTOR_TIME_TO_ON;//uporablja se za inicializacijo
		else if(synchro_interrupt_control & __SET_L2_CONTACTOR)
			htim7.Instance -> ARR=L2_CONTACTOR_TIME_TO_ON;//uporablja se za inicializacijo
		else if(synchro_interrupt_control & __SET_L3_CONTACTOR)
			htim7.Instance -> ARR=L3_CONTACTOR_TIME_TO_ON;//uporablja se za inicializacijo
		else if(synchro_interrupt_control & __SET_N_CONTACTOR)
			htim7.Instance -> ARR=50;//uporablja se za inicializacijo
		else if(synchro_interrupt_control & __SET_PE_CONTACTOR)
			htim7.Instance -> ARR=50;//uporablja se za inicializacijo
		htim7.Instance -> CNT=0;
		HAL_TIM_Base_Start_IT(&htim7);
		meas_control |= __START_TIMER_ON;
	}		
	if((global_control & __ON_TEST_IN_PROG)&&(meas_control & __MEAS_IN_PROGRESS)&&(!(global_control & __INPUT_PHASES_DET)))
	{
		float temp1;
		float temp2;
		float temp3;
		float temp4;
		static uint32_t count3=0;
		temp1=get_inst_value(__ULN1);
		temp2=get_inst_value(__ULN2);
		temp3=get_inst_value(__ULN3);
		perm_temp1+=temp1;
		perm_temp2+=temp2;
		perm_temp3+=temp3;
		if(count3>=1)
		{
			if(count>=4)
			{
				global_control &= (~(__VOLTAGES_IN_PHASE));
				perm_temp1=perm_temp1/((float)count+1);
				perm_temp2=perm_temp2/((float)count+1);
				perm_temp3=perm_temp3/((float)count+1);
				//pogledamo ce so napetosti v trenutku ko faza ena precka 0 razlicne od napetosti faze 1
				if(((perm_temp2>(perm_temp1+PHASE_DETECT_LOW_LIMIT))||(perm_temp2<(perm_temp1-PHASE_DETECT_LOW_LIMIT)))&&((perm_temp3>(perm_temp1+PHASE_DETECT_LOW_LIMIT))||(perm_temp3<(perm_temp1-PHASE_DETECT_LOW_LIMIT))))
				global_control |= __VOLTAGES_IN_PHASE;	
				//to pomeni, da ce je napetost UL1PE blizu nicle in UNPE visoka, da gre za enofazni sistem, ce pa je obratno pa pomeni da je na vseh treh fazah ista faza
				global_control |= __INPUT_PHASES_DET;
			}
			else
			{
				count++;
			}
			count3=0;
		}
		else count3++;
	}
		
	HAL_GPIO_EXTI_IRQHandler(L1_SINHRO_Pin);
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
