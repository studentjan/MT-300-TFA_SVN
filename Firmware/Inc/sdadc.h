  //******************************************************************************
  //* File Name          : sdadc.h
  //* Description        : SDADC initialization
  //* Created			 : Jan Koprivec
  //* Date				 : 13.7.2017
  //******************************************************************************
#ifndef __SDADC_H
#define __SDADC_H
#include <stdint.h>
#include <arm_math.h>
//#define SDADC1_AIN6P_ULN3_Pin GPIO_PIN_0
//#define SDADC1_AIN6P_ULN3_GPIO_Port GPIOB
//#define SDADC1_AIN6M_ULN3_Pin GPIO_PIN_1
//#define SDADC1_AIN6M_ULN3_GPIO_Port GPIOB
//#define SDADC1_AIN4P_ULN2_Pin GPIO_PIN_2
//#define SDADC1_AIN4P_ULN2_GPIO_Port GPIOB
//#define SDADC1_AIN4M_ULN2_Pin GPIO_PIN_7
//#define SDADC1_AIN4M_ULN2_GPIO_Port GPIOE
//#define SDADC1_AIN8P_ULN1_Pin GPIO_PIN_8
//#define SDADC1_AIN8P_ULN1_GPIO_Port GPIOE
//#define SDADC1_AIN8M_ULN1_Pin GPIO_PIN_9
//#define SDADC1_AIN8M_ULN1_GPIO_Port GPIOE
//#define SDADC2_AIN4P_IL3_Pin GPIO_PIN_11
//#define SDADC2_AIN4P_IL3_GPIO_Port GPIOE
//#define SDADC2_AIN4M_IL3_Pin GPIO_PIN_12
//#define SDADC2_AIN4M_IL3_GPIO_Port GPIOE
//#define SDADC2_AIN2P_IL2_Pin GPIO_PIN_13
//#define SDADC2_AIN2P_IL2_GPIO_Port GPIOE
//#define SDADC2_AIN2M_IL2_Pin GPIO_PIN_14
//#define SDADC2_AIN2M_IL2_GPIO_Port GPIOE
//#define SDADC2_AIN0P_IL1_Pin GPIO_PIN_15
//#define SDADC2_AIN0P_IL1_GPIO_Port GPIOE
//#define SDADC2_AIN0M_IL1_Pin GPIO_PIN_10
//#define SDADC2_AIN0M_IL1_GPIO_Port GPIOB
void MX_SDADC1_Init(void);
void MX_SDADC2_Init(void); 
void MX_SDADC3_Init(void); 
void SDADC3_Handler(void);
void SDADC1_Handler(void);
void SDADC2_Handler(void);
void SDADC3_Handler(void);
float get_value(uint32_t param);
void IL_setGain(uint32_t channel, uint8_t gain);
void input_START_measure(void);
void input_STOP_measure(void);
void input_START_THD_measure(void);
void input_STOP_THD_measure(void);
void SDADC2_set_THD(void);
void SDADC2_set_normal(void);
void SDADC1_set_THD(void);
void SDADC1_set_normal(void);
void compute_rms(void);
void set_SDADC_zero(void);
void write_THD(float32_t value, uint32_t place);
float get_inst_value(uint32_t param);
void setConstant(char* constant, char* value);
void setInitConstants(void);


#define COMPUTE_TRMS		_ON


#endif
