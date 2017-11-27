  //******************************************************************************
  //* File Name          : sdadc.c
  //* Description        : SDADC initialization
  //* Created			 : Jan Koprivec
  //* Date				 : 13.7.2017
  //******************************************************************************
 #include "sdadc.h"
 #include "arm_math.h"
 #include "stm32f3xx_hal.h"
 #include "do_task.h"
 #include "defines.h"
 #include "test.h"
 #include "comunication.h"
 #include "stm32f3xx_hal_conf.h"

//---------------------------GLOBAL VARIABLES------------------------------------
struct MEAS_struct{
	float correction;
	float effective;
	float avarage_sq;
	float THD_value;
};
struct POWER_struct{
	float real_power_sum;
	float real_power;
	float apparent_power;
	float PF;
};
struct SDADC_struct{
	float32_t sempl;
	float32_t sum;
	uint32_t sample_count;
	int16_t max;
	uint16_t peak_at;
	float32_t max_float;
};

uint16_t stevec2;
SDADC_HandleTypeDef hsdadc1;
SDADC_HandleTypeDef hsdadc2;
SDADC_HandleTypeDef hsdadc3;
struct SDADC_struct SDADC1_CH1s;
struct SDADC_struct SDADC1_CH2s;
struct SDADC_struct SDADC1_CH3s;
struct SDADC_struct SDADC2_CH1s;
struct SDADC_struct SDADC2_CH2s;
struct SDADC_struct SDADC2_CH3s;
struct SDADC_struct SDADC3_CH1s;
struct SDADC_struct SDADC3_CH2s;
struct SDADC_struct SDADC3_CH3s;
struct SDADC_struct SDADC3_CH4s;
struct MEAS_struct IDIFF;
struct MEAS_struct UL1PE;
struct MEAS_struct UNPE;
struct MEAS_struct IL1;
struct MEAS_struct IL2;
struct MEAS_struct IL3;
struct MEAS_struct ULN1;
struct MEAS_struct ULN2;
struct MEAS_struct ULN3;
struct POWER_struct PHASE1;
struct POWER_struct PHASE2;
struct POWER_struct PHASE3;
enum IL_GAIN IL1_GAIN;
enum IL_GAIN IL2_GAIN;
enum IL_GAIN IL3_GAIN;

extern TIM_HandleTypeDef htim6;
extern uint32_t meas_control;
extern uint32_t global_control;
uint32_t compute_control = 0;
uint32_t compute_control2 = 0;
uint32_t interrupt_control = 0;
uint32_t il_thd_channel;
uint32_t uln_thd_channel;
uint32_t last_THD_MEASURED;
uint32_t last_IL_MEASURED;
uint32_t SDADC1_ch_cnt=0;
uint32_t SDADC2_ch_cnt=0;
int16_t krneki[2250];
//float32_t krneki2[2250];
//float32_t krneki_THD[1020];
int16_t reskrneki;
int16_t reskrneki2;
int16_t reskrneki3;
float32_t reskrneki4;
float32_t reskrneki5;
float32_t reskrneki6;
int16_t reskrneki7;
int16_t reskrneki8;
int16_t reskrneki9;
uint32_t channel_global;

uint32_t 	current_THD_sampling;
//bufferja za hranjenje vrednosti za izracun korelacije oziroma fft-ja
//ce je THD_COMPUTATION_METHOD 1 potem racunamo s korelacijo
#if THD_COMPUTATION_METHOD
	float32_t THD_buffer1[CORELATION_THD_BUFFER_SIZE];
#else
	float32_t THD_buffer1[FFT_THD_BUFFER_SIZE*2];
#endif
//-------------------------------------------------------------------------------------

//------------------------------LOCAL FUNCTIONS----------------------------------------
static void measure_IL_Current(int16_t ConvertionResult, uint32_t channel);
static void measure_ULN_Voltage(int16_t ConvertionResult, uint32_t channel);
static void IL_auto_gain(uint32_t channel);
static void measure_IDIFF(int16_t ConvertionResult);
static void measure_UL1PE(int16_t ConvertionResult);
static void measure_UNPE(int16_t ConvertionResult);
static void SDADC2_THD_channel(void);
static void SDADC1_THD_channel(void);
//----------------------------------------------------------------------------------

//----------------------------------FILTERS-----------------------------------------
//+++++++++++++++++++++++++++++HIGHPASS FILTRI++++++++++++++++++++++++++++++++++++++
//IIR Type (Biquad): Butterworth
//Filter Arithmetic: Floating Point (Single Precision)
//Response: Highpass
//Sampling Frequency: 5.555KHz
//Lower Cut-off Frequency: 0.005KHz
//Filter Order: 2
static float32_t IIR_HPF2_coeffs[5] = {0.996028066, -1.99205613,0.996028066,1.992002,-0.992033839};	//PAZI! a koeficiente monzi z -1
//---------------------------------ULN------------------------------------------
//Fs=16,666 kHz, Fc=5 Hz, butterworth

static float32_t ULN1_IIR_state[1*4];	
static const arm_biquad_casd_df1_inst_f32 INST_ULN1_HPF = {1, ULN1_IIR_state, IIR_HPF2_coeffs};
static float32_t ULN2_IIR_state[1*4];	
static const arm_biquad_casd_df1_inst_f32 INST_ULN2_HPF = {1, ULN2_IIR_state, IIR_HPF2_coeffs};
static float32_t ULN3_IIR_state[1*4];	
static const arm_biquad_casd_df1_inst_f32 INST_ULN3_HPF = {1, ULN3_IIR_state, IIR_HPF2_coeffs};
//----------------------------------IL------------------------------------------
//Fs=16,666 kHz, Fc=5 Hz, butterworth
static float32_t IL1_IIR_state[1*4];	
static const arm_biquad_casd_df1_inst_f32 INST_IL1_HPF = {1, IL1_IIR_state, IIR_HPF2_coeffs};
static float32_t IL2_IIR_state[1*4];	
static const arm_biquad_casd_df1_inst_f32 INST_IL2_HPF = {1, IL2_IIR_state, IIR_HPF2_coeffs};
static float32_t IL3_IIR_state[1*4];	
static const arm_biquad_casd_df1_inst_f32 INST_IL3_HPF = {1, IL3_IIR_state, IIR_HPF2_coeffs};
//---------------------------------IDIFF------------------------------------------
static float32_t IDIFF_IIR_state[1*4];	
static const arm_biquad_casd_df1_inst_f32 INST_IDIFF_HPF = {1, IDIFF_IIR_state, IIR_HPF2_coeffs};
//---------------------------------UL1PE------------------------------------------
static float32_t UL1PE_IIR_state[1*4];	
static const arm_biquad_casd_df1_inst_f32 INST_UL1PE_HPF = {1, UL1PE_IIR_state, IIR_HPF2_coeffs};
//---------------------------------UNPE------------------------------------------
static float32_t UNPE_IIR_state[1*4];	
static const arm_biquad_casd_df1_inst_f32 INST_UNPE_HPF = {1, UNPE_IIR_state, IIR_HPF2_coeffs};
//+++++++++++++++++++++++++++++++++++++LOWPASS++++++++++++++++++++++++++++++++++++++++
//IIR Type (Biquad): Butterworth
//Filter Arithmetic: Floating Point (Single Precision)
//Response: Lowpass
//Sampling Frequency: 5.555KHz
//Lower Cut-off Frequency: 0.3KHz
//Filter Order: 2
static float32_t IIR_LPF2_coeffs[5] = { 0.0230786223,  0.0461572446, 0.0230786223,1.52664924,-0.618962};
//----------------------------------ULN----------------------------------------
static float32_t ULN1_IIR_LOW_state[4];	
static const arm_biquad_casd_df1_inst_f32 INST_ULN1_LPF = {1, ULN1_IIR_LOW_state, IIR_LPF2_coeffs};
static float32_t ULN2_IIR_LOW_state[4];	
static const arm_biquad_casd_df1_inst_f32 INST_ULN2_LPF = {1, ULN2_IIR_LOW_state, IIR_LPF2_coeffs};
static float32_t ULN3_IIR_LOW_state[4];	
static const arm_biquad_casd_df1_inst_f32 INST_ULN3_LPF = {1, ULN3_IIR_LOW_state, IIR_LPF2_coeffs};
//-----------------------------------IL----------------------------------------
static float32_t IL1_IIR_LOW_state[4];	
static const arm_biquad_casd_df1_inst_f32 INST_IL1_LPF = {1, IL1_IIR_LOW_state, IIR_LPF2_coeffs};
static float32_t IL2_IIR_LOW_state[4];	
static const arm_biquad_casd_df1_inst_f32 INST_IL2_LPF = {1, IL2_IIR_LOW_state, IIR_LPF2_coeffs};
static float32_t IL3_IIR_LOW_state[4];	
static const arm_biquad_casd_df1_inst_f32 INST_IL3_LPF = {1, IL3_IIR_LOW_state, IIR_LPF2_coeffs};
//---------------------------------IDIFF------------------------------------------
static float32_t IDIFF_IIR_LOW_state[4];	
static const arm_biquad_casd_df1_inst_f32 INST_IDIFF_LPF = {1, IDIFF_IIR_LOW_state, IIR_LPF2_coeffs};
//---------------------------------UL1PE------------------------------------------
static float32_t UL1PE_IIR_LOW_state[4];	
static const arm_biquad_casd_df1_inst_f32 INST_UL1PE_LPF = {1, UL1PE_IIR_LOW_state, IIR_LPF2_coeffs};
//---------------------------------UNPE------------------------------------------
static float32_t UNPE_IIR_LOW_state[4];	
static const arm_biquad_casd_df1_inst_f32 INST_UNPE_LPF = {1, UNPE_IIR_LOW_state, IIR_LPF2_coeffs};
//-----------------------------------------------------------------------------------
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void MX_SDADC1_Init(void)
{

  SDADC_ConfParamTypeDef ConfParamStruct;

    /**Configure the SDADC low power mode, fast conversion mode,
    slow clock mode and SDADC1 reference voltage 
    */
//  hsdadc1.Instance = SDADC1;
//  hsdadc1.Init.IdleLowPowerMode = SDADC_LOWPOWER_NONE;
//  hsdadc1.Init.FastConversionMode = SDADC_FAST_CONV_ENABLE;
//  hsdadc1.Init.SlowClockMode = SDADC_SLOW_CLOCK_DISABLE;
//  hsdadc1.Init.ReferenceVoltage = SDADC_VREF_EXT;
	 hsdadc1.Instance = SDADC1;
  hsdadc1.Init.IdleLowPowerMode = SDADC_LOWPOWER_NONE;
  hsdadc1.Init.FastConversionMode = SDADC_FAST_CONV_DISABLE;
  hsdadc1.Init.SlowClockMode = SDADC_SLOW_CLOCK_DISABLE;
  hsdadc1.Init.ReferenceVoltage = SDADC_VREF_EXT;
  hsdadc1.InjectedTrigger = SDADC_SOFTWARE_TRIGGER;
  if (HAL_SDADC_Init(&hsdadc1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure The Regular Mode 
    */
//  if (HAL_SDADC_SelectRegularTrigger(&hsdadc1, SDADC_SOFTWARE_TRIGGER) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }  
//	
  if (HAL_SDADC_MultiModeConfigChannel(&hsdadc1, SDADC_MULTIMODE_SDADC1_SDADC2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Injected Mode 
    */
  if (HAL_SDADC_SelectInjectedDelay(&hsdadc1, SDADC_INJECTED_DELAY_NONE) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_SDADC_SelectInjectedTrigger(&hsdadc1, SDADC_SOFTWARE_TRIGGER) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	  if (HAL_SDADC_SelectRegularTrigger(&hsdadc1, SDADC_SOFTWARE_TRIGGER) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }  

  if (HAL_SDADC_InjectedMultiModeConfigChannel(&hsdadc1, SDADC_MULTIMODE_SDADC1_SDADC2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_SDADC_InjectedConfigChannel(&hsdadc1, SDADC_CHANNEL_4|SDADC_CHANNEL_6
                              |SDADC_CHANNEL_8, SDADC_CONTINUOUS_CONV_ON) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
	}
    /**Set parameters for SDADC configuration 0 Register 
    */
  ConfParamStruct.InputMode = SDADC_INPUT_MODE_DIFF;
  ConfParamStruct.Gain = SDADC_GAIN_1;
  ConfParamStruct.CommonMode = SDADC_COMMON_MODE_VSSA;
  ConfParamStruct.Offset = 0;
  if (HAL_SDADC_PrepareChannelConfig(&hsdadc1, SDADC_CONF_INDEX_0, &ConfParamStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	if (HAL_SDADC_PrepareChannelConfig(&hsdadc1, SDADC_CONF_INDEX_1, &ConfParamStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
//  if (HAL_SDADC_PrepareChannelConfig(&hsdadc1, SDADC_CONF_INDEX_1, &ConfParamStruct) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }
//  if (HAL_SDADC_PrepareChannelConfig(&hsdadc1, SDADC_CONF_INDEX_2, &ConfParamStruct) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }
	
	
//	//channel ULN1
//	if (HAL_SDADC_AssociateChannelConfig(&hsdadc1, SDADC_CHANNEL_8|SDADC_CHANNEL_4|SDADC_CHANNEL_6, SDADC_CONF_INDEX_0) != HAL_OK)
//	{
//			_Error_Handler(__FILE__, __LINE__);
//	}
//	if (HAL_SDADC_ConfigChannel(&hsdadc1, SDADC_CHANNEL_8, SDADC_CONTINUOUS_CONV_ON) != HAL_OK)
//	{
//			_Error_Handler(__FILE__, __LINE__);
//	}
//	HAL_SDADC_CalibrationStart(&hsdadc1,SDADC_CALIBRATION_SEQ_1);
//	while(HAL_SDADC_PollForCalibEvent(&hsdadc1,CALIBRATION_TIMEOUT)!=HAL_OK);
//	//channel ULN2
//	if (HAL_SDADC_AssociateChannelConfig(&hsdadc1, SDADC_CHANNEL_4, SDADC_CONF_INDEX_1) != HAL_OK)
//	{
//			_Error_Handler(__FILE__, __LINE__);
//	}
////	if (HAL_SDADC_ConfigChannel(&hsdadc1, SDADC_CHANNEL_4, SDADC_CONTINUOUS_CONV_OFF) != HAL_OK)
////	{
////			_Error_Handler(__FILE__, __LINE__);
////	}
//	HAL_SDADC_CalibrationStart(&hsdadc1,SDADC_CALIBRATION_SEQ_2);
//	while(HAL_SDADC_PollForCalibEvent(&hsdadc1,CALIBRATION_TIMEOUT)!=HAL_OK);
//	//channel ULN3
//	if (HAL_SDADC_AssociateChannelConfig(&hsdadc1, SDADC_CHANNEL_6, SDADC_CONF_INDEX_2) != HAL_OK)
//	{
//			_Error_Handler(__FILE__, __LINE__);
//	}
////	if (HAL_SDADC_ConfigChannel(&hsdadc1, SDADC_CHANNEL_6, SDADC_CONTINUOUS_CONV_OFF) != HAL_OK)
////	{
////			_Error_Handler(__FILE__, __LINE__);
////	}
//	HAL_SDADC_CalibrationStart(&hsdadc1,SDADC_CALIBRATION_SEQ_3);
//	while(HAL_SDADC_PollForCalibEvent(&hsdadc1,CALIBRATION_TIMEOUT)!=HAL_OK);
//	
	/**Configure the Injected Channel 
  */
	  if (HAL_SDADC_AssociateChannelConfig(&hsdadc1, SDADC_CHANNEL_4, SDADC_CONF_INDEX_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Injected Channel 
    */
  if (HAL_SDADC_AssociateChannelConfig(&hsdadc1, SDADC_CHANNEL_6, SDADC_CONF_INDEX_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Injected Channel 
    */
  if (HAL_SDADC_AssociateChannelConfig(&hsdadc1, SDADC_CHANNEL_8, SDADC_CONF_INDEX_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	HAL_SDADC_CalibrationStart(&hsdadc1,SDADC_CALIBRATION_SEQ_2);
	while(HAL_SDADC_PollForCalibEvent(&hsdadc1,CALIBRATION_TIMEOUT)!=HAL_OK);

}
void SDADC1_set_THD(void) 
{
	SDADC_ConfParamTypeDef ConfParamStruct;
	
	if (HAL_SDADC_DeInit(&hsdadc1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  } 
	
	
  hsdadc1.Instance = SDADC1;
  hsdadc1.Init.IdleLowPowerMode = SDADC_LOWPOWER_NONE;
  hsdadc1.Init.FastConversionMode = SDADC_FAST_CONV_ENABLE;
  hsdadc1.Init.SlowClockMode = SDADC_SLOW_CLOCK_DISABLE;
  hsdadc1.Init.ReferenceVoltage = SDADC_VREF_EXT;
	hsdadc1.RegularTrigger = SDADC_SOFTWARE_TRIGGER;
  if (HAL_SDADC_Init(&hsdadc1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	
//	/* Disable SDADC */
//	hsdadc1.Instance->CR2 &= ~(SDADC_CR2_ADON);//more bit drgac ne dela
//	
//	hsdadc1.Instance->CR2 &= ~(SDADC_CR2_FAST);
//  hsdadc1.Instance->CR2 |= SDADC_FAST_CONV_ENABLE;
//	/* Enable SDADC */
//  hsdadc1.Instance->CR2 |= SDADC_CR2_ADON;
//	
//	
//	
//  /* Wait end of stabilization */
//  while((hsdadc1.Instance->ISR & SDADC_ISR_STABIP) != 0U)
//  {
//  }
//	hsdadc1.State = HAL_SDADC_STATE_READY;
    /**Configure The Regular Mode 
    */
	
    /**Set parameters for SDADC configuration 0 Register 
    */
//  ConfParamStruct.InputMode = SDADC_INPUT_MODE_DIFF;
//  ConfParamStruct.Gain = SDADC_GAIN_1;
//  ConfParamStruct.CommonMode = SDADC_COMMON_MODE_VSSA;
//  ConfParamStruct.Offset = 0;
//  if (HAL_SDADC_PrepareChannelConfig(&hsdadc1, SDADC_CONF_INDEX_1, &ConfParamStruct) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }

	if (HAL_SDADC_AssociateChannelConfig(&hsdadc1, SDADC_CHANNEL_8, SDADC_CONF_INDEX_1) != HAL_OK)
	{
			_Error_Handler(__FILE__, __LINE__);
	}

	if (HAL_SDADC_AssociateChannelConfig(&hsdadc1, SDADC_CHANNEL_4, SDADC_CONF_INDEX_1) != HAL_OK)
	{
			_Error_Handler(__FILE__, __LINE__);
	}

	if (HAL_SDADC_AssociateChannelConfig(&hsdadc1, SDADC_CHANNEL_6, SDADC_CONF_INDEX_1) != HAL_OK)
	{
			_Error_Handler(__FILE__, __LINE__);
	}
	SDADC1_THD_channel();
}
void SDADC1_set_normal(void)
{

  SDADC_ConfParamTypeDef ConfParamStruct;
	
	if (HAL_SDADC_DeInit(&hsdadc1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	hsdadc1.Instance = SDADC1;
  hsdadc1.Init.IdleLowPowerMode = SDADC_LOWPOWER_NONE;
  hsdadc1.Init.FastConversionMode = SDADC_FAST_CONV_DISABLE;
  hsdadc1.Init.SlowClockMode = SDADC_SLOW_CLOCK_DISABLE;
  hsdadc1.Init.ReferenceVoltage = SDADC_VREF_EXT;
  hsdadc1.InjectedTrigger = SDADC_SOFTWARE_TRIGGER;
  if (HAL_SDADC_Init(&hsdadc1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
//		/* Disable SDADC */
//	hsdadc1.Instance->CR2 &= ~(SDADC_CR2_ADON);
//	
//	hsdadc1.Instance->CR2 &= ~(SDADC_CR2_FAST);
//  hsdadc1.Instance->CR2 |= SDADC_FAST_CONV_DISABLE;
//	/* Enable SDADC */
//  hsdadc1.Instance->CR2 |= SDADC_CR2_ADON;
//	
//	
//	
//  /* Wait end of stabilization */
//  while((hsdadc1.Instance->ISR & SDADC_ISR_STABIP) != 0U)
//  {
//  }
//	hsdadc1.State = HAL_SDADC_STATE_READY;
	
  if (HAL_SDADC_MultiModeConfigChannel(&hsdadc1, SDADC_MULTIMODE_SDADC1_SDADC2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	
  if (HAL_SDADC_SelectInjectedDelay(&hsdadc1, SDADC_INJECTED_DELAY_NONE) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_SDADC_SelectInjectedTrigger(&hsdadc1, SDADC_SOFTWARE_TRIGGER) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_SDADC_InjectedMultiModeConfigChannel(&hsdadc1, SDADC_MULTIMODE_SDADC1_SDADC2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	//tle sm mel na zacetk 4 6 8
  if (HAL_SDADC_InjectedConfigChannel(&hsdadc1, SDADC_CHANNEL_8|SDADC_CHANNEL_4
                              |SDADC_CHANNEL_6, SDADC_CONTINUOUS_CONV_ON) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
	}

//  ConfParamStruct.InputMode = SDADC_INPUT_MODE_DIFF;
//  ConfParamStruct.Gain = SDADC_GAIN_1;
//  ConfParamStruct.CommonMode = SDADC_COMMON_MODE_VSSA;
//  ConfParamStruct.Offset = 0;
//  if (HAL_SDADC_PrepareChannelConfig(&hsdadc1, SDADC_CONF_INDEX_0, &ConfParamStruct) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }
//  if (HAL_SDADC_PrepareChannelConfig(&hsdadc1, SDADC_CONF_INDEX_1, &ConfParamStruct) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }
//  if (HAL_SDADC_PrepareChannelConfig(&hsdadc1, SDADC_CONF_INDEX_2, &ConfParamStruct) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }

	if (HAL_SDADC_AssociateChannelConfig(&hsdadc1, SDADC_CHANNEL_4, SDADC_CONF_INDEX_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Injected Channel 
    */
  if (HAL_SDADC_AssociateChannelConfig(&hsdadc1, SDADC_CHANNEL_6, SDADC_CONF_INDEX_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Injected Channel 
    */
  if (HAL_SDADC_AssociateChannelConfig(&hsdadc1, SDADC_CHANNEL_8, SDADC_CONF_INDEX_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}
static void SDADC1_THD_channel(void)
{
	if((last_THD_MEASURED==IL3_THD)||(last_THD_MEASURED==0))
	{
		if (HAL_SDADC_ConfigChannel(&hsdadc1, SDADC_CHANNEL_8, SDADC_CONTINUOUS_CONV_ON) != HAL_OK)
		{
			_Error_Handler(__FILE__, __LINE__);
		}
		uln_thd_channel=ULN1_CHANNEL;
	}
	else if(last_THD_MEASURED==ULN1_THD)
	{
		if (HAL_SDADC_ConfigChannel(&hsdadc1, SDADC_CHANNEL_4, SDADC_CONTINUOUS_CONV_ON) != HAL_OK)
		{
			_Error_Handler(__FILE__, __LINE__);
		}
		uln_thd_channel=ULN2_CHANNEL;
	}
	else if(last_THD_MEASURED==ULN2_THD)
	{
		if (HAL_SDADC_ConfigChannel(&hsdadc1, SDADC_CHANNEL_6, SDADC_CONTINUOUS_CONV_ON) != HAL_OK)
		{
			_Error_Handler(__FILE__, __LINE__);
		}
		uln_thd_channel=ULN3_CHANNEL;
	}
}
/* SDADC2 init function */
void MX_SDADC2_Init(void)
{

  SDADC_ConfParamTypeDef ConfParamStruct;

    /**Configure the SDADC low power mode, fast conversion mode,
    slow clock mode and SDADC1 reference voltage 
    */
//  hsdadc2.Instance = SDADC2;
//  hsdadc2.Init.IdleLowPowerMode = SDADC_LOWPOWER_NONE;
//  hsdadc2.Init.FastConversionMode = SDADC_FAST_CONV_ENABLE;
//  hsdadc2.Init.SlowClockMode = SDADC_SLOW_CLOCK_DISABLE;
//  hsdadc2.Init.ReferenceVoltage = SDADC_VREF_EXT;
	/**Configure the SDADC low power mode, fast conversion mode,
  slow clock mode and SDADC1 reference voltage 
  */
  hsdadc2.Instance = SDADC2;
  hsdadc2.Init.IdleLowPowerMode = SDADC_LOWPOWER_NONE;
  hsdadc2.Init.FastConversionMode = SDADC_FAST_CONV_DISABLE;
  hsdadc2.Init.SlowClockMode = SDADC_SLOW_CLOCK_DISABLE;
  hsdadc2.Init.ReferenceVoltage = SDADC_VREF_EXT;
  hsdadc2.InjectedTrigger = SDADC_SYNCHRONOUS_TRIGGER;
  if (HAL_SDADC_Init(&hsdadc2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

//    /**Configure The Regular Mode 
//    */
//  if (HAL_SDADC_SelectRegularTrigger(&hsdadc2, SDADC_SYNCHRONOUS_TRIGGER) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }
	    /**Configure the Injected Mode 
    */
  if (HAL_SDADC_SelectInjectedDelay(&hsdadc2, SDADC_INJECTED_DELAY_NONE) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_SDADC_SelectInjectedTrigger(&hsdadc2, SDADC_SYNCHRONOUS_TRIGGER) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	//tole sm mel na zacetk 2 -> 0 -> 4
  if (HAL_SDADC_InjectedConfigChannel(&hsdadc2, SDADC_CHANNEL_4|SDADC_CHANNEL_2
                              |SDADC_CHANNEL_0, SDADC_CONTINUOUS_CONV_ON) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Set parameters for SDADC configuration 0 Register 
    */
  ConfParamStruct.InputMode = SDADC_INPUT_MODE_DIFF;
  ConfParamStruct.Gain = SDADC_GAIN_1;
  ConfParamStruct.CommonMode = SDADC_COMMON_MODE_VSSA;
  ConfParamStruct.Offset = 0;
  if (HAL_SDADC_PrepareChannelConfig(&hsdadc2, SDADC_CONF_INDEX_0, &ConfParamStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
//  if (HAL_SDADC_PrepareChannelConfig(&hsdadc2, SDADC_CONF_INDEX_1, &ConfParamStruct) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }
//  if (HAL_SDADC_PrepareChannelConfig(&hsdadc2, SDADC_CONF_INDEX_2, &ConfParamStruct) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }
	
	
//	//channel IL1
//	if (HAL_SDADC_AssociateChannelConfig(&hsdadc2, SDADC_CHANNEL_0, SDADC_CONF_INDEX_0) != HAL_OK)
//	{
//			_Error_Handler(__FILE__, __LINE__);
//	}
//	if (HAL_SDADC_ConfigChannel(&hsdadc2, SDADC_CHANNEL_0|SDADC_CHANNEL_2|SDADC_CHANNEL_4, SDADC_CONTINUOUS_CONV_ON) != HAL_OK)
//	{
//			_Error_Handler(__FILE__, __LINE__);
//	}
//	//channel IL2
//	if (HAL_SDADC_AssociateChannelConfig(&hsdadc2, SDADC_CHANNEL_2, SDADC_CONF_INDEX_1) != HAL_OK)
//	{
//			_Error_Handler(__FILE__, __LINE__);
//	}
////	if (HAL_SDADC_ConfigChannel(&hsdadc2, SDADC_CHANNEL_2, SDADC_CONTINUOUS_CONV_OFF) != HAL_OK)
////	{
////			_Error_Handler(__FILE__, __LINE__);
////	}
//	HAL_SDADC_CalibrationStart(&hsdadc2,SDADC_CALIBRATION_SEQ_2);
//	while(HAL_SDADC_PollForCalibEvent(&hsdadc2,CALIBRATION_TIMEOUT)!=HAL_OK);
//	//channel IL3
//	if (HAL_SDADC_AssociateChannelConfig(&hsdadc2, SDADC_CHANNEL_4, SDADC_CONF_INDEX_2) != HAL_OK)
//	{
//			_Error_Handler(__FILE__, __LINE__);
//	}
////	if (HAL_SDADC_ConfigChannel(&hsdadc2, SDADC_CHANNEL_4, SDADC_CONTINUOUS_CONV_OFF) != HAL_OK)
////	{
////			_Error_Handler(__FILE__, __LINE__);
////	}
//	HAL_SDADC_CalibrationStart(&hsdadc2,SDADC_CALIBRATION_SEQ_3);
//	while(HAL_SDADC_PollForCalibEvent(&hsdadc2,CALIBRATION_TIMEOUT)!=HAL_OK);
	if (HAL_SDADC_AssociateChannelConfig(&hsdadc2, SDADC_CHANNEL_2, SDADC_CONF_INDEX_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Injected Channel 
    */
  if (HAL_SDADC_AssociateChannelConfig(&hsdadc2, SDADC_CHANNEL_0, SDADC_CONF_INDEX_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Injected Channel 
    */
  if (HAL_SDADC_AssociateChannelConfig(&hsdadc2, SDADC_CHANNEL_4, SDADC_CONF_INDEX_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
	}
	HAL_SDADC_CalibrationStart(&hsdadc2,SDADC_CALIBRATION_SEQ_1);
	while(HAL_SDADC_PollForCalibEvent(&hsdadc2,CALIBRATION_TIMEOUT)!=HAL_OK);
}
void SDADC2_set_THD(void) 
{
	SDADC_ConfParamTypeDef ConfParamStruct;
	
	if (HAL_SDADC_DeInit(&hsdadc2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	//mislem da mormo po deinit ponovno nastavt celotn sdadc
	hsdadc2.Instance = SDADC2;
  hsdadc2.Init.IdleLowPowerMode = SDADC_LOWPOWER_NONE;
  hsdadc2.Init.FastConversionMode = SDADC_FAST_CONV_ENABLE;
  hsdadc2.Init.SlowClockMode = SDADC_SLOW_CLOCK_DISABLE;
  hsdadc2.Init.ReferenceVoltage = SDADC_VREF_EXT;
  hsdadc2.RegularTrigger = SDADC_SOFTWARE_TRIGGER;
	if (HAL_SDADC_Init(&hsdadc2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	if (HAL_SDADC_SelectRegularTrigger(&hsdadc2, SDADC_SOFTWARE_TRIGGER) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

//		/* Disable SDADC */
//	hsdadc2.Instance->CR2 &= ~(SDADC_CR2_ADON);
//	
//	hsdadc2.Instance->CR2 &= ~(SDADC_CR2_FAST);
//  hsdadc2.Instance->CR2 |= SDADC_FAST_CONV_ENABLE;
//	/* Enable SDADC */
//  hsdadc2.Instance->CR2 |= SDADC_CR2_ADON;
//	
//	
//	
//  /* Wait end of stabilization */
//  while((hsdadc2.Instance->ISR & SDADC_ISR_STABIP) != 0U)
//  {
//  }
//	hsdadc2.State = HAL_SDADC_STATE_READY;
	//    /**Configure The Regular Mode 
//    */
//	ConfParamStruct.InputMode = SDADC_INPUT_MODE_DIFF;
//  ConfParamStruct.Gain = SDADC_GAIN_1;
//  ConfParamStruct.CommonMode = SDADC_COMMON_MODE_VSSA;
//  ConfParamStruct.Offset = 0;
//  if (HAL_SDADC_PrepareChannelConfig(&hsdadc2, SDADC_CONF_INDEX_0, &ConfParamStruct) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }
	if (HAL_SDADC_AssociateChannelConfig(&hsdadc2, SDADC_CHANNEL_2, SDADC_CONF_INDEX_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Injected Channel 
    */
  if (HAL_SDADC_AssociateChannelConfig(&hsdadc2, SDADC_CHANNEL_0, SDADC_CONF_INDEX_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Injected Channel 
    */
  if (HAL_SDADC_AssociateChannelConfig(&hsdadc2, SDADC_CHANNEL_4, SDADC_CONF_INDEX_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
	}
	//SDADC2_THD_channel(); zaenkat zakomenteran
	if (HAL_SDADC_ConfigChannel(&hsdadc2, SDADC_CHANNEL_2, SDADC_CONTINUOUS_CONV_ON) != HAL_OK)
		{
			_Error_Handler(__FILE__, __LINE__);
		}
		SDADC2_THD_channel();
}
void SDADC2_set_normal(void) 
{
	SDADC_ConfParamTypeDef ConfParamStruct;
	
	if (HAL_SDADC_DeInit(&hsdadc2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  hsdadc2.Instance = SDADC2;
  hsdadc2.Init.IdleLowPowerMode = SDADC_LOWPOWER_NONE;
  hsdadc2.Init.FastConversionMode = SDADC_FAST_CONV_DISABLE;
  hsdadc2.Init.SlowClockMode = SDADC_SLOW_CLOCK_DISABLE;
  hsdadc2.Init.ReferenceVoltage = SDADC_VREF_EXT;
  hsdadc2.InjectedTrigger = SDADC_SYNCHRONOUS_TRIGGER;
  if (HAL_SDADC_Init(&hsdadc2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	
//	//		/* Disable SDADC */
//	hsdadc2.Instance->CR2 &= ~(SDADC_CR2_ADON);
//	
//	hsdadc2.Instance->CR2 &= ~(SDADC_CR2_FAST);
//  hsdadc2.Instance->CR2 |= SDADC_FAST_CONV_DISABLE;
//	/* Enable SDADC */
//  hsdadc2.Instance->CR2 |= SDADC_CR2_ADON;
//	
//	
//	
//  /* Wait end of stabilization */
//  while((hsdadc2.Instance->ISR & SDADC_ISR_STABIP) != 0U)
//  {
//  }
//	hsdadc2.State = HAL_SDADC_STATE_READY;
////	
//	    /**Configure the Injected Mode 
//    */
  if (HAL_SDADC_SelectInjectedDelay(&hsdadc2, SDADC_INJECTED_DELAY_NONE) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_SDADC_SelectInjectedTrigger(&hsdadc2, SDADC_SYNCHRONOUS_TRIGGER) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_SDADC_InjectedConfigChannel(&hsdadc2, SDADC_CHANNEL_4|SDADC_CHANNEL_2
                              |SDADC_CHANNEL_0, SDADC_CONTINUOUS_CONV_ON) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Set parameters for SDADC configuration 0 Register 
    */
//  ConfParamStruct.InputMode = SDADC_INPUT_MODE_DIFF;
//  ConfParamStruct.Gain = SDADC_GAIN_1;
//  ConfParamStruct.CommonMode = SDADC_COMMON_MODE_VSSA;
//  ConfParamStruct.Offset = 0;
//  if (HAL_SDADC_PrepareChannelConfig(&hsdadc2, SDADC_CONF_INDEX_0, &ConfParamStruct) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }
//  if (HAL_SDADC_PrepareChannelConfig(&hsdadc2, SDADC_CONF_INDEX_1, &ConfParamStruct) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }
//  if (HAL_SDADC_PrepareChannelConfig(&hsdadc2, SDADC_CONF_INDEX_2, &ConfParamStruct) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }
	
	if (HAL_SDADC_AssociateChannelConfig(&hsdadc2, SDADC_CHANNEL_2, SDADC_CONF_INDEX_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	
    /**Configure the Injected Channel 
    */
  if (HAL_SDADC_AssociateChannelConfig(&hsdadc2, SDADC_CHANNEL_0, SDADC_CONF_INDEX_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Injected Channel 
    */
  if (HAL_SDADC_AssociateChannelConfig(&hsdadc2, SDADC_CHANNEL_4, SDADC_CONF_INDEX_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
	}
}
static void SDADC2_THD_channel(void)
{
	if(last_THD_MEASURED==IL2_THD)
	{
		if (HAL_SDADC_ConfigChannel(&hsdadc2, SDADC_CHANNEL_2, SDADC_CONTINUOUS_CONV_ON) != HAL_OK)
		{
			_Error_Handler(__FILE__, __LINE__);
		}
		il_thd_channel=IL3_CHANNEL;
	}
	else if(last_THD_MEASURED==IL1_THD)
	{
		if (HAL_SDADC_ConfigChannel(&hsdadc2, SDADC_CHANNEL_0, SDADC_CONTINUOUS_CONV_ON) != HAL_OK)
		{
			_Error_Handler(__FILE__, __LINE__);
		}
		il_thd_channel=IL2_CHANNEL;
	}
	else if((last_THD_MEASURED==ULN3_THD)||(last_THD_MEASURED==0))
	{
		if (HAL_SDADC_ConfigChannel(&hsdadc2, SDADC_CHANNEL_4, SDADC_CONTINUOUS_CONV_ON) != HAL_OK)
		{
			_Error_Handler(__FILE__, __LINE__);
		}
		il_thd_channel=IL1_CHANNEL;
	}
}
/* SDADC3 init function */
void MX_SDADC3_Init(void)
{

  SDADC_ConfParamTypeDef ConfParamStruct;

    /**Configure the SDADC low power mode, fast conversion mode,
    slow clock mode and SDADC1 reference voltage 
    */
  hsdadc3.Instance = SDADC3;
  hsdadc3.Init.IdleLowPowerMode = SDADC_LOWPOWER_NONE;
  hsdadc3.Init.FastConversionMode = SDADC_FAST_CONV_DISABLE;
  hsdadc3.Init.SlowClockMode = SDADC_SLOW_CLOCK_DISABLE;
  hsdadc3.Init.ReferenceVoltage = SDADC_VREF_EXT;
	hsdadc3.InjectedTrigger = SDADC_SOFTWARE_TRIGGER;
  if (HAL_SDADC_Init(&hsdadc3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure The Regular Mode 
    */
	if (HAL_SDADC_SelectInjectedDelay(&hsdadc3, SDADC_INJECTED_DELAY) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  if (HAL_SDADC_SelectInjectedTrigger(&hsdadc3, SDADC_SOFTWARE_TRIGGER) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	//za zacetek nastavimo tri kanale za sinhrono zajemanje
	if (HAL_SDADC_InjectedConfigChannel(&hsdadc3, SDADC_CHANNEL_0|SDADC_CHANNEL_2
                              |SDADC_CHANNEL_4, SDADC_CONTINUOUS_CONV_ON) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Set parameters for SDADC configuration 0 Register 
    */
  ConfParamStruct.InputMode = SDADC_INPUT_MODE_DIFF;
  ConfParamStruct.Gain = SDADC_GAIN_1;
  ConfParamStruct.CommonMode = SDADC_COMMON_MODE_VSSA;
  ConfParamStruct.Offset = 0;
  if (HAL_SDADC_PrepareChannelConfig(&hsdadc3, SDADC_CONF_INDEX_0, &ConfParamStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

	if (HAL_SDADC_AssociateChannelConfig(&hsdadc3, SDADC_CHANNEL_0, SDADC_CONF_INDEX_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
    /**Configure the Injected Channel 
    */
  if (HAL_SDADC_AssociateChannelConfig(&hsdadc3, SDADC_CHANNEL_2, SDADC_CONF_INDEX_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
    /**Configure the Injected Channel 
    */
  if (HAL_SDADC_AssociateChannelConfig(&hsdadc3, SDADC_CHANNEL_4, SDADC_CONF_INDEX_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
	}
	HAL_SDADC_CalibrationStart(&hsdadc3,SDADC_CALIBRATION_SEQ_1);
	while(HAL_SDADC_PollForCalibEvent(&hsdadc3,CALIBRATION_TIMEOUT)!=HAL_OK);
	//-----------------------------------------------------------------------
}

float get_value(uint32_t param)
{
	switch(param)
	{
		case __IDIFF: return IDIFF.effective;
		case __ULN1: return ULN1.effective;
		case __ULN2: return ULN2.effective;
		case __ULN3: return ULN3.effective;
		case __IL1: return IL1.effective;
		case __IL2: return IL2.effective;
		case __IL3: return IL3.effective;
		case __UL1PE: return UL1PE.effective;
		case __UNPE: return UNPE.effective;
		default: return 0;
	}
}
float get_inst_value(uint32_t param)
{
	switch(param)
	{
		case __IDIFF: return SDADC3_CH1s.sempl;
		case __ULN1: return SDADC1_CH1s.sempl;
		case __ULN2: return SDADC1_CH1s.sempl;
		case __ULN3: return SDADC1_CH1s.sempl;
		case __IL1: return SDADC2_CH1s.sempl;
		case __IL2: return SDADC2_CH1s.sempl;
		case __IL3: return SDADC2_CH1s.sempl;
		case __UL1PE: return SDADC3_CH2s.sempl;
		case __UNPE: return SDADC3_CH3s.sempl;
		default: return 0;
	}
}
void SDADC1_Handler(void)
{
	int16_t ConvertionResult;
	int16_t ConvertionResult2;
	int16_t ConvertionResult3;
	uint32_t channel1;
	uint32_t channel2;
	//ko je na vrsti THD meritev dobimo vrednost v SDADC1 in SDADC2 handlerju v nasprotnem primeru
	//dobimo obe vrednosti SDADCja 2 in 1 v SDADC1_Handler-ju
	if((meas_control & __THD_MEASURING)&&(THD_COMPUTATION_METHOD==CORELATION))
	{
		ConvertionResult = HAL_SDADC_GetValue(&hsdadc1);
		channel1= uln_thd_channel;
	}
	else
	{
		ConvertionResult3 = HAL_SDADC_InjectedGetValue(&hsdadc2, &channel2);
		ConvertionResult = HAL_SDADC_InjectedGetValue(&hsdadc1, &channel1);
		ConvertionResult2 = HAL_SDADC_InjectedMultiModeGetValue(&hsdadc1);
	}
	channel_global=channel1;
	measure_ULN_Voltage(ConvertionResult, channel1);
	if((meas_control & __MULTIMODE_MASK)&&((!(meas_control & __THD_MEASURING))||(!(THD_COMPUTATION_METHOD==CORELATION))))
	{
		measure_IL_Current(ConvertionResult3, channel2);
	}
}
void SDADC2_Handler(void)
{
	int16_t ConvertionResult;
	//ko je na vrsti THD meritev dobimo vrednost v SDADC1 in SDADC2 handlerju v nasprotnem primeru
	//dobimo obe vrednosti SDADCja 2 in 1 v SDADC1_Handler-ju
	if((meas_control & __THD_MEASURING)&&(THD_COMPUTATION_METHOD==CORELATION))
	{
		ConvertionResult = HAL_SDADC_GetValue(&hsdadc2);
		measure_IL_Current(ConvertionResult, il_thd_channel);
	}
}
void SDADC3_Handler()
{
	int16_t ConvertionResult;
	uint32_t channel;
	ConvertionResult = HAL_SDADC_InjectedGetValue(&hsdadc3, &channel);
	switch(channel)
	{
		case IDIFF_CHANNEL: measure_IDIFF(ConvertionResult); break;
		case UL1PE_CHANNEL: measure_UL1PE(ConvertionResult); break;
		case UNPE_CHANNEL: measure_UNPE(ConvertionResult); break;
		default: break;		
	}
	
}
void IL_setGain(uint32_t channel, uint8_t gain)
{
	if(gain==__GAIN40)
	{
		switch(channel)
		{
			case IL1_CHANNEL:
			{
				HAL_GPIO_WritePin(CURRENT_GAIN1_GPIO_Port, CURRENT_GAIN1_Pin, _OFF);
				IL1_GAIN=__GAIN40;
				break;
			}
			case IL2_CHANNEL:
			{
				HAL_GPIO_WritePin(CURRENT_GAIN2_GPIO_Port, CURRENT_GAIN2_Pin, _OFF);
				IL2_GAIN=__GAIN40;
				break;
			}
			case IL3_CHANNEL:
			{
				HAL_GPIO_WritePin(CURRENT_GAIN3_GPIO_Port, CURRENT_GAIN3_Pin, _OFF);
				IL3_GAIN=__GAIN40;
				break;
			}
			default: break;
		}
	}
	else
	{
		switch(channel)
		{
			case IL1_CHANNEL:
			{
				HAL_GPIO_WritePin(CURRENT_GAIN1_GPIO_Port, CURRENT_GAIN1_Pin, _ON);
				IL1_GAIN=__GAIN1;
				break;
			}
			case IL2_CHANNEL:
			{
				HAL_GPIO_WritePin(CURRENT_GAIN2_GPIO_Port, CURRENT_GAIN2_Pin, _ON);
				IL2_GAIN=__GAIN1;
				break;
			}
			case IL3_CHANNEL:
			{
				HAL_GPIO_WritePin(CURRENT_GAIN3_GPIO_Port, CURRENT_GAIN3_Pin, _ON);
				IL3_GAIN=__GAIN1;
				break;
			}
			default: break;
		}
	}
}
static void measure_ULN_Voltage(int16_t ConvertionResult, uint32_t channel)
{
//	static uint32_t count1=0;
	static uint32_t thd_counter=0;
	static uint32_t thd_counter2=0;
  float SDADC1_sample;
	float SDADC1_sample2;
	ULN1.correction = 1;
	ULN2.correction = 1;
	ULN3.correction = 1;
	float uln1_temp;	
	float uln2_temp;
	float uln3_temp;
	uln1_temp = ULN1.correction*ULN1_GAIN1;
	uln2_temp = ULN2.correction*ULN2_GAIN1;
	uln3_temp = ULN3.correction*ULN3_GAIN1;
	#if THD_FFT_WINDOW != 0
	float nminus1inv;
	nminus1inv=(float)(1.0/FFT_SAMPLE_NUMBER);
	#endif

	switch(channel)
	{
		case ULN1_CHANNEL: 
		{
			SDADC1_sample = ConvertionResult*uln1_temp;
			#if	THD_COMPUTATION_METHOD == CORELATION
			if(meas_control & __THD_MEASURING)
			{
				if((!(meas_control & __ULN_THD_MEASURED))&&(compute_control & __ULN1_THD_SAMPLING)&&(current_THD_sampling==__ULN1_THD_SAMPLING))
				{
					if(thd_counter2>=4)
					{
						#if THD_FFT_WINDOW == __FLAT_TOP
						SDADC1_sample2 = SDADC1_sample*(1.0f-1.93f * arm_cos_f32(__2PI*thd_counter*nminus1inv)+1.29f * arm_cos_f32(__4PI*thd_counter*nminus1inv)-0.388f * arm_cos_f32(__6PI*thd_counter*nminus1inv)+0.028f * arm_cos_f32(__8PI*thd_counter*nminus1inv));
						#elif THD_FFT_WINDOW == __HANN
						SDADC1_sample2 = SDADC1_sample*(0.5f-0.5f*arm_cos_f32(__2PI*thd_counter*nminus1inv));
						#elif THD_FFT_WINDOW == __HAMMING
						SDADC1_sample2 = SDADC1_sample*(0.54f-0.46f*arm_cos_f32(__2PI*thd_counter*nminus1inv));
						#else 
						SDADC1_sample2 = SDADC1_sample;
						#endif
						THD_buffer1[thd_counter]=SDADC1_sample2;
						thd_counter2=0;
						if(thd_counter>=CORELATION_SAMPLE_NUMBER)
						{
							compute_control &= (~__ULN1_THD_SAMPLING);
							compute_control |= __ULN1_THD_SAMPLED;
							last_THD_MEASURED=ULN1_THD;
							thd_counter=0;
							meas_control |= __ULN_THD_MEASURED;
							set_event(COMPUTE_THD_WITH_CORELATION,compute_THD_with_corelation);
						}
						else thd_counter++;
					}
					else thd_counter2++;
				}
			}
			#else
			if((compute_control & __ULN1_THD_SAMPLING)&&(!(meas_control &__ULN_THD_MEASURED))&&(current_THD_sampling==__ULN1_THD_SAMPLING)&&(meas_control & __THD_MEASURING))
			{
				#if THD_FFT_WINDOW == __FLAT_TOP
				SDADC1_sample2 = SDADC1_sample*(1.0f-1.93f * arm_cos_f32(__2PI*thd_counter*nminus1inv)+1.29f * arm_cos_f32(__4PI*thd_counter*nminus1inv)-0.388f * arm_cos_f32(__6PI*thd_counter*nminus1inv)+0.028f * arm_cos_f32(__8PI*thd_counter*nminus1inv));
				#elif THD_FFT_WINDOW == __HANN
				SDADC1_sample2 = SDADC1_sample*(0.5f-0.5f*arm_cos_f32(__2PI*thd_counter*nminus1inv));
				#elif THD_FFT_WINDOW == __HAMMING
				SDADC1_sample2 = SDADC1_sample*(0.54f-0.46f*arm_cos_f32(__2PI*thd_counter*nminus1inv));
				#else 
				SDADC1_sample2 = SDADC1_sample;
				#endif
				THD_buffer1[thd_counter*2]=SDADC1_sample2;
				THD_buffer1[(thd_counter*2)+1]=0;
				if(thd_counter>=FFT_SAMPLE_NUMBER)
				{
					compute_control |= __ULN1_THD_SAMPLED;
					compute_control &= (~__ULN1_THD_SAMPLING);
					last_THD_MEASURED=ULN1_THD;
					thd_counter=0;
					meas_control |= __ULN_THD_MEASURED;	//zastavica je enotna za vse IL faze, zato ker se meri samo ena faza naenkrat in to zadostuje
					set_event(COMPUTE_THD_WITH_FFT,compute_THD_with_FFT);
				}
				else thd_counter++;
			}
			#endif
			//if(((!(meas_control & __ULN1_MEASURED))||(meas_control & __NO_THD_MEAS))&&((!(meas_control & __THD_MEASURING))||(THD_COMPUTATION_METHOD==FFT)))
			if(((!(meas_control & __ULN1_MEASURED))&&(!(meas_control & __THD_MEASURING)))||(THD_COMPUTATION_METHOD==FFT)||(meas_control & __NO_THD_MEAS))
			{
				//ULN1 - HIGH pass Fs=16,66kHz Fc=1Hz
				//--------------------------------------
				if(SDADC1_high_pass_filter==_ON)
				{
					SDADC1_sample2=SDADC1_sample;
					arm_biquad_cascade_df1_f32(&INST_ULN1_HPF, &SDADC1_sample2, &SDADC1_sample,1);
				}
				if(SDADC1_low_pass_filter==_ON)
				{
					SDADC1_sample2=SDADC1_sample;
					arm_biquad_cascade_df1_f32(&INST_ULN1_LPF, &SDADC1_sample2, &SDADC1_sample,1);
				}
			}
			break;
		}
		case ULN2_CHANNEL: 
		{
			SDADC1_sample = ConvertionResult*uln2_temp; 
			#if	THD_COMPUTATION_METHOD == CORELATION
			if(meas_control & __THD_MEASURING)
			{
				if((!(meas_control & __ULN_THD_MEASURED))&&(compute_control & __ULN2_THD_SAMPLING)&&(current_THD_sampling==__ULN2_THD_SAMPLING))
				{
					if(thd_counter2>=4)
					{
						#if THD_FFT_WINDOW == __FLAT_TOP
						SDADC1_sample2 = SDADC1_sample*(1.0f-1.93f * arm_cos_f32(__2PI*thd_counter*nminus1inv)+1.29f * arm_cos_f32(__4PI*thd_counter*nminus1inv)-0.388f * arm_cos_f32(__6PI*thd_counter*nminus1inv)+0.028f * arm_cos_f32(__8PI*thd_counter*nminus1inv));
						#elif THD_FFT_WINDOW == __HANN
						SDADC1_sample2 = SDADC1_sample*(0.5f-0.5f*arm_cos_f32(__2PI*thd_counter*nminus1inv));
						#elif THD_FFT_WINDOW == __HAMMING
						SDADC1_sample2 = SDADC1_sample*(0.54f-0.46f*arm_cos_f32(__2PI*thd_counter*nminus1inv));
						#else 
						SDADC1_sample2 = SDADC1_sample;
						#endif
						THD_buffer1[thd_counter]=SDADC1_sample2;
						thd_counter2=0;
						if(thd_counter>=CORELATION_SAMPLE_NUMBER)
						{
							compute_control &= (~__ULN2_THD_SAMPLING);
							compute_control |= __ULN2_THD_SAMPLED;
							last_THD_MEASURED=ULN2_THD;
							thd_counter=0;
							meas_control |= __ULN_THD_MEASURED;
							set_event(COMPUTE_THD_WITH_CORELATION,compute_THD_with_corelation);
						}
						else thd_counter++;
					}
					else thd_counter2++;
				}
			}
			#else
			if((compute_control & __ULN2_THD_SAMPLING)&&(!(meas_control &__ULN_THD_MEASURED))&&(current_THD_sampling==__ULN2_THD_SAMPLING)&&(meas_control & __THD_MEASURING))
			{
				#if THD_FFT_WINDOW == __FLAT_TOP
				SDADC1_sample2 = SDADC1_sample*(1.0f-1.93f * arm_cos_f32(__2PI*thd_counter*nminus1inv)+1.29f * arm_cos_f32(__4PI*thd_counter*nminus1inv)-0.388f * arm_cos_f32(__6PI*thd_counter*nminus1inv)+0.028f * arm_cos_f32(__8PI*thd_counter*nminus1inv));
				#elif THD_FFT_WINDOW == __HANN
				SDADC1_sample2 = SDADC1_sample*(0.5f-0.5f*arm_cos_f32(__2PI*thd_counter*nminus1inv));
				#elif THD_FFT_WINDOW == __HAMMING
				SDADC1_sample2 = SDADC1_sample*(0.54f-0.46f*arm_cos_f32(__2PI*thd_counter*nminus1inv));
				#else 
				SDADC1_sample2 = SDADC1_sample;
				#endif
				THD_buffer1[thd_counter*2]=SDADC1_sample2;
				THD_buffer1[(thd_counter*2)+1]=0;
				if(thd_counter>=FFT_SAMPLE_NUMBER)
				{
					compute_control |= __ULN2_THD_SAMPLED;
					compute_control &= (~__ULN2_THD_SAMPLING);
					last_THD_MEASURED=ULN2_THD;
					thd_counter=0;
					meas_control |= __ULN_THD_MEASURED;	//zastavica je enotna za vse IL faze, zato ker se meri samo ena faza naenkrat in to zadostuje
					set_event(COMPUTE_THD_WITH_FFT,compute_THD_with_FFT);
				}
				else thd_counter++;
			}
			#endif
			//if(((!(meas_control & __ULN2_MEASURED))||(meas_control & __NO_THD_MEAS))&&((!(meas_control & __THD_MEASURING))||(THD_COMPUTATION_METHOD==FFT)))
			if(((!(meas_control & __ULN2_MEASURED))&&(!(meas_control & __THD_MEASURING)))||(THD_COMPUTATION_METHOD==FFT)||(meas_control & __NO_THD_MEAS))
			{
				if(SDADC1_high_pass_filter==_ON)
				{
					SDADC1_sample2=SDADC1_sample;
					arm_biquad_cascade_df1_f32(&INST_ULN2_HPF, &SDADC1_sample2, &SDADC1_sample,1);
				}
				if(SDADC1_low_pass_filter==_ON)
				{
					SDADC1_sample2=SDADC1_sample;
					arm_biquad_cascade_df1_f32(&INST_ULN2_LPF, &SDADC1_sample2, &SDADC1_sample,1);
				}
			}
			break;
		}
		case ULN3_CHANNEL: 
		{
			SDADC1_sample = ConvertionResult*uln3_temp;
			#if	THD_COMPUTATION_METHOD == CORELATION
			if(meas_control & __THD_MEASURING)
			{
				if((!(meas_control & __ULN_THD_MEASURED))&&(compute_control & __ULN3_THD_SAMPLING)&&(current_THD_sampling==__ULN3_THD_SAMPLING))
				{
					if(thd_counter2>=4)
					{
						#if THD_FFT_WINDOW == __FLAT_TOP
						SDADC1_sample2 = SDADC1_sample*(1.0f-1.93f * arm_cos_f32(__2PI*thd_counter*nminus1inv)+1.29f * arm_cos_f32(__4PI*thd_counter*nminus1inv)-0.388f * arm_cos_f32(__6PI*thd_counter*nminus1inv)+0.028f * arm_cos_f32(__8PI*thd_counter*nminus1inv));
						#elif THD_FFT_WINDOW == __HANN
						SDADC1_sample2 = SDADC1_sample*(0.5f-0.5f*arm_cos_f32(__2PI*thd_counter*nminus1inv));
						#elif THD_FFT_WINDOW == __HAMMING
						SDADC1_sample2 = SDADC1_sample*(0.54f-0.46f*arm_cos_f32(__2PI*thd_counter*nminus1inv));
						#else 
						SDADC1_sample2 = SDADC1_sample;
						#endif
						THD_buffer1[thd_counter]=SDADC1_sample2;
						thd_counter2=0;
						if(thd_counter>=CORELATION_SAMPLE_NUMBER)
						{
							last_THD_MEASURED=ULN3_THD;
							compute_control &= (~__ULN3_THD_SAMPLING);
							compute_control |= __ULN3_THD_SAMPLED;
							thd_counter=0;
							meas_control |= __ULN_THD_MEASURED;
							set_event(COMPUTE_THD_WITH_CORELATION,compute_THD_with_corelation);
						}
						else thd_counter++;
					}
					else thd_counter2++;
				}
			}
			#else
			if((compute_control & __ULN3_THD_SAMPLING)&&(!(meas_control &__ULN_THD_MEASURED))&&(current_THD_sampling==__ULN3_THD_SAMPLING)&&(meas_control & __THD_MEASURING))
			{
				#if THD_FFT_WINDOW == __FLAT_TOP
				SDADC1_sample2 = SDADC1_sample*(1.0f-1.93f * arm_cos_f32(__2PI*thd_counter*nminus1inv)+1.29f * arm_cos_f32(__4PI*thd_counter*nminus1inv)-0.388f * arm_cos_f32(__6PI*thd_counter*nminus1inv)+0.028f * arm_cos_f32(__8PI*thd_counter*nminus1inv));
				#elif THD_FFT_WINDOW == __HANN
				SDADC1_sample2 = SDADC1_sample*(0.5f-0.5f*arm_cos_f32(__2PI*thd_counter*nminus1inv));
				#elif THD_FFT_WINDOW == __HAMMING
				SDADC1_sample2 = SDADC1_sample*(0.54f-0.46f*arm_cos_f32(__2PI*thd_counter*nminus1inv));
				#else 
				SDADC1_sample2 = SDADC1_sample;
				#endif
				THD_buffer1[thd_counter*2]=SDADC1_sample2;
				THD_buffer1[(thd_counter*2)+1]=0;
				if(thd_counter>=FFT_SAMPLE_NUMBER)
				{
					compute_control |= __ULN3_THD_SAMPLED;
					compute_control &= (~__ULN3_THD_SAMPLING);
					last_THD_MEASURED=ULN3_THD;
					thd_counter=0;
					meas_control |= __ULN_THD_MEASURED;	//zastavica je enotna za vse IL faze, zato ker se meri samo ena faza naenkrat in to zadostuje
					set_event(COMPUTE_THD_WITH_FFT,compute_THD_with_FFT);
				}
				else thd_counter++;
			}
			#endif
			//if(((!(meas_control & __ULN3_MEASURED))||(meas_control & __NO_THD_MEAS))&&((!(meas_control & __THD_MEASURING))||(THD_COMPUTATION_METHOD==FFT)))
			if(((!(meas_control & __ULN3_MEASURED))&&(!(meas_control & __THD_MEASURING)))||(THD_COMPUTATION_METHOD==FFT)||(meas_control & __NO_THD_MEAS))
			{
				if(SDADC1_high_pass_filter==_ON)
				{
					SDADC1_sample2=SDADC1_sample;
					arm_biquad_cascade_df1_f32(&INST_ULN3_HPF, &SDADC1_sample2, &SDADC1_sample,1);
				}
				if(SDADC1_low_pass_filter==_ON)
				{
					SDADC1_sample2=SDADC1_sample;
					arm_biquad_cascade_df1_f32(&INST_ULN3_LPF, &SDADC1_sample2, &SDADC1_sample,1);
				}
			}
			break;
		}
			default: break;
	}
	//sledeca koda se izvaja v primeru da ni THD meritev
	if((!(meas_control & __THD_MEASURING))||(THD_COMPUTATION_METHOD==FFT))
	{
		switch(channel)
		{
			case ULN1_CHANNEL: 
			{
				if((!(meas_control & __ULN1_MEASURED))||(meas_control & __NO_THD_MEAS)||(THD_COMPUTATION_METHOD==FFT))
				{
					//koda za detektiranje faznega zaporedja napetosti
					if(SDADC1_CH1s.sample_count==0) test1_on;
					if((SDADC1_CH1s.sample_count<(uint32_t)(ULN_MEAS_FS*0.02f))&&(compute_control2 & __GET_PHASES)&&(!(compute_control2 & __PHASE1_MEASURED)))//samo 1. periodo gledamo za maksimum
					{
						if(SDADC1_CH1s.sempl > SDADC1_CH1s.max_float)
						{
							SDADC1_CH1s.peak_at = (htim6.Instance -> CNT)+12;//timer ki steje tece na 10 us ker je frekvenca vzorcenja 16,666 za vse kanale je 60us zakasnitve med vzorcenji kanalov
							SDADC1_CH1s.max_float=SDADC1_CH1s.sempl;
						}
					}
					else if((SDADC1_CH1s.sample_count>=(uint32_t)(ULN_MEAS_FS*0.02f))&&(compute_control2 & __GET_PHASES)&&(!(compute_control2 & __PHASE1_MEASURED)))
					{compute_control2 |= __PHASE1_MEASURED;test1_off;}
					SDADC1_CH1s.sempl=SDADC1_sample;
					SDADC1_CH1s.sum += (SDADC1_CH1s.sempl*SDADC1_CH1s.sempl);
					if(SDADC1_CH1s.sample_count>=SDADC1_ULN_SAMPLE_CNT)
					{
						if(!(compute_control & __ULN1_SAMPLED))ULN1.avarage_sq = SDADC1_CH1s.sum/(SDADC1_CH1s.sample_count+1);
						SDADC1_CH1s.sample_count = 0;
						SDADC1_CH1s.sum = 0;
						compute_control |= __ULN1_SAMPLED;	//postavitev zastavice za izracun efektivne vrednosti
						meas_control |= __ULN1_MEASURED;
					}
					else SDADC1_CH1s.sample_count++;
				}
				break;
			}
			case ULN2_CHANNEL: 
			{
				if((!(meas_control & __ULN2_MEASURED))||(meas_control & __NO_THD_MEAS)||(THD_COMPUTATION_METHOD==FFT))
				{
					//koda za detektiranje faznega zaporedja napetosti
					if((SDADC1_CH2s.sample_count<(uint32_t)(ULN_MEAS_FS*0.02f))&&(compute_control2 & __GET_PHASES)&&(!(compute_control2 & __PHASE2_MEASURED)))//samo 1. periodo gledamo za maksimum
					{
						if(SDADC1_CH2s.sempl > SDADC1_CH2s.max_float)
						{
							SDADC1_CH2s.peak_at = (htim6.Instance -> CNT)+6;//timer ki steje tece na 10 us ker je frekvenca vzorcenja 16,666 za vse kanale je 60us zakasnitve med vzorcenji kanalov
							SDADC1_CH2s.max_float=SDADC1_CH2s.sempl;
						}
					}
					else if((SDADC1_CH2s.sample_count>=(uint32_t)(ULN_MEAS_FS*0.02f))&&(compute_control2 & __GET_PHASES)&&(!(compute_control2 & __PHASE2_MEASURED)))
						compute_control2 |= __PHASE2_MEASURED;
					SDADC1_CH2s.sempl=SDADC1_sample;
					SDADC1_CH2s.sum += (SDADC1_CH2s.sempl*SDADC1_CH2s.sempl);
					if(SDADC1_CH2s.sample_count>=SDADC1_ULN_SAMPLE_CNT)
					{
						if(!(compute_control & __ULN2_SAMPLED))ULN2.avarage_sq = (SDADC1_CH2s.sum/(SDADC1_CH2s.sample_count+1));
						SDADC1_CH2s.sample_count = 0;
						SDADC1_CH2s.sum = 0;
						compute_control |= __ULN2_SAMPLED;	//postavitev zastavice za izracun efektivne vrednosti
						meas_control |= __ULN2_MEASURED;
					}
					else SDADC1_CH2s.sample_count++;
				}
				break;
			}
			case ULN3_CHANNEL: 
			{
				if((!(meas_control & __ULN3_MEASURED))||(meas_control & __NO_THD_MEAS)||(THD_COMPUTATION_METHOD==FFT))
				{
					//koda za detektiranje faznega zaporedja napetosti
					if((SDADC1_CH3s.sample_count<(uint32_t)(ULN_MEAS_FS*0.02f))&&(compute_control2 & __GET_PHASES)&&(!(compute_control2 & __PHASE3_MEASURED)))//samo 1. periodo gledamo za maksimum
					{
						if(SDADC1_CH3s.sempl > SDADC1_CH3s.max_float)
						{
							SDADC1_CH3s.peak_at = htim6.Instance -> CNT;
							SDADC1_CH3s.max_float=SDADC1_CH3s.sempl;
						}
					}
					else if((SDADC1_CH3s.sample_count>=(uint32_t)(ULN_MEAS_FS*0.02f))&&(compute_control2 & __GET_PHASES)&&(!(compute_control2 & __PHASE3_MEASURED)))
						compute_control2 |= __PHASE3_MEASURED;
					SDADC1_CH3s.sempl=SDADC1_sample;
					SDADC1_CH3s.sum += (SDADC1_CH3s.sempl*SDADC1_CH3s.sempl);
					if(SDADC1_CH3s.sample_count>=SDADC1_ULN_SAMPLE_CNT)
					{
						if(!(compute_control & __ULN3_SAMPLED))ULN3.avarage_sq = (SDADC1_CH3s.sum/(SDADC1_CH3s.sample_count+1));
						SDADC1_CH3s.sample_count = 0;
						SDADC1_CH3s.sum = 0;
						compute_control |= __ULN3_SAMPLED;	//postavitev zastavice za izracun efektivne vrednosti
						meas_control |= __ULN3_MEASURED;
					}
					else SDADC1_CH3s.sample_count++;
				}
				break;
			}
			default: break;
		}
//		if((compute_control2 & __GET_PHASES)&&(meas_control & __ULN1_MEASURED)&&(meas_control & __ULN2_MEASURED)&&(meas_control & __ULN3_MEASURED))
//			count1++;
	}
}
static void measure_IL_Current(int16_t ConvertionResult, uint32_t channel)
{
	static uint32_t thd_counter	=	0;
	static uint32_t thd_counter2=	0;
  float SDADC2_sample;
	float SDADC2_sample2;
	float il1_temp;	
	float il2_temp;
	float il3_temp;
	float Hanning_coef;
	float cos;
	static uint32_t count=1;
	static uint32_t count2=1;
	static uint32_t u=0;
	#if THD_FFT_WINDOW != 0
	float nminus1inv;
	nminus1inv=(float)(1.0/FFT_SAMPLE_NUMBER);
	#endif
	//-------------------------racunanje korekcijske spremenljivke-----------------------
	//ko bo narejen umerjanje s softwerom zamenjaj ILX_GAIN1_CORRECTION z ILX.correction
	//in pobrisi spodnje tri vrstice
	IL1.correction = 1;
	IL2.correction = 1;
	IL3.correction = 1;
	if(IL1_GAIN==__GAIN1) il1_temp = IL1_GAIN1_CORRECTION*IL1_GAIN1;
	else il1_temp = IL1_GAIN40_CORRECTION*IL1_GAIN40;
	if(IL2_GAIN==__GAIN1) il2_temp = IL2_GAIN1_CORRECTION*IL2_GAIN1;
	else il2_temp = IL2_GAIN40_CORRECTION*IL2_GAIN40;
	if(IL3_GAIN==__GAIN1) il3_temp = IL3_GAIN1_CORRECTION*IL3_GAIN1;
	else il3_temp = IL3_GAIN40_CORRECTION*IL3_GAIN40;
	//-----------------------------------------------------------------------------------
	switch(channel)
	{
		case IL1_CHANNEL: 
		{
			SDADC2_sample = ConvertionResult*il1_temp;
			#if	THD_COMPUTATION_METHOD == CORELATION
			if(meas_control & __THD_MEASURING)
			{
				if((!(meas_control & __IL_THD_MEASURED))&&(compute_control & __IL1_THD_SAMPLING)&&(current_THD_sampling==__IL1_THD_SAMPLING))
				{
					if(thd_counter2>=5)//vzamemo vsak peti vzorec, ker je frekvenca vzorcenja 50 KHz mi pa rabimo 10 kHz
					{
						#if THD_FFT_WINDOW == __FLAT_TOP
						SDADC2_sample2 = SDADC2_sample*(1.0f-1.93f * arm_cos_f32(__2PI*thd_counter*nminus1inv)+1.29f * arm_cos_f32(__4PI*thd_counter*nminus1inv)-0.388f * arm_cos_f32(__6PI*thd_counter*nminus1inv)+0.028f * arm_cos_f32(__8PI*thd_counter*nminus1inv));
						#elif THD_FFT_WINDOW == __HANN
						SDADC2_sample2 = SDADC2_sample*(0.5f-0.5f*arm_cos_f32(__2PI*thd_counter*nminus1inv));
						#elif THD_FFT_WINDOW == __HAMMING
						SDADC2_sample2 = SDADC2_sample*(0.54f-0.46f*arm_cos_f32(__2PI*thd_counter*nminus1inv));
						#else 
						SDADC2_sample2 = SDADC2_sample;
						#endif
						thd_counter2=0;
						reskrneki7	=	ConvertionResult;
						THD_buffer1[thd_counter]=SDADC2_sample2;
						if(thd_counter>=CORELATION_SAMPLE_NUMBER)
						{
							compute_control |= __IL1_THD_SAMPLED;
							compute_control &= (~__IL1_THD_SAMPLING);
							last_THD_MEASURED=IL1_THD;
							thd_counter=0;
							meas_control |= __IL_THD_MEASURED;	//zastavica je enotna za vse IL faze, zato ker se meri samo ena faza naenkrat in to zadostuje
							set_event(COMPUTE_THD_WITH_CORELATION,compute_THD_with_corelation);
						}
						else thd_counter++;
					}
					else thd_counter2++;
				}
			}
			//sledeca koda se nalozi samo ce imamo za THD racunanje izbran FFT
			#else
			if((compute_control & __IL1_THD_SAMPLING)&&(!(meas_control &__IL_THD_MEASURED))&&(current_THD_sampling==__IL1_THD_SAMPLING)&&(meas_control & __THD_MEASURING))
			{
				#if THD_FFT_WINDOW == __FLAT_TOP
				SDADC2_sample2 = SDADC2_sample*(1.0f-1.93f * arm_cos_f32(__2PI*thd_counter*nminus1inv)+1.29f * arm_cos_f32(__4PI*thd_counter*nminus1inv)-0.388f * arm_cos_f32(__6PI*thd_counter*nminus1inv)+0.028f * arm_cos_f32(__8PI*thd_counter*nminus1inv));
				#elif THD_FFT_WINDOW == __HANN
				SDADC2_sample2 = SDADC2_sample*(0.5f-0.5f*arm_cos_f32(__2PI*thd_counter*nminus1inv));
				#elif THD_FFT_WINDOW == __HAMMING
				SDADC2_sample2 = SDADC2_sample*(0.54f-0.46f*arm_cos_f32(__2PI*thd_counter*nminus1inv));
				#else 
				SDADC2_sample2 = SDADC2_sample;
				#endif
				THD_buffer1[thd_counter*2]=SDADC2_sample2;
				THD_buffer1[(thd_counter*2)+1]=0;
				if(thd_counter>=FFT_SAMPLE_NUMBER)
				{
					compute_control |= __IL1_THD_SAMPLED;
					compute_control &= (~__IL1_THD_SAMPLING);
					last_THD_MEASURED=IL1_THD;
					thd_counter=0;
					meas_control |= __IL_THD_MEASURED;	//zastavica je enotna za vse IL faze, zato ker se meri samo ena faza naenkrat in to zadostuje
					set_event(COMPUTE_THD_WITH_FFT,compute_THD_with_FFT);
				}
				else thd_counter++;
			}
			#endif
			//sledeci pogoj pomemben zarad filtrov
			//if(((!(meas_control & __IL1_MEASURED))||(meas_control & __NO_THD_MEAS))&&((!(meas_control & __THD_MEASURING))||(THD_COMPUTATION_METHOD==FFT)))
			if(((!(meas_control & __IL1_MEASURED))&&(!(meas_control & __THD_MEASURING)))||(THD_COMPUTATION_METHOD==FFT)||(meas_control & __NO_THD_MEAS))
			{
				reskrneki=ConvertionResult;
				reskrneki4 = SDADC2_sample;
				last_IL_MEASURED = IL1_NORMAL;
//				if(count2==1) krneki2[SDADC1_CH1s.sample_count]=SDADC2_sample;
//				else if(count2==2) krneki2[SDADC1_CH1s.sample_count+1110]=SDADC2_sample;
				//iskanje najvecje vrednosti za automatic gain select
				if(ConvertionResult > SDADC2_CH1s.max) SDADC2_CH1s.max = ConvertionResult;
				//--------------------------------------FILTRI--------------------------------
				if(SDADC2_high_pass_filter==_ON)
				{
					SDADC2_sample2=SDADC2_sample;
					arm_biquad_cascade_df1_f32(&INST_IL1_HPF, &SDADC2_sample2, &SDADC2_sample,1);
				}
				if(SDADC2_low_pass_filter==_ON)
				{
					SDADC2_sample2=SDADC2_sample;
					arm_biquad_cascade_df1_f32(&INST_IL1_LPF, &SDADC2_sample2, &SDADC2_sample,1);
				}
				if(IL_Hanning_window==_ON)
				{	
					Hanning_coef = 0.5f*(1.0f-arm_cos_f32(2.0f*3.14159f*SDADC2_CH1s.sample_count*sdadc2_il));// * arm_cos_f32((2.0f*M_PI*(counta))/(current_sample_number-1)));
					SDADC2_sample=SDADC2_sample*Hanning_coef;
				}
			}
			break;
		}
		case IL2_CHANNEL: 
		{
			SDADC2_sample = ConvertionResult*il2_temp; 
			krneki[SDADC2_CH2s.sample_count+(SDADC2_IL_SAMPLE_CNT*u)]=ConvertionResult;
			#if	THD_COMPUTATION_METHOD== CORELATION
			//naslednja koda namenjena za pridobivanje podatkov pri racunanju THD-ja s korelacijo
			if((THD_COMPUTATION_METHOD==CORELATION)&&(meas_control & __THD_MEASURING))
			{
				if((!(meas_control & __IL_THD_MEASURED))&&(compute_control & __IL2_THD_SAMPLING)&&(current_THD_sampling==__IL2_THD_SAMPLING))
				{
					if(thd_counter2>=5)//vzamemo vsak peti vzorec, ker je frekvenca 50	KHz
					{
						#if THD_FFT_WINDOW == __FLAT_TOP
						SDADC2_sample2 = SDADC2_sample*(1.0f-1.93f * arm_cos_f32(__2PI*thd_counter*nminus1inv)+1.29f * arm_cos_f32(__4PI*thd_counter*nminus1inv)-0.388f * arm_cos_f32(__6PI*thd_counter*nminus1inv)+0.028f * arm_cos_f32(__8PI*thd_counter*nminus1inv));
						#elif THD_FFT_WINDOW == __HANN
						SDADC2_sample2 = SDADC2_sample*(0.5f-0.5f*arm_cos_f32(__2PI*thd_counter*nminus1inv));
						#elif THD_FFT_WINDOW == __HAMMING
						SDADC2_sample2 = SDADC2_sample*(0.54f-0.46f*arm_cos_f32(__2PI*thd_counter*nminus1inv));
						#else 
						SDADC2_sample2 = SDADC2_sample;
						#endif
						thd_counter2=0;
						reskrneki8	=	ConvertionResult;
//						if(count==1) krneki[thd_counter]=SDADC2_sample;
//						else if(count==2) krneki[thd_counter+1000]=SDADC2_sample;
						THD_buffer1[thd_counter]=SDADC2_sample2;
						if(thd_counter>=CORELATION_SAMPLE_NUMBER)
						{
							compute_control |= __IL2_THD_SAMPLED;
							last_THD_MEASURED= IL2_THD;
							compute_control &= (~__IL2_THD_SAMPLING);
							thd_counter=0;
							meas_control |= __IL_THD_MEASURED;
							set_event(COMPUTE_THD_WITH_CORELATION,compute_THD_with_corelation);
							if(count==2){
								count=1;
							}
							else count++;
						}
						else thd_counter++;
					}
					else thd_counter2++;
				}
			}
			#else
			if((compute_control & __IL2_THD_SAMPLING)&&(!(meas_control &__IL_THD_MEASURED))&&(current_THD_sampling==__IL2_THD_SAMPLING)&&(meas_control & __THD_MEASURING))
			{
				#if THD_FFT_WINDOW == __FLAT_TOP
				SDADC2_sample2 = SDADC2_sample*(1.0f-1.93f * arm_cos_f32(__2PI*thd_counter*nminus1inv)+1.29f * arm_cos_f32(__4PI*thd_counter*nminus1inv)-0.388f * arm_cos_f32(__6PI*thd_counter*nminus1inv)+0.028f * arm_cos_f32(__8PI*thd_counter*nminus1inv));
				#elif THD_FFT_WINDOW == __HANN
				SDADC2_sample2 = SDADC2_sample*(0.5f-0.5f*arm_cos_f32(__2PI*thd_counter*nminus1inv));
				#elif THD_FFT_WINDOW == __HAMMING
				SDADC2_sample2 = SDADC2_sample*(0.54f-0.46f*arm_cos_f32(__2PI*thd_counter*nminus1inv));
				#else 
				SDADC2_sample2 = SDADC2_sample;
				#endif
				THD_buffer1[thd_counter*2]=SDADC2_sample2;
				THD_buffer1[(thd_counter*2)+1]=0;
				if(thd_counter>=FFT_SAMPLE_NUMBER)
				{
					compute_control |= __IL2_THD_SAMPLED;
					compute_control &= (~__IL2_THD_SAMPLING);
					last_THD_MEASURED=IL2_THD;
					thd_counter=0;
					meas_control |= __IL_THD_MEASURED;	//zastavica je enotna za vse IL faze, zato ker se meri samo ena faza naenkrat in to zadostuje
					set_event(COMPUTE_THD_WITH_FFT,compute_THD_with_FFT);
				}
				else thd_counter++;
			}
			#endif
			//sledeci pogoj pomemben zarad filtrov
			//ce je vklopljena THD meritev se ne izvrsi, razn ce racunamo THD z FFT
			//if(((!(meas_control & __IL2_MEASURED))||(meas_control & __NO_THD_MEAS))&&((!(meas_control & __THD_MEASURING))||(THD_COMPUTATION_METHOD==FFT)))
			if(((!(meas_control & __IL2_MEASURED))&&(!(meas_control & __THD_MEASURING)))||(THD_COMPUTATION_METHOD==FFT)||(meas_control & __NO_THD_MEAS))
			{
				reskrneki2=ConvertionResult;
				reskrneki5 = SDADC2_sample;
				last_IL_MEASURED = IL2_NORMAL;
				if(ConvertionResult > SDADC2_CH2s.max) SDADC2_CH2s.max = ConvertionResult;
				//--------------------------------------FILTRI--------------------------------
				if(SDADC2_high_pass_filter==_ON)
				{
					SDADC2_sample2=SDADC2_sample;
					arm_biquad_cascade_df1_f32(&INST_IL2_HPF, &SDADC2_sample2, &SDADC2_sample,1);
				}
				if(SDADC2_low_pass_filter==_ON)
				{
					SDADC2_sample2=SDADC2_sample;
					arm_biquad_cascade_df1_f32(&INST_IL2_LPF, &SDADC2_sample2, &SDADC2_sample,1);
				}
				if(IL_Hanning_window==_ON)
				{	
					Hanning_coef = 0.5f*(1.0f-arm_cos_f32(2.0f*3.14159f*SDADC2_CH2s.sample_count*sdadc2_il));// * arm_cos_f32((2.0f*M_PI*(counta))/(current_sample_number-1)));
					SDADC2_sample=SDADC2_sample*Hanning_coef;
				}
			}
			break;
		}
		case IL3_CHANNEL: 
		{
			SDADC2_sample = ConvertionResult*il3_temp;
			#if THD_COMPUTATION_METHOD == CORELATION
			if((THD_COMPUTATION_METHOD==CORELATION)&&(meas_control & __THD_MEASURING))
			{
				if((!(meas_control & __IL_THD_MEASURED))&&(compute_control & __IL3_THD_SAMPLING)&&(current_THD_sampling==__IL3_THD_SAMPLING))
				{
					if(thd_counter2>=5)//vzamemo vsak peti vzorec, ker je frekvenca 50	KHz
					{
						#if THD_FFT_WINDOW == __FLAT_TOP
						SDADC2_sample2 = SDADC2_sample*(1.0f-1.93f * arm_cos_f32(__2PI*thd_counter*nminus1inv)+1.29f * arm_cos_f32(__4PI*thd_counter*nminus1inv)-0.388f * arm_cos_f32(__6PI*thd_counter*nminus1inv)+0.028f * arm_cos_f32(__8PI*thd_counter*nminus1inv));
						#elif THD_FFT_WINDOW == __HANN
						SDADC2_sample2 = SDADC2_sample*(0.5f-0.5f*arm_cos_f32(__2PI*thd_counter*nminus1inv));
						#elif THD_FFT_WINDOW == __HAMMING
						SDADC2_sample2 = SDADC2_sample*(0.54f-0.46f*arm_cos_f32(__2PI*thd_counter*nminus1inv));
						#else 
						SDADC2_sample2 = SDADC2_sample;
						#endif
						thd_counter2=0;
						reskrneki9	=	ConvertionResult;
						THD_buffer1[thd_counter]=SDADC2_sample2;
						if(thd_counter>=CORELATION_SAMPLE_NUMBER)
						{
							compute_control |= __IL3_THD_SAMPLED;
							last_THD_MEASURED = IL3_THD;
							compute_control &= (~__IL3_THD_SAMPLING);
							thd_counter=0;
							meas_control |= __IL_THD_MEASURED;
							set_event(COMPUTE_THD_WITH_CORELATION,compute_THD_with_corelation);
						}
						else thd_counter++;
					}
					else thd_counter2++;
				}
			}
			#else
			if((compute_control & __IL3_THD_SAMPLING)&&(!(meas_control &__IL_THD_MEASURED))&&(current_THD_sampling==__IL3_THD_SAMPLING)&&(meas_control & __THD_MEASURING))
			{
				#if THD_FFT_WINDOW == __FLAT_TOP
				SDADC2_sample2 = SDADC2_sample*(1.0f-1.93f * arm_cos_f32(__2PI*thd_counter*nminus1inv)+1.29f * arm_cos_f32(__4PI*thd_counter*nminus1inv)-0.388f * arm_cos_f32(__6PI*thd_counter*nminus1inv)+0.028f * arm_cos_f32(__8PI*thd_counter*nminus1inv));
				#elif THD_FFT_WINDOW == __HANN
				SDADC2_sample2 = SDADC2_sample*(0.5f-0.5f*arm_cos_f32(__2PI*thd_counter*nminus1inv));
				#elif THD_FFT_WINDOW == __HAMMING
				SDADC2_sample2 = SDADC2_sample*(0.54f-0.46f*arm_cos_f32(__2PI*thd_counter*nminus1inv));
				#else 
				SDADC2_sample2 = SDADC2_sample;
				#endif
				THD_buffer1[thd_counter*2]=SDADC2_sample2;
				THD_buffer1[(thd_counter*2)+1]=0;
				if(thd_counter>=FFT_SAMPLE_NUMBER)
				{
					compute_control |= __IL3_THD_SAMPLED;
					compute_control &= (~__IL3_THD_SAMPLING);
					last_THD_MEASURED=IL3_THD;
					thd_counter=0;
					meas_control |= __IL_THD_MEASURED;	//zastavica je enotna za vse IL faze, zato ker se meri samo ena faza naenkrat in to zadostuje
					set_event(COMPUTE_THD_WITH_FFT,compute_THD_with_FFT);
				}
				else thd_counter++;
			}
			#endif
			//sledeci pogoj pomemben zarad filtrov
			//if(((!(meas_control & __IL3_MEASURED))||(meas_control & __NO_THD_MEAS))&&((!(meas_control & __THD_MEASURING))||(THD_COMPUTATION_METHOD==FFT)))
			if(((!(meas_control & __IL3_MEASURED))&&(!(meas_control & __THD_MEASURING)))||(THD_COMPUTATION_METHOD==FFT)||(meas_control & __NO_THD_MEAS))
			{
				reskrneki3=ConvertionResult;
				reskrneki6 = SDADC2_sample;
				last_IL_MEASURED = IL3_NORMAL;
				if(ConvertionResult > SDADC2_CH3s.max) SDADC2_CH3s.max = ConvertionResult;
				//--------------------------------------FILTRI--------------------------------
				if(SDADC2_high_pass_filter==_ON)
				{
					SDADC2_sample2=SDADC2_sample;
					arm_biquad_cascade_df1_f32(&INST_IL3_HPF, &SDADC2_sample2, &SDADC2_sample,1);
				}
				if(SDADC2_low_pass_filter==_ON)
				{
					SDADC2_sample2=SDADC2_sample;
					arm_biquad_cascade_df1_f32(&INST_IL3_LPF, &SDADC2_sample2, &SDADC2_sample,1);
				}
				if(IL_Hanning_window==_ON)
				{	
					Hanning_coef = 0.5f*(1.0f-arm_cos_f32(2.0f*3.14159f*SDADC2_CH3s.sample_count * sdadc2_il));// * arm_cos_f32((2.0f*M_PI*(counta))/(current_sample_number-1)));
					SDADC2_sample=SDADC2_sample*Hanning_coef;
				}
			}
			break;
		}
		default: break;
	}
	//sledeca koda se izvaja v primeru da ni THD meritev
	if((!(meas_control & __THD_MEASURING))||(THD_COMPUTATION_METHOD==FFT))
	{
		switch(channel)
		{
			case IL1_CHANNEL: 
			{
				if((!(meas_control & __IL1_MEASURED))||(meas_control & __NO_THD_MEAS)||(THD_COMPUTATION_METHOD==FFT))
				{
					SDADC2_CH1s.sempl=SDADC2_sample;
					SDADC2_CH1s.sum += (SDADC2_CH1s.sempl*SDADC2_CH1s.sempl);
					if(compute_control2 & __POWER_MEAS_ON)
					{
						//trenutna delovna moc	(SDADC1_CH1.sempl se pomeri pred tem)
						PHASE1.real_power_sum+=SDADC2_CH1s.sempl*SDADC1_CH1s.sempl;
					}
					if(SDADC2_CH1s.sample_count>=SDADC2_IL_SAMPLE_CNT)
					{
						//ce se nismo zracunal prejsne napetosti je ne prepise ampak zacne znova dokler napetost ni izracunana
						if(!(compute_control & __IL1_SAMPLED))IL1.avarage_sq = SDADC2_CH1s.sum/(SDADC1_CH1s.sample_count+1);
						if(compute_control2 & __POWER_MEAS_ON) 
						{
							PHASE1.real_power = PHASE1.real_power_sum/(SDADC2_CH1s.sample_count+1);
							PHASE1.real_power_sum=0;
						}
						SDADC2_CH1s.sample_count = 0;
						SDADC2_CH1s.sum = 0;
						IL_auto_gain(IL1_CHANNEL);
						compute_control |= __IL1_SAMPLED;	//postavitev zastavice za izracun efektivne vrednosti
						meas_control |= __IL1_MEASURED;
						if(count2==2){
							count2=1;
						}
						else count2++;
					}
					else SDADC2_CH1s.sample_count++;
				}
				break;
			}
			case IL2_CHANNEL: 
			{
				if((!(meas_control & __IL2_MEASURED))||(meas_control & __NO_THD_MEAS)||(THD_COMPUTATION_METHOD==FFT))
				{
					SDADC2_CH2s.sempl=SDADC2_sample;
					SDADC2_CH2s.sum += (SDADC2_CH2s.sempl*SDADC2_CH2s.sempl);
					if(compute_control2 & __POWER_MEAS_ON)
					{
						//trenutna delovna moc	(SDADC1_CH1.sempl se pomeri pred tem)
						PHASE2.real_power_sum+=SDADC2_CH2s.sempl*SDADC1_CH2s.sempl;
					}
					if(SDADC2_CH2s.sample_count>=SDADC2_IL_SAMPLE_CNT)
					{
						//ce se nismo zracunal prejsne napetosti je ne prepise ampak zacne znova dokler napetost ni izracunana
						if(!(compute_control & __IL2_SAMPLED))IL2.avarage_sq = SDADC2_CH2s.sum/(SDADC2_CH2s.sample_count+1);
						if(compute_control2 & __POWER_MEAS_ON) 
						{
							PHASE2.real_power = PHASE2.real_power_sum/(SDADC2_CH2s.sample_count+1);
							PHASE2.real_power_sum=0;
						}
						SDADC2_CH2s.sample_count = 0;
						SDADC2_CH2s.sum = 0;
						IL_auto_gain(IL2_CHANNEL);
						compute_control |= __IL2_SAMPLED;	//postavitev zastavice za izracun efektivne vrednosti
						meas_control |= __IL2_MEASURED;
						if(u>=1) u=0;
						else u++;
					}
					else SDADC2_CH2s.sample_count++;
				}
				break;
			}
			case IL3_CHANNEL: 
			{
				if((!(meas_control & __IL3_MEASURED))||(meas_control & __NO_THD_MEAS)||(THD_COMPUTATION_METHOD==FFT))
				{
					SDADC2_CH3s.sempl=SDADC2_sample;
					SDADC2_CH3s.sum += (SDADC2_CH3s.sempl*SDADC2_CH3s.sempl);
					if(compute_control2 & __POWER_MEAS_ON)
					{
						//trenutna delovna moc	(SDADC1_CH1.sempl se pomeri pred tem)
						PHASE3.real_power_sum+=SDADC2_CH3s.sempl*SDADC1_CH3s.sempl;
					}
					if(SDADC2_CH3s.sample_count>=SDADC2_IL_SAMPLE_CNT)
					{
						//ce se nismo zracunal prejsne napetosti je ne prepise ampak zacne znova dokler napetost ni izracunana
						if(!(compute_control & __IL3_SAMPLED))IL3.avarage_sq = SDADC2_CH3s.sum/(SDADC2_CH3s.sample_count+1);
						if(compute_control2 & __POWER_MEAS_ON) 
						{
							PHASE3.real_power = PHASE3.real_power_sum/(SDADC2_CH3s.sample_count+1);
							PHASE3.real_power_sum=0;
						}
						SDADC2_CH3s.sample_count = 0;
						SDADC2_CH3s.sum = 0;
						IL_auto_gain(IL3_CHANNEL);
						compute_control |= __IL3_SAMPLED;	//postavitev zastavice za izracun efektivne vrednosti
						meas_control |= __IL3_MEASURED;
					}
					else SDADC2_CH3s.sample_count++;
				}
				break;
			}
			default: break;
		}	
	}
	//if((meas_control & __IL1_MEASURED)&&(meas_control & __IL2_MEASURED)&&(meas_control & __IL3_MEASURED)) test1_tog;
}
static void IL_auto_gain(uint32_t channel)
{
	char i;
	//narejena histereza za preklop ojacenja
	switch(channel)
	{
		case IL1_CHANNEL: 
		{
			if(IL1_GAIN ==__GAIN1)
			{
				if(SDADC2_CH1s.max<=IL_AUTO_GAIN_TD) 
				{
					IL_setGain(IL1_CHANNEL, __GAIN40);
					//ponastavim filtre
					for(i=0;i<4;i++) 
					{
						IL1_IIR_state[i]=0;
						IL1_IIR_LOW_state[i]=0;
					}
				}
			}
			else if(IL1_GAIN ==__GAIN40)
			{
				if(SDADC2_CH1s.max>=IL_AUTO_GAIN_TU) 
				{
					IL_setGain(IL1_CHANNEL, __GAIN1);
					for(i=0;i<4;i++) 
					{
						IL1_IIR_state[i]=0;
						IL1_IIR_LOW_state[i]=0;
					}
				}
			}
			SDADC2_CH1s.max=0;
			break;
		}
		case IL2_CHANNEL: 
		{
			if(IL2_GAIN ==__GAIN1)
			{
				if(SDADC2_CH2s.max<=IL_AUTO_GAIN_TD) 
				{
					IL_setGain(IL2_CHANNEL, __GAIN40);
					for(i=0;i<4;i++) 
					{
						IL2_IIR_state[i]=0;
						IL2_IIR_LOW_state[i]=0;
					}
				}
			}
			else if(IL2_GAIN ==__GAIN40)
			{
				if(SDADC2_CH2s.max>=IL_AUTO_GAIN_TU) 
				{
				IL_setGain(IL2_CHANNEL, __GAIN1);
				//ponastavim filtre
				for(i=0;i<4;i++) 
				{
					IL2_IIR_state[i]=0;
					IL2_IIR_LOW_state[i]=0;
				}
				}
			}
			SDADC2_CH2s.max=0;
			break;
		}
		case IL3_CHANNEL: 
		{
			if(IL3_GAIN ==__GAIN1)
			{
				if(SDADC2_CH3s.max<=IL_AUTO_GAIN_TD) 
				{
					IL_setGain(IL3_CHANNEL, __GAIN40);
					for(i=0;i<4;i++) 
					{
						IL3_IIR_state[i]=0;
						IL3_IIR_LOW_state[i]=0;
					}
				}
			}
			else if(IL3_GAIN ==__GAIN40)
			{
				if(SDADC2_CH3s.max>=IL_AUTO_GAIN_TU) 
				{
					IL_setGain(IL3_CHANNEL, __GAIN1);
					//ponastavim filtre
					for(i=0;i<4;i++) 
					{
						IL3_IIR_state[i]=0;
						IL3_IIR_LOW_state[i]=0;
					}
				}
			}
			SDADC2_CH3s.max=0;
			break;
		}
		default:break;
	}
}
void compute_rms(void)
{
	//funkcija za racunannje rms vrednosti, ker sqrt operacija uzame prevec casa za
	//izvajanje v interruptu
	if(compute_control & __ULN1_SAMPLED)	
	{
		arm_sqrt_f32(ULN1.avarage_sq, &ULN1.effective);
		compute_control |= __ULN1_EFF_COMPUTED;
		compute_control&=(~__ULN1_SAMPLED);
	}
	if(compute_control & __ULN2_SAMPLED)	
	{
		arm_sqrt_f32(ULN2.avarage_sq, &ULN2.effective);
		compute_control2 |= __ULN2_EFF_COMPUTED;
		compute_control&=(~__ULN2_SAMPLED);
	}
	if(compute_control & __ULN3_SAMPLED)	
	{
		arm_sqrt_f32(ULN3.avarage_sq, &ULN3.effective);	
		compute_control2 |= __ULN3_EFF_COMPUTED;		
		compute_control&=(~__ULN3_SAMPLED);
	}
	if(compute_control & __IL1_SAMPLED)	
	{
		arm_sqrt_f32(IL1.avarage_sq, &IL1.effective);
		compute_control |= __IL1_EFF_COMPUTED;
		compute_control&=(~__IL1_SAMPLED);
	}
	if(compute_control & __IL2_SAMPLED)	
	{
		arm_sqrt_f32(IL2.avarage_sq, &IL2.effective);
		compute_control |= __IL2_EFF_COMPUTED;
		compute_control&=(~__IL2_SAMPLED);
	}
	if(compute_control & __IL3_SAMPLED)	
	{
		arm_sqrt_f32(IL3.avarage_sq, &IL3.effective);
		compute_control |= __IL3_EFF_COMPUTED;
		compute_control&=(~__IL3_SAMPLED);
	}
	if(compute_control & __IDIFF_SAMPLED)	
	{
		arm_sqrt_f32(IDIFF.avarage_sq, &IDIFF.effective);
		//korekcija zaradi posabeznih vplivov fazpotrebno umeriti z vsako fazo posebej
//		to ne bo pil vode!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//		temp1=IL1.effective*IDIFF_K1+IDIFF_N1;
//		temp2=IL2.effective*IDIFF_K2+IDIFF_N2;
//		temp3=IL3.effective*IDIFF_K3+IDIFF_N3;
//		arm_mult_f32(&temp1, &temp1, &temp1_2, 1);		//izracun kvadrata
//		arm_mult_f32(&temp2, &temp2, &temp2_2, 1);
//		arm_mult_f32(&temp3, &temp3, &temp3_2, 1);
//		arm_sqrt_f32((temp1_2-(2*temp1*COS60*temp2+temp2_2)), &Ix);
//		cos=arm_cos_f32(((temp1-COS60*temp2)/Ix));
//		arccos=1/cos;
//		cos=arm_cos_f32(arccos-60);
//		arm_sqrt_f32((temp3_2-2*temp3*cos+temp2_2-2*temp2*COS60*temp1+temp1_2), &napaka);
		
//		IDIFF.effective=IDIFF.effective-napaka;
		//----------------------------------------------------------------------------
//		if(IDIFF.effective<=1.0f)
//		{
//			IDIFF.effective=(IDIFF.effective*IDIFF_Klow+IDIFF_Nlow);
//		}
//		else
//		{
//			IDIFF.effective=(IDIFF.effective*IDIFF_Khigh+IDIFF_Nhigh);
//		}
		compute_control&=(~__IDIFF_SAMPLED);
	}
	if(compute_control & __UL1PE_SAMPLED)	
	{
		arm_sqrt_f32(UL1PE.avarage_sq, &UL1PE.effective);
		compute_control&=(~__UL1PE_SAMPLED);
	}
	if(compute_control & __UNPE_SAMPLED)	
	{
		arm_sqrt_f32(UNPE.avarage_sq, &UNPE.effective);
		compute_control&=(~__UNPE_SAMPLED);
	}
	//racunanje navidezne moci in faznega kota
	if(compute_control2 & __POWER_MEAS_ON)
	{
		if((compute_control & __IL1_EFF_COMPUTED) && (compute_control & __ULN1_EFF_COMPUTED))
		{
			PHASE1.apparent_power = ULN1.effective*IL1.effective;
			PHASE1.PF = PHASE1.real_power/PHASE1.apparent_power;
			compute_control &= (~__IL1_EFF_COMPUTED);
			compute_control &= (~__ULN1_EFF_COMPUTED);
		}
		if((compute_control & __IL2_EFF_COMPUTED) && (compute_control2 & __ULN2_EFF_COMPUTED))
		{
			PHASE2.apparent_power = ULN2.effective*IL2.effective;
			PHASE2.PF = PHASE2.real_power/PHASE2.apparent_power;
			compute_control &= (~__IL2_EFF_COMPUTED);
			compute_control2 &= (~__ULN2_EFF_COMPUTED);
		}
		if((compute_control & __IL3_EFF_COMPUTED) && (compute_control2 & __ULN3_EFF_COMPUTED))
		{
			PHASE3.apparent_power = ULN3.effective*IL3.effective;
			PHASE3.PF = PHASE3.real_power/PHASE3.apparent_power;
			compute_control &= (~__IL3_EFF_COMPUTED);
			compute_control2 &= (~__ULN3_EFF_COMPUTED);
		}
	}
	//ce so fazna zaporedja izmerjena
	if((compute_control2 & __GET_PHASES)&&(compute_control2 & __PHASE1_MEASURED)&&(compute_control2 & __PHASE2_MEASURED)&&(compute_control2 & __PHASE3_MEASURED))
	{
		//na zacetku pobrisemo zastavice, zato da jih lahko ponovno postavimo
		global_control &= (~(__PHASE_SEQ_132|__PHASE_SEQ_UNKNOWN|__PHASE_SEQ_INPHASE|__PHASE_SEQ_123|__PHASE_SEQ_1P_SYS|__PHASE_MISSING));
		compute_control2 &= (~(__IN_PHASE_1_2|__IN_PHASE_2_3|__IN_PHASE_1_3));
		//ce na ULN2 in ULN3 ni napetosti lahko takoj recemo da gre za enofazni sistem in ne gledamo ostalih zaporedij
		//ce so prisotne napetosti se o faznem sistemu odlocimo kasnjeje
		if((SDADC1_CH2s.max_float<190.0f)&&(SDADC1_CH3s.max_float<190.0f))
			global_control |= __PHASE_SEQ_1P_SYS;
		//ce manjka samo ena faza
		else if((SDADC1_CH2s.max_float<190.f)||(SDADC1_CH3s.max_float<190.f))
			global_control |= __PHASE_MISSING;
		else
		{
			//faza 1 v fazi s fazo 2
			if((SDADC1_CH1s.peak_at < (SDADC1_CH2s.peak_at+PHASE_UNCERTANTY))&&(SDADC1_CH1s.peak_at > (SDADC1_CH2s.peak_at-PHASE_UNCERTANTY)))
			{
				//faza 2 v fazi s fazo 3
				if((SDADC1_CH2s.peak_at < (SDADC1_CH3s.peak_at+PHASE_UNCERTANTY))&&(SDADC1_CH2s.peak_at > (SDADC1_CH3s.peak_at-PHASE_UNCERTANTY)))
					global_control |= __PHASE_SEQ_INPHASE;
				else
				{compute_control2 |= __IN_PHASE_1_2; global_control |= __PHASE_SEQ_UNKNOWN;}
			}
			//faza 1 v fazi s fazo 3
			else if((SDADC1_CH1s.peak_at < (SDADC1_CH3s.peak_at+PHASE_UNCERTANTY))&&(SDADC1_CH1s.peak_at > (SDADC1_CH3s.peak_at-PHASE_UNCERTANTY)))
			{compute_control2 |= __IN_PHASE_1_3; global_control |= __PHASE_SEQ_UNKNOWN;}
				//faza 2 v fazi s fazo 3
			else if((SDADC1_CH2s.peak_at < (SDADC1_CH3s.peak_at+PHASE_UNCERTANTY))&&(SDADC1_CH2s.peak_at > (SDADC1_CH3s.peak_at-PHASE_UNCERTANTY)))
			{compute_control2 |= __IN_PHASE_2_3; global_control |= __PHASE_SEQ_UNKNOWN;}
			else 
				global_control |= __PHASE_SEQ_UNKNOWN;
			//ce zaporedje ni v fazi odcitamo kaksno je
			if(!(global_control & __PHASE_SEQ_INPHASE))
			{
				if((SDADC1_CH1s.peak_at <	SDADC1_CH2s.peak_at)&&(SDADC1_CH2s.peak_at<	SDADC1_CH3s.peak_at))
				{
					global_control |= __PHASE_SEQ_123;
				}
				else if((SDADC1_CH1s.peak_at <	SDADC1_CH3s.peak_at)&&(SDADC1_CH3s.peak_at<	SDADC1_CH2s.peak_at))
				{
					global_control |= __PHASE_SEQ_132;
				}
				else global_control |= __PHASE_SEQ_UNKNOWN;
			}
		}
		//ponastaimo vrednosti da je pripravljeno za naslednje merjenje
		SDADC1_CH1s.max_float=0;
		SDADC1_CH2s.max_float=0;
		SDADC1_CH3s.max_float=0;
		SDADC1_CH1s.peak_at=0;
		SDADC1_CH2s.peak_at=0;
		SDADC1_CH3s.peak_at=0; //da dobimo rezultat potrebujemo cca 250ms
		HAL_TIM_Base_Stop(&htim6);
		compute_control2 &= (~(__GET_PHASES|__PHASE1_MEASURED|__PHASE2_MEASURED|__PHASE3_MEASURED));
	}
	if(global_control & __ON_TEST_IN_PROG) 
	{
//		if(count>=THROW_AWAY_RESULTS)
//		{
			stop_measure();
			compute_control2 |= __INIT_MEASURED;
//			count=0;
//		}
//		else count++;
	}


}
static void measure_IDIFF(int16_t ConvertionResult)
{
	float SDADC3_sample;
	float SDADC3_sample2;
	IDIFF.correction = 1;
	float idiff1_temp;
	idiff1_temp = IDIFF.correction*IDIFF_GAIN_1;
	float32_t temp1, temp2, temp3;

	SDADC3_sample = ConvertionResult*idiff1_temp;// - 32768;//pretvorba v 1k

	//--------------------------------------
	if(((!(meas_control & __IDIFF_MEASURED))||(meas_control & __NO_THD_MEAS))&&(!(meas_control & __THD_MEASURING)))
	{
		if(IDIFF_high_pass_filter==_ON)
		{
			SDADC3_sample2=SDADC3_sample;
			arm_biquad_cascade_df1_f32(&INST_IDIFF_HPF, &SDADC3_sample2, &SDADC3_sample,1);
		}
		if(IDIFF_low_pass_filter==_ON)
		{
			SDADC3_sample2=SDADC3_sample;
			arm_biquad_cascade_df1_f32(&INST_IDIFF_LPF, &SDADC3_sample2, &SDADC3_sample,1);
		}
	}
	
//	k_IL2 = (SDADC2_CH2s.sempl-IL2_prev)*2777.777f;
//	k_IL3 = (SDADC2_CH3s.sempl-IL3_prev)*5555.555f;
	
	//korekcija IDIFF
	temp1 = SDADC2_CH1s.sempl*IDIFF_K1*(1.0f)+IDIFF_N1;//linearna funkcija napake enefaze -1 ker je tok na izhodu zamaknjen za 180 stopinj 
	temp2 = SDADC2_CH2s.sempl*IDIFF_K2*(1.0f)+IDIFF_N2;//linearna funkcija napake enefaze -1 ker je tok na izhodu zamaknjen za 180 stopinj
	temp3 = SDADC2_CH3s.sempl*IDIFF_K3*(1.0f)+IDIFF_N3;//linearna funkcija napake enefaze -1 ker je tok na izhodu zamaknjen za 180 stopinj
////	temp1 = ((IL1.effective*IDIFF_K1*(1.0f))* (IL1.effective/SDADC2_CH1s.sempl))+IDIFF_N1;//linearna funkcija napake enefaze -1 ker je tok na izhodu zamaknjen za 180 stopinj 
////	temp2 = ((IL2.effective*IDIFF_K2*(1.0f))* (IL2.effective/SDADC2_CH2s.sempl))+IDIFF_N2;//linearna funkcija napake enefaze -1 ker je tok na izhodu zamaknjen za 180 stopinj
////	temp3 = ((IL3.effective*IDIFF_K3*(1.0f))* (IL3.effective/SDADC2_CH3s.sempl))+IDIFF_N3;//linearna funkcija napake enefaze -1 ker je tok na izhodu zamaknjen za 180 stopinj
	SDADC3_CH1s.sempl=SDADC3_sample-(temp1+temp2+temp3);
	//korekcija z umerjanjem

	//SDADC3_CH1s.sempl=(SDADC3_CH1s.sempl*IDIFF_K+IDIFF_N);
//	if(SDADC3_CH1s.sempl<=1.0f)
//	{
//		SDADC3_CH1s.sempl=(SDADC3_CH1s.sempl*IDIFF_Klow+IDIFF_Nlow);
//	}
//	else
//	{
//		SDADC3_CH1s.sempl=(SDADC3_CH1s.sempl*IDIFF_Khigh+IDIFF_Nhigh);
//	}
	if((!(meas_control & __IDIFF_MEASURED))||(meas_control & __NO_THD_MEAS))
	{
		SDADC3_CH1s.sum += (SDADC3_CH1s.sempl*SDADC3_CH1s.sempl);
		if(SDADC3_CH1s.sample_count>=SDADC3_IDIFF_SAMPLE_CNT)
		{
			if(!(compute_control & __IDIFF_SAMPLED))IDIFF.avarage_sq = SDADC3_CH1s.sum/SDADC3_CH1s.sample_count;
			SDADC3_CH1s.sample_count = 0;
			SDADC3_CH1s.sum = 0;
			compute_control |= __IDIFF_SAMPLED;	//postavitev zastavice za izracun efektivne vrednosti
			meas_control |= __IDIFF_MEASURED;
		}
		else SDADC3_CH1s.sample_count++;
	}
}
static void measure_UL1PE(int16_t ConvertionResult)
{
	float SDADC3_sample;
	float SDADC3_sample2;
	UL1PE.correction = 1;
	float ul1pe_temp;	
	ul1pe_temp = UL1PE.correction*UL1PE_GAIN1;

	

	SDADC3_sample = ConvertionResult*ul1pe_temp;// - 32768;//pretvorba v 1k
	//--------------------------------------
	if(UL1PE_high_pass_filter==_ON)
	{
		SDADC3_sample2=SDADC3_sample;
		arm_biquad_cascade_df1_f32(&INST_UL1PE_HPF, &SDADC3_sample2, &SDADC3_sample,1);
	}
	if(UL1PE_low_pass_filter==_ON)
	{
		SDADC3_sample2=SDADC3_sample;
		arm_biquad_cascade_df1_f32(&INST_UL1PE_LPF, &SDADC3_sample2, &SDADC3_sample,1);
	}

	SDADC3_CH2s.sempl=SDADC3_sample;
	SDADC3_CH2s.sum += (SDADC3_CH2s.sempl*SDADC3_CH2s.sempl);
	if(SDADC3_CH2s.sample_count>=SDADC3_UL1PE_SAMPLE_CNT)
	{
		if(!(compute_control & __UL1PE_SAMPLED))UL1PE.avarage_sq = SDADC3_CH2s.sum/SDADC3_CH2s.sample_count;
		SDADC3_CH2s.sample_count = 0;
		SDADC3_CH2s.sum = 0;
		compute_control |= __UL1PE_SAMPLED;	//postavitev zastavice za izracun efektivne vrednosti
		meas_control |= __UL1PE_MEASURED;
	}
	else SDADC3_CH2s.sample_count++;
}
static void measure_UNPE(int16_t ConvertionResult)
{
	float SDADC3_sample;
	float SDADC3_sample2;
	UNPE.correction = 1;
	float unpe_temp;	
	unpe_temp = UNPE.correction*UNPE_GAIN1;
	SDADC3_sample = ConvertionResult*unpe_temp;// - 32768;//pretvorba v 1k
	//--------------------------------------
	if(UNPE_high_pass_filter==_ON)
	{
		SDADC3_sample2=SDADC3_sample;
		arm_biquad_cascade_df1_f32(&INST_UNPE_HPF, &SDADC3_sample2, &SDADC3_sample,1);
	}
	if(UNPE_low_pass_filter==_ON)
	{
		SDADC3_sample2=SDADC3_sample;
		arm_biquad_cascade_df1_f32(&INST_UNPE_LPF, &SDADC3_sample2, &SDADC3_sample,1);
	}

	SDADC3_CH3s.sempl=SDADC3_sample;
	SDADC3_CH3s.sum += (SDADC3_CH3s.sempl*SDADC3_CH3s.sempl);
	if(SDADC3_CH3s.sample_count>=SDADC3_UNPE_SAMPLE_CNT)
	{
		if(!(compute_control & __UNPE_SAMPLED))UNPE.avarage_sq = SDADC3_CH3s.sum/SDADC3_CH3s.sample_count;
		SDADC3_CH3s.sample_count = 0;
		SDADC3_CH3s.sum = 0;
		compute_control |= __UNPE_SAMPLED;			//postavitev zastavice za izracun efektivne vrednosti
		meas_control |= __UNPE_MEASURED;
	}
	else SDADC3_CH3s.sample_count++;
}
void input_START_measure(void)
	{
//		postavi zastavice za vklop sdadc-jev, ki se vklopijo ob naslednjem interruptu sinhrota
		meas_control |= __SDADC1_START_MASK;
		meas_control |= __SDADC2_START_MASK;
		//med samo meritvijo SDADC-ja 3 ne izklapljamo, zato ga ni potrebno ponovno zagnati
		if(!(meas_control & __MEAS_IN_PROGRESS))	meas_control |= __SDADC3_START_MASK;
		//zakasnitev, ki sprozi sdadc1 ob preckanju napetosti LN1 skozi 0
		
//		tole se ful cudno obnasa zato ne bom izklaplov pa vklaplov
//		HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);	//vklopi interrupt na sinhronizacijo
//		__DSB();
//		__ISB();
	}
void input_STOP_measure(void)
{
//izklopi sinhro interupt
//	HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
//	__DSB();
//	__ISB();
	
	HAL_SDADC_InjectedStop(&hsdadc2);
	HAL_SDADC_InjectedStop_IT(&hsdadc1);
	//med samo meritvijo SDADC-ja 3 ne smemo izklopit, ker to privede do napake pri ponovnem zagonu
	if(!(meas_control & __MEAS_IN_PROGRESS)) HAL_SDADC_InjectedStop_IT(&hsdadc3);
	
}
void input_START_THD_measure(void)
{
//		postavi zastavice za vklop sdadc-jev, ki se vklopijo ob naslednjem interruptu sinhrota
		meas_control |= __SDADC1_START_MASK;
		meas_control |= __SDADC2_START_MASK;
		//zakasnitev, ki sprozi sdadc1 ob preckanju napetosti LN1 skozi 0
//		HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);	//vklopi interrupt na sinhronizacijo
//		__DSB();
//		__ISB();
}
void input_STOP_THD_measure(void)
{
	HAL_SDADC_Stop_IT(&hsdadc1);
	HAL_SDADC_Stop_IT(&hsdadc2);
	//izklopi sinhro interupt
//	HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
//	__DSB();
//	__ISB();
}
//funkcija ponastavi vse spremeljivke za merjenje
void set_SDADC_zero(void)
{
	//char i;
	SDADC1_CH1s.sample_count = 0;
	SDADC1_CH2s.sample_count = 0;
	SDADC1_CH3s.sample_count = 0;
	SDADC2_CH1s.sample_count = 0;
	SDADC2_CH2s.sample_count = 0;
	SDADC2_CH3s.sample_count = 0;
	SDADC3_CH1s.sample_count = 0;
	SDADC3_CH2s.sample_count = 0;
	SDADC3_CH3s.sample_count = 0;
	SDADC1_CH1s.sum=0;
	SDADC1_CH2s.sum=0;
	SDADC1_CH3s.sum=0;
	SDADC2_CH1s.sum=0;
	SDADC2_CH2s.sum=0;
	SDADC2_CH3s.sum=0;
	SDADC3_CH1s.sum=0;
	SDADC3_CH2s.sum=0;
	SDADC3_CH3s.sum=0;
//	for(i=0;i<4;i++) 
//	{
//		IL1_IIR_state[i]=0;
//		IL1_IIR_LOW_state[i]=0;
//		IL2_IIR_state[i]=0;
//		IL2_IIR_LOW_state[i]=0;
//		IL3_IIR_state[i]=0;
//		IL3_IIR_LOW_state[i]=0;
//		ULN1_IIR_state[i]=0;
//		ULN1_IIR_LOW_state[i]=0;
//		ULN2_IIR_state[i]=0;
//		ULN2_IIR_LOW_state[i]=0;
//		ULN3_IIR_state[i]=0;
//		ULN3_IIR_LOW_state[i]=0;
//		IDIFF_IIR_state[i]=0;
//		IDIFF_IIR_LOW_state[i]=0;
//		UL1PE_IIR_state[i]=0;
//		UL1PE_IIR_LOW_state[i]=0;
//		UNPE_IIR_state[i]=0;
//		UNPE_IIR_LOW_state[i]=0;
//	}
}

void write_THD(float32_t value, uint32_t place)
{
	switch (place)
    {
    	case ULN1_THD:
			ULN1.THD_value = value;
    		break;
    	case ULN2_THD:
			ULN2.THD_value = value;
    		break;
		case ULN3_THD:
			ULN3.THD_value = value;
    		break;
    	case IL1_THD:
			IL1.THD_value = value;
    		break;
		case IL2_NORMAL:
			IL2.THD_value = value;
    		break;
		case IL3_NORMAL:
			IL3.THD_value = value;
    		break;
    	default:
    		break;
    }
}
