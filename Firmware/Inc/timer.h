  //******************************************************************************
  //* File Name          : timer.h
  //* Description        : Timer initialization
  //* Created			 : Jan Koprivec
  //* Date				 : 13.7.2017
  //******************************************************************************
  
#ifndef __TIMER_H
#define __TIMER_H

#include <stdint.h>

void MX_TIM3_Init(void);
void TIMER3_IRQHandler(void);
void TIMER7_IRQHandler(void);
void MX_TIM6_Init(void);
void MX_TIM7_Init(void);
#endif


