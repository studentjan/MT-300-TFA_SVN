  //******************************************************************************
  //* File Name          : timer.c
  //* Description        : Timer initialization
  //* Created			 : Jan Koprivec
  //* Date				 : 13.7.2017
  //******************************************************************************
  
 #include "timer.h"
 #include "stm32f3xx_hal.h"
 #include "defines.h"
 #include "machines.h"
 #include "do_task.h"
 #include "rel_driver.h"
 //#include "includes.h"

//pozor, inicializacija za TIMER 2 je v serial_com.c
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim7;
TIM_HandleTypeDef htim6;
uint32_t stevec4;
extern uint32_t meas_control;
extern uint32_t compute_control;
extern uint32_t compute_control2;
extern SDADC_HandleTypeDef hsdadc2;
extern SDADC_HandleTypeDef hsdadc1;
extern SDADC_HandleTypeDef hsdadc3;
extern uint32_t mach_task_control;
extern uint32_t current_URES_measurement;
extern uint32_t global_control;
extern uint32_t synchro_interrupt_control;

/* TIM3 init function */
void MX_TIM3_Init(void)
{

	TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
	TIM_IC_InitTypeDef sConfigIC;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 7199;
  htim3.Init.CounterMode = TIM_COUNTERMODE_DOWN;
  htim3.Init.Period = 186;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	  if (HAL_TIM_IC_Init(&htim3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	if (HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	
//  TIM_ClockConfigTypeDef sClockSourceConfig;
//  TIM_MasterConfigTypeDef sMasterConfig;
//  TIM_IC_InitTypeDef sConfigIC;

//  htim3.Instance = TIM3;
//  htim3.Init.Prescaler = 0;
//  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
//  htim3.Init.Period = 0;
//  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
//  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }

//  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
//  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }

//  if (HAL_TIM_IC_Init(&htim3) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }

//  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
//  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
//  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }

//  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
//  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
//  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
//  sConfigIC.ICFilter = 0;
//  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }

}
//timer za delay counter prozi interrupt
//koliko cez koliko casa bo sprozil interrupt po klicu starta je odvisno od ARR registra
//nastavitev prozi interupt cez n*100us!!!!!!!!!
void MX_TIM7_Init(void)
{

  TIM_MasterConfigTypeDef sMasterConfig;

  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 7199;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 187;
  htim7.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

//  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
//  if (HAL_TIM_ConfigClockSource(&htim7, &sClockSourceConfig) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }

//  if (HAL_TIM_IC_Init(&htim6) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	HAL_TIM_Base_Start_IT(&htim7);//za zacetek enkrat vklopimo timer da se vse nastavi

//  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
//  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
//  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
//  sConfigIC.ICFilter = 0;
//  if (HAL_TIM_IC_ConfigChannel(&htim6, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }

}
//timer za stetje brez interrupta steje na 10 us  en count
void MX_TIM6_Init(void)
{
	TIM_MasterConfigTypeDef sMasterConfig;
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 719;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 0xFFFF;
  //htim6.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

//ce je vklopljen se timer 7 interrupt prozi na 10 us
void TIMER7_IRQHandler(void)
{
	if(meas_control & __START_TIMER_ON)
	{
		if(((meas_control & __SDADC2_START_MASK) || (meas_control & __SDADC1_START_MASK) || (meas_control & __SDADC3_START_MASK))&&((!(meas_control & __THD_MEASURING))||(THD_COMPUTATION_METHOD==FFT)))
		{
			stevec4 = htim7.Instance -> CNT;
			if(meas_control & __SDADC2_START_MASK)
			{
				HAL_SDADC_InjectedStart(&hsdadc2);
				meas_control &= (~__SDADC2_START_MASK);
			}
			if(meas_control & __SDADC1_START_MASK)
			{
				HAL_SDADC_InjectedStart_IT(&hsdadc1);
				meas_control &= (~__SDADC1_START_MASK);
			}
			if(meas_control & __SDADC3_START_MASK)
			{
				HAL_SDADC_InjectedStart_IT(&hsdadc3);
				meas_control &= (~__SDADC3_START_MASK);
			}
			meas_control &= (~__START_TIMER_ON);
		}
		else if(((meas_control & __SDADC2_START_MASK) || (meas_control & __SDADC1_START_MASK))&&(meas_control & __THD_MEASURING))
		{
			if(meas_control & __SDADC1_START_MASK)
			{
				HAL_SDADC_Start_IT(&hsdadc1);
				meas_control &= (~__SDADC1_START_MASK);
			}
			if(meas_control & __SDADC2_START_MASK)
			{
				HAL_SDADC_Start_IT(&hsdadc2);
				meas_control &= (~__SDADC2_START_MASK);
			}
			meas_control &= (~__START_TIMER_ON);
		}
		else if(mach_task_control &__MACH_URES_DISCONNECT_PS)
		{
			disconnectURESContactors();
			if(current_URES_measurement == __TIMER_INIT)
				mach_task_control |= __MACH_TIMER_INIT;
			mach_task_control &= ~__MACH_URES_DISCONNECT_PS;
			meas_control &= (~__START_TIMER_ON);
		}
		//ko pridemo v naslednjo funkcijo najprej postavimo prvi kontaktor v vrsti L1, L2, L3
		//ce je zahteva po vkolpu ostalih relejev vklopimo se te tako, da odstejemo cas, ki je bil potreben za vklop prvega od casa, ki je potreben za vklop naslednjega
		else if(global_control & __TURN_ON_CONTACTOR)
		{
			if(synchro_interrupt_control & __SET_L1_CONTACTOR)
			{
				SET_L1_CONTACTOR;
				synchro_interrupt_control &= ~__SET_L1_CONTACTOR;
				if(synchro_interrupt_control & __SET_L2_CONTACTOR)
				{
					htim7.Instance ->SR =0;
					htim7.Instance -> ARR=L2_CONTACTOR_TIME_TO_ON-L1_CONTACTOR_TIME_TO_ON;
					htim7.Instance -> CNT=0;
					HAL_TIM_Base_Start_IT(&htim7);
					
				}
				else if(synchro_interrupt_control & __SET_L3_CONTACTOR)
				{
					htim7.Instance ->SR =0;
					htim7.Instance -> ARR=L3_CONTACTOR_TIME_TO_ON-L1_CONTACTOR_TIME_TO_ON;
					htim7.Instance -> CNT=0;
					HAL_TIM_Base_Start_IT(&htim7);
				}
			}
			else if(synchro_interrupt_control & __SET_L2_CONTACTOR)
			{
				SET_L2_CONTACTOR;
				synchro_interrupt_control &= ~__SET_L2_CONTACTOR;
				if(synchro_interrupt_control & __SET_L3_CONTACTOR)
				{
					htim7.Instance ->SR =0;
					htim7.Instance -> ARR=L3_CONTACTOR_TIME_TO_ON-L2_CONTACTOR_TIME_TO_ON;
					htim7.Instance -> CNT=0;
					HAL_TIM_Base_Start_IT(&htim7);
				}
			}
			else if(synchro_interrupt_control & __SET_L3_CONTACTOR)
			{
				SET_L3_CONTACTOR;
				synchro_interrupt_control &= ~__SET_L3_CONTACTOR;
			}
			if(!(synchro_interrupt_control & (__SET_L1_CONTACTOR|__SET_L2_CONTACTOR|__SET_L3_CONTACTOR)))
			{
				meas_control &= ~__START_TIMER_ON;
				global_control &= ~__TURN_ON_CONTACTOR;
				disable_sinchro_interrupt(__CONTACTOR_SYNCHRO_ON);
			}
		}
		if(compute_control & __ULN1_THD_START) 
		{
			compute_control |= __ULN1_THD_SAMPLING; 
			meas_control &= (~__START_TIMER_ON);
			compute_control &= (~__ULN1_THD_START);
		}
		else if(compute_control & __ULN2_THD_START) 
		{
			compute_control |= __ULN2_THD_SAMPLING;
			meas_control &= (~__START_TIMER_ON);
			compute_control &= (~__ULN2_THD_START);
		}
		else if(compute_control & __ULN3_THD_START) 
		{
			compute_control |= __ULN3_THD_SAMPLING;
			meas_control &= (~__START_TIMER_ON);
			compute_control &= (~__ULN3_THD_START);
		}
		else if(compute_control & __IL1_THD_START) 
		{
			compute_control |= __IL1_THD_SAMPLING;
			meas_control &= (~__START_TIMER_ON);
			compute_control &= (~__IL1_THD_START);
		}
		else if(compute_control & __IL2_THD_START) 
		{
			compute_control |= __IL2_THD_SAMPLING;
			meas_control &= (~__START_TIMER_ON);
			compute_control &= (~__IL2_THD_START);
		}
		else if(compute_control & __IL3_THD_START) 
		{
			compute_control |= __IL3_THD_SAMPLING;
			meas_control &= (~__START_TIMER_ON);
			compute_control &= (~__IL3_THD_START);
		}
	}
}

void TIMER3_IRQHandler(void)
{
	
	HAL_TIM_IRQHandler(&htim3);
}
