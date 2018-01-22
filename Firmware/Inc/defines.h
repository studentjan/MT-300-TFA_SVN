  //******************************************************************************
  //* File Name          : defines.h
  //* Description        : Defines
  //* Created			 : Jan Koprivec
  //* Date				 : 13.7.2017
  //******************************************************************************

//define to prevent recursive inclusion
#ifndef __DEFINES_H
#define __DEFINES_H

//firmware version
//+++++++++++++++++++++++++++++++++++++++++++++++
#define _FW_VERSION         01
#define _FW_VERSION_TXT    "01"
//+++++++++++++++++++++++++++++++++++++++++++++++


//general constants
//++++++++++++++++++++++++++++++++++++++++++++++++
#define _UNDEF 		   255

#define _NO    			 0
#define _YES		     1

#define _NAN    		 0
#define _VALID		     1

#define _OFF    	     0
#define _ON			     1

#define _LEFT    	     0
#define _RIGHT			 1

#define _OK  			 0
#define _ERROR		     1
//++++++++++++++++++++++++++++++++++++++++++++++++

//math constants
//++++++++++++++++++++++++++++++++++++++++++++++++
#define __PI	3.14159f
#define __2PI	6.28318f
#define __4PI	12.5663f
#define __6PI	18.8495f
#define __8PI	25.1327f
//++++++++++++++++++++++++++++++++++++++++++++++++

//GPIO pin deffinitions
//++++++++++++++++++++++++++++++++++++++++++++++++
#define IND_LED_Pin GPIO_PIN_2
#define IND_LED_GPIO_Port GPIOE
#define TEST_PAD1_Pin GPIO_PIN_3
#define TEST_PAD1_GPIO_Port GPIOE
#define TEST_PAD2_Pin GPIO_PIN_4
#define TEST_PAD2_GPIO_Port GPIOE
#define TEST_PAD3_Pin GPIO_PIN_5
#define TEST_PAD3_GPIO_Port GPIOE
#define TEST_PAD4_Pin GPIO_PIN_6
#define TEST_PAD4_GPIO_Port GPIOE
#define IND_PE_Pin GPIO_PIN_15
#define IND_PE_GPIO_Port GPIOC
#define IND_1P_Pin GPIO_PIN_14
#define IND_1P_GPIO_Port GPIOC
#define IND_3P_Pin GPIO_PIN_13
#define IND_3P_GPIO_Port GPIOC
#define TRIM_CS1_Pin GPIO_PIN_0
#define TRIM_CS1_GPIO_Port GPIOA
#define TRIM_CS2_Pin GPIO_PIN_1
#define TRIM_CS2_GPIO_Port GPIOA
#define TRIM_CS3_Pin GPIO_PIN_2
#define TRIM_CS3_GPIO_Port GPIOA
#define TRIM_UD_Pin GPIO_PIN_3
#define TRIM_UD_GPIO_Port GPIOA
//current gaine sm mogu premenat zarad napacne porazdelitve
#define CURRENT_GAIN1_Pin GPIO_PIN_2//GPIO_PIN_3
#define CURRENT_GAIN1_GPIO_Port GPIOD
#define CURRENT_GAIN3_Pin GPIO_PIN_3//GPIO_PIN_1
#define CURRENT_GAIN3_GPIO_Port GPIOD
#define CURRENT_GAIN2_Pin GPIO_PIN_1//GPIO_PIN_2
#define CURRENT_GAIN2_GPIO_Port GPIOD
//--------------------------------------------------------
#define L1_SINHRO_Pin GPIO_PIN_6
#define L1_SINHRO_GPIO_Port GPIOA
#define USB_PU_Pin GPIO_PIN_9
#define USB_PU_GPIO_Port GPIOA
#define USB_VBUS_Pin GPIO_PIN_10
#define USB_VBUS_GPIO_Port GPIOA
#define RS485_DET_Pin GPIO_PIN_6
#define RS485_DET_GPIO_Port GPIOF
#define RELAY_CS1_Pin GPIO_PIN_7//4
#define RELAY_CS1_GPIO_Port GPIOD
#define RELAY_CS2_Pin GPIO_PIN_5
#define RELAY_CS2_GPIO_Port GPIOD
#define RELAY_CS3_Pin GPIO_PIN_12
#define RELAY_CS3_GPIO_Port GPIOC
#define RELAY_CS4_Pin GPIO_PIN_4
#define RELAY_CS4_GPIO_Port GPIOD
#define RELAY_CS5_Pin GPIO_PIN_6//7
#define RELAY_CS5_GPIO_Port GPIOD
#define RELAY_RESET_Pin GPIO_PIN_1
#define RELAY_RESET_GPIO_Port GPIOC
#define SDADC1_AIN6P_ULN3_Pin GPIO_PIN_0
#define SDADC1_AIN6P_ULN3_GPIO_Port GPIOB
#define SDADC1_AIN6M_ULN3_Pin GPIO_PIN_1
#define SDADC1_AIN6M_ULN3_GPIO_Port GPIOB
#define SDADC1_AIN4P_ULN2_Pin GPIO_PIN_2
#define SDADC1_AIN4P_ULN2_GPIO_Port GPIOB
#define SDADC1_AIN4M_ULN2_Pin GPIO_PIN_7
#define SDADC1_AIN4M_ULN2_GPIO_Port GPIOE
#define SDADC1_AIN8P_ULN1_Pin GPIO_PIN_8
#define SDADC1_AIN8P_ULN1_GPIO_Port GPIOE
#define SDADC1_AIN8M_ULN1_Pin GPIO_PIN_9
#define SDADC1_AIN8M_ULN1_GPIO_Port GPIOE
#define SDADC2_AIN4P_IL3_Pin GPIO_PIN_11
#define SDADC2_AIN4P_IL3_GPIO_Port GPIOE
#define SDADC2_AIN4M_IL3_Pin GPIO_PIN_12
#define SDADC2_AIN4M_IL3_GPIO_Port GPIOE
#define SDADC2_AIN2P_IL2_Pin GPIO_PIN_13
#define SDADC2_AIN2P_IL2_GPIO_Port GPIOE
#define SDADC2_AIN2M_IL2_Pin GPIO_PIN_14
#define SDADC2_AIN2M_IL2_GPIO_Port GPIOE
#define SDADC2_AIN0P_IL1_Pin GPIO_PIN_15
#define SDADC2_AIN0P_IL1_GPIO_Port GPIOE
#define SDADC2_AIN0M_IL1_Pin GPIO_PIN_10
#define SDADC2_AIN0M_IL1_GPIO_Port GPIOB
//++++++++++++++++++++++++++++++++++++++++++++++++


//LEDS definitions
//++++++++++++++++++++++++++++++++++++++++++++++++
#define LED_TEST_off IND_LED_GPIO_Port->BSRR = IND_LED_Pin
#define LED_TEST_on  IND_LED_GPIO_Port->BRR  = IND_LED_Pin
#define _LED_BLINK_TIME_OK                       10     //100ms 
#define _LED_BLINK_TIME_ERROR                   100     //1s 
#define IND_1P_ON			HAL_GPIO_WritePin(IND_1P_GPIO_Port, IND_1P_Pin, GPIO_PIN_SET)
#define IND_1P_OFF		HAL_GPIO_WritePin(IND_1P_GPIO_Port, IND_1P_Pin, GPIO_PIN_RESET)
#define IND_1P_TOG		HAL_GPIO_TogglePin(IND_1P_GPIO_Port, IND_1P_Pin)
#define IND_3P_ON			HAL_GPIO_WritePin(IND_3P_GPIO_Port, IND_3P_Pin, GPIO_PIN_SET)
#define IND_3P_OFF		HAL_GPIO_WritePin(IND_3P_GPIO_Port, IND_3P_Pin, GPIO_PIN_RESET)
#define IND_3P_TOG		HAL_GPIO_TogglePin(IND_3P_GPIO_Port, IND_3P_Pin)
#define IND_PE_ON			HAL_GPIO_WritePin(IND_PE_GPIO_Port, IND_PE_Pin, GPIO_PIN_SET)
#define IND_PE_OFF		HAL_GPIO_WritePin(IND_PE_GPIO_Port, IND_PE_Pin, GPIO_PIN_RESET)
#define IND_BLINKING_TIME													5			//n*100ms
//++++++++++++++++++++++++++++++++++++++++++++++++

//TEST pins
//++++++++++++++++++++++++++++++++++++++++++++++++
#define test4_off HAL_GPIO_WritePin(TEST_PAD4_GPIO_Port, TEST_PAD4_Pin, GPIO_PIN_RESET)
#define test4_on  HAL_GPIO_WritePin(TEST_PAD4_GPIO_Port, TEST_PAD4_Pin, GPIO_PIN_SET)
#define test4_tog HAL_GPIO_TogglePin(TEST_PAD4_GPIO_Port, TEST_PAD4_Pin)

#define test3_off HAL_GPIO_WritePin(TEST_PAD3_GPIO_Port, TEST_PAD3_Pin, GPIO_PIN_RESET)
#define test3_on  HAL_GPIO_WritePin(TEST_PAD3_GPIO_Port, TEST_PAD3_Pin, GPIO_PIN_SET)
#define test3_tog HAL_GPIO_TogglePin(TEST_PAD3_GPIO_Port, TEST_PAD3_Pin)

#define test2_off HAL_GPIO_WritePin(TEST_PAD2_GPIO_Port, TEST_PAD2_Pin, GPIO_PIN_RESET)
#define test2_on  HAL_GPIO_WritePin(TEST_PAD2_GPIO_Port, TEST_PAD2_Pin, GPIO_PIN_SET)
#define test2_tog HAL_GPIO_TogglePin(TEST_PAD2_GPIO_Port, TEST_PAD2_Pin)

#define test1_off HAL_GPIO_WritePin(TEST_PAD1_GPIO_Port, TEST_PAD1_Pin, GPIO_PIN_RESET)
#define test1_on  HAL_GPIO_WritePin(TEST_PAD1_GPIO_Port, TEST_PAD1_Pin, GPIO_PIN_SET)
#define test1_tog HAL_GPIO_TogglePin(TEST_PAD1_GPIO_Port, TEST_PAD1_Pin)
//++++++++++++++++++++++++++++++++++++++++++++++++


//CONTACTORS
//++++++++++++++++++++++++++++++++++++++++++++++++
#define SET_L1_CONTACTOR	set_REL(4)
#define SET_L2_CONTACTOR	set_REL(3)
#define SET_L3_CONTACTOR	set_REL(2)
#define SET_N_CONTACTOR		set_REL(6)
#define SET_PE_CONTACTOR	rst_REL(5)	//set_REL(5)
#define RST_L1_CONTACTOR	rst_REL(4)
#define RST_L2_CONTACTOR	rst_REL(3)
#define RST_L3_CONTACTOR	rst_REL(2)
#define RST_N_CONTACTOR		rst_REL(6)
#define RST_PE_CONTACTOR	set_REL(5)	//rst_REL(5)
//++++++++++++++++++++++++++++++++++++++++++++++++

//USB
//++++++++++++++++++++++++++++++++++++++++++++++++
#define USB_PU_ON 	HAL_GPIO_WritePin(USB_PU_GPIO_Port, USB_PU_Pin, GPIO_PIN_RESET)
#define USB_PU_OFF  HAL_GPIO_WritePin(USB_PU_GPIO_Port, USB_PU_Pin, GPIO_PIN_SET)
#define INPUT_BUFFER_SIZE 100
#define TERMINATOR_CHAR 13			//stevilka znaka terminatorja
//++++++++++++++++++++++++++++++++++++++++++++++++

//COMUNICATION
//++++++++++++++++++++++++++++++++++++++++++++++++
#define CONNECTION_CHECK						_OFF
#define CONNECTION_CHECK_INTERVAL		500		//n*10ms
//++++++++++++++++++++++++++++++++++++++++++++++++

//TEST
//++++++++++++++++++++++++++++++++++++++++++++++++
#define BUTTON_CHECK_TIME 5
//++++++++++++++++++++++++++++++++++++++++++++++++

//REL_DRIVER
//++++++++++++++++++++++++++++++++++++++++++++++++
#define REL_RESET_OFF HAL_GPIO_WritePin(RELAY_RESET_GPIO_Port,RELAY_RESET_Pin,GPIO_PIN_RESET);
#define REL_RESET_ON  HAL_GPIO_WritePin(RELAY_RESET_GPIO_Port,RELAY_RESET_Pin,GPIO_PIN_SET);
#define REL_CS1_ON		HAL_GPIO_WritePin(RELAY_CS1_GPIO_Port, RELAY_CS1_Pin, GPIO_PIN_RESET)
#define REL_CS1_OFF		HAL_GPIO_WritePin(RELAY_CS1_GPIO_Port, RELAY_CS1_Pin, GPIO_PIN_SET)
#define REL_CS2_ON		HAL_GPIO_WritePin(RELAY_CS2_GPIO_Port, RELAY_CS2_Pin, GPIO_PIN_RESET)
#define REL_CS2_OFF		HAL_GPIO_WritePin(RELAY_CS2_GPIO_Port, RELAY_CS2_Pin, GPIO_PIN_SET)
#define REL_CS3_ON		HAL_GPIO_WritePin(RELAY_CS3_GPIO_Port, RELAY_CS3_Pin, GPIO_PIN_RESET)
#define REL_CS3_OFF		HAL_GPIO_WritePin(RELAY_CS3_GPIO_Port, RELAY_CS3_Pin, GPIO_PIN_SET)
#define REL_CS4_ON		HAL_GPIO_WritePin(RELAY_CS4_GPIO_Port, RELAY_CS4_Pin, GPIO_PIN_RESET)
#define REL_CS4_OFF		HAL_GPIO_WritePin(RELAY_CS4_GPIO_Port, RELAY_CS4_Pin, GPIO_PIN_SET)
#define REL_CS5_ON		HAL_GPIO_WritePin(RELAY_CS5_GPIO_Port, RELAY_CS5_Pin, GPIO_PIN_RESET)
#define REL_CS5_OFF		HAL_GPIO_WritePin(RELAY_CS5_GPIO_Port, RELAY_CS5_Pin, GPIO_PIN_SET)
#define REL_OUT1			0x01
#define REL_OUT2			0x02
#define REL_OUT3			0x04
#define REL_OUT4			0x08
#define REL_OUT5			0x10
#define REL_OUT6			0x20
#define REL_OUT7			0x40
#define REL_OUT8			0x80
//++++++++++++++++++++++++++++++++++++++++++++++++

//SDADC
//++++++++++++++++++++++++++++++++++++++++++++++++
//ena cela stevilka je 100us
#define SDADC_ON_DELAY 				187 //*0.0001s je zakasnitveni cas, za katerega zakasnimo vklop sdadcja,
#define SDADC_ON_IL1THD_DELAY 1189 //zato da zacnemo ravno ko signal precka nulo3
#define SDADC_ON_IL2THD_DELAY 1187 //sdadc potrebuje cca 70us za zagon
#define SDADC_ON_IL3THD_DELAY 1185 //cca 1,24ms da se vklopi interrupt po preckanju nule torej 1,3ms do 1. sampla za THD je maks 1200, ker potem podaljsa cas >500ms

//THD zamik zato, da damo SDADCju cas da se ustali preden pomerimo vzorce
#define CALIBRATION_TIMEOUT			30
//stevilo samplov za racunanje efektivne vrednosti
#define SDADC3_IDIFF_SAMPLE_CNT 1110	//stevilo samplov z 5,555 kHz vzorcenjem za 10 period
#define SDADC2_IL_SAMPLE_CNT	1110
#define SDADC1_ULN_SAMPLE_CNT 1110	//stevilo samplov z 5,555 kHz vzorcenjem za 10 period
#define SDADC3_UL1PE_SAMPLE_CNT 1110
#define SDADC3_UNPE_SAMPLE_CNT 1110
#define THD_MEASURE_CNT					1000
#define ULN_MEAS_FS							5555.5f	//frekvenca vzorcenja
#define IL_MEAS_FS							5555.5f	//frekvenca vzorcenja
#define PHASE_UNCERTANTY				50			//n*10u :za koliko lahko neka faza odstopa da recemo da je se v fazi z drugo
#define PHASE_AVARAGE_CNT				5
#define sdadc2_il								0.0009009009f
//vklop filtrov
#define SDADC1_high_pass_filter _ON 	// vklop HPF
#define SDADC1_low_pass_filter  _ON   // vklop LPF
#define SDADC2_high_pass_filter _ON 	// vklop HPF
#define SDADC2_low_pass_filter  _ON   // vklop LPF
#define SDADC3_high_pass_filter _ON 	// vklop HPF
#define SDADC3_low_pass_filter  _ON   // vklop LPF
#define IDIFF_high_pass_filter	_ON
#define IDIFF_low_pass_filter		_ON
#define UL1PE_high_pass_filter	_ON
#define UL1PE_low_pass_filter		_ON
#define UNPE_high_pass_filter		_ON
#define UNPE_low_pass_filter		_ON
#define IL_Hanning_window			_OFF
#define ULN_Hanning_window			_OFF

//gain konstante
#define IL1_GAIN1_CORRECTION		0.794f
#define IL1_GAIN40_CORRECTION		1.394547f
#define IL2_GAIN1_CORRECTION		0.797f
#define IL2_GAIN40_CORRECTION		1.39743f
#define IL3_GAIN1_CORRECTION		0.78881f
#define IL3_GAIN40_CORRECTION		1.3941f
#define IL1_GAIN1								84.0f*0.0000305175f	//*100 - prej: 0.0000305175f
#define IL1_GAIN40							2.12f*0.0000305175f
#define IL2_GAIN1								84.0f*0.0000305175f
#define IL2_GAIN40							2.12f*0.0000305175f
#define IL3_GAIN1								84.0f*0.0000305175f
#define IL3_GAIN40							2.12f*0.0000305175f
#define ULN1_GAIN1							612.5f*0.0000305175f	//*100 - prej: 0.0000305175f
#define ULN2_GAIN1							612.5f*0.00305175f	//*100 - prej: 0.0000305175f
#define ULN3_GAIN1							612.5f*0.00305175f	//*100 - prej: 0.0000305175f
#define IDIFF_GAIN_1						26.52f*0.0000305175f//--tole je za trafo MR-4//40.8f*0.0000305175f	//-tale je za CR8420//full scale na gain 1 je (+,-) 0,0408 A - podatek v mA
#define UL1PE_GAIN1							613.0f*0.0000305175f
#define UNPE_GAIN1							613.0f*0.0000305175f

		

//THD racunanje
#define CORELATION							1
#define FFT									0
#define THD_COMPUTATION_METHOD				FFT
#define	FFT_THD_BUFFER_SIZE					1024		//*2 zaradi CFFT
#define FFT_SAMPLE_NUMBER					999//1024-1 sample number-1
#define CORELATION_THD_BUFFER_SIZE			1000
#define FFT_BIT_REVERSE						_ON
#define CORELATION_SAMPLE_NUMBER			999


//Okna
#define	__RACTANGULAR	0	
#define	__FLAT_TOP		1		//najboljsi za uporabo s FFT racunanjem	
#define __HANN			2			//najboljsi za uporabo s KORELACIJSKIM racunanjem
#define __HAMMING		3
#define	THD_FFT_WINDOW	__FLAT_TOP	


#define ULN1_THD								1
#define ULN2_THD								2
#define ULN3_THD								3
#define IL1_THD									4
#define IL2_THD									5
#define IL3_THD									6
#define IL1_NORMAL							7
#define IL2_NORMAL							8
#define IL3_NORMAL							9

#define ULN1_CHANNEL						0x00000008
#define ULN2_CHANNEL						0x00000004
#define ULN3_CHANNEL						0x00000006
//IL-je sm mogu premenat zarad napacne porazdelitve
#define IL1_CHANNEL							0x00000002//0x00000000
#define IL2_CHANNEL							0x00000004//0x00000002
#define IL3_CHANNEL							0x00000000//0x00000004
//---
#define IDIFF_CHANNEL						0x00000000
#define UL1PE_CHANNEL						0x00000002
#define UNPE_CHANNEL						0x00000004
#define LEAKAGE_CURRENT_CHANNEL	0x00000006
//histereza za auto gain pri IL toku
#define IL_AUTO_GAIN_TD					0x0064	//100
#define IL_AUTO_GAIN_TU					0x7FF8	//32760

#define IDIFF_K1								(float)0.00295		//v mA
#define IDIFF_N1								(float)0.0//-0.00398f
#define IDIFF_K2								(float)0.000459
#define IDIFF_N2								(float)0.0//-0.0088f
#define IDIFF_K3								(float)0.0058196
#define IDIFF_N3								(float)0.0//-0.023f
#define IDIFF_K									(float)0.9527
#define IDIFF_N									(float)-0.081
#define IDIFF_Klow							(float)0.9428
#define IDIFF_Nlow							(float)0//-0.0485f
#define IDIFF_Khigh							(float)0.955
#define IDIFF_Nhigh							(float)-0.063
#define COS60										(float)0.5
//zastavice za meritve

//omejitve za opravljanje zacetnega testa
#define INIT_VOLTAGE_HIGH_LIMIT	180.0f
#define INIT_VOLTAGE_LOW_LIMIT	50.0f
#define INIT_PE_LOW_LIMIT				50.0f
#define INIT_PE_HIGH_LIMIT			190.0f
#define PHASE_DETECT_HIGH_LIMIT	150.0f
#define PHASE_DETECT_LOW_LIMIT	20.0f
#define MAINS_PLUS10						270.0f//253.0f
#define MAINS_MINUS10						180.0f//207.0f
//#define THROW_AWAY_RESULTS			1				//koliko rezultatov vrzemo stran preden izberemo pravega za zacetno kontrolo - pomembno zaradi filtrov

enum MEAS_ON_MASKS
{
  __IDIFF_MASK  			= 1,						//0x00000001
	__ULN1_MASK					= 0x00000002,
	__ULN2_MASK					= 0x00000004,
	__ULN3_MASK					= 0x00000008,
	__IL1_MASK					= 0x00000010,
	__IL2_MASK					= 0x00000020,
	__IL3_MASK					= 0x00000040,
	__MULTIMODE_MASK		= 0x00000080,
	__UL1PE_MASK				= 0x00000100,
	__UNPE_MASK					= 0x00000200,
	__SDADC1_START_MASK	= 0x00000400,
	__SDADC2_START_MASK	= 0x00000800,
	__SDADC3_START_MASK	= 0x00001000,
	__IL1_MEASURED			= 0x00002000,
	__IL2_MEASURED			= 0x00004000,
	__IL3_MEASURED			= 0x00008000,
	__ULN1_MEASURED			= 0x00010000,
	__ULN2_MEASURED			= 0x00020000,
	__ULN3_MEASURED			= 0x00040000,
	__UL1PE_MEASURED		=	0x00080000,
	__UNPE_MEASURED			=	0x00100000,
	__IDIFF_MEASURED		=	0x00200000,
	__THD_MEASURING			=	0x00400000,
	__IL_THD_MEASURED		=	0x00800000,
	__ULN_THD_MEASURED	=	0x01000000,
	__MEAS_IN_PROGRESS	=	0x02000000,
	__START_TIMER_ON		= 0x04000000,
	__NO_THD_MEAS				= 0x08000000,			//vklopljena meritev brez THD
	__DEL_START_NORMAL 	= 0x10000000,
	__DEL_START_THD	  	= 0x20000000,
	__THD_SYNCHRO_ON		= 0x40000000,	//vklopi sinhronizacijo za THD meritve
};
enum COMPUTE_MASKS
{
	__ULN1_SAMPLED			=0x00000001,
	__ULN2_SAMPLED			=0x00000002,
	__ULN3_SAMPLED			=0x00000004,
	__IL1_SAMPLED				=0x00000008,
	__IL2_SAMPLED				=0x00000010,
	__IL3_SAMPLED				=0x00000020,
	__UL1PE_SAMPLED			=0x00000040,
	__UNPE_SAMPLED			=0x00000080,
	__ULN1_THD_SAMPLED	=0x00000100,
	__ULN2_THD_SAMPLED	=0x00000200,
	__ULN3_THD_SAMPLED	=0x00000400,
	__IL1_THD_SAMPLED		=0x00000800,
	__IL2_THD_SAMPLED		=0x00001000,
	__IL3_THD_SAMPLED		=0x00002000,
	__IDIFF_SAMPLED			=0x00004000,
	__ULN1_THD_SAMPLING	=0x00008000,	//-----------postavljene ko se zajema vzorce------
	__ULN2_THD_SAMPLING	=0x00010000,	//+
	__ULN3_THD_SAMPLING	=0x00020000,	//+
	__IL1_THD_SAMPLING	=0x00040000,	//+
	__IL2_THD_SAMPLING	=0x00080000,	//+
	__IL3_THD_SAMPLING	=0x00100000,	//------------------------------------------------
	__ULN1_THD_START		=0x00200000,	//-------postavijo sampling zastavice s sinhronizacijo------
	__ULN2_THD_START		=0x00400000,	//+
	__ULN3_THD_START		=0x00800000,	//+
	__IL1_THD_START			=0x01000000,	//+
	__IL2_THD_START			=0x02000000,	//+
	__IL3_THD_START			=0x04000000,	//----------------------------------------------------------
	__IL1_EFF_COMPUTED	=0x08000000,
	__IL2_EFF_COMPUTED	=0x10000000,
	__IL3_EFF_COMPUTED	=0x20000000,
	__ULN1_EFF_COMPUTED	=0x40000000,
};
enum COMPUTE_MASKS_2
{
	__ULN2_EFF_COMPUTED	=0x00000001,
	__ULN3_EFF_COMPUTED	=0x00000002,
	__POWER_MEAS_ON 		=0x00000004,	//vklopi merjenje moci
	__INIT_MEASURED			=0x00000008,
	__PHASE1_MEASURED		=0x00000010,
	__PHASE2_MEASURED		=0x00000020,
	__PHASE3_MEASURED		=0x00000040,
	__GET_PHASES				=0x00000080,
	__IN_PHASE_1_2			=0x00000100,
	__IN_PHASE_2_3			=0x00000200,
	__IN_PHASE_1_3			=0x00000400,
	
};
enum GLOBAL_MASKS
{
	__PE_DISCONNECTED		=0x00000001,
	__N_DISCONECTED			=0x00000002,
	__INIT_TEST_PASS		=0x00000004,
	__IN_CONNECTION_OK	=0x00000008,
	__ON_TEST_IN_PROG		=0x00000010,
	__INPUT_PHASES_DET	=0x00000020,
	__VOLTAGES_IN_PHASE =0x00000040,
	__WRONG_CONNECTION	=0x00000080,
	__1P_CONNECTION			=0x00000100,
	__3P_CONNECTION			=0x00000200,
	__INIT_TEST_FAIL		=0x00000400,
	__ULN1_OUT_OF_RANGE =0x00000800,
	__ULN2_OUT_OF_RANGE =0x00001000,
	__ULN3_OUT_OF_RANGE =0x00002000,
	__PHASE_SEQ_123			=0x00004000,
	__PHASE_SEQ_INPHASE =0x00008000,
	__PHASE_SEQ_UNKNOWN	=0x00010000,	//dve vhodni napetosti imata isto fazo tretja pa razlicno, lahko je tudi kaj drugega
	__PHASE_SEQ_132			=0x00020000,
	__PHASE_SEQ_1P_SYS	=0x00040000,
	__PHASE_MISSING			=0x00080000,
	__L_ON_N						=0x00100000,
	__L_ON_L						=0x00200000,
	__TURN_ON_CONTACTOR =0x00400000,
};
enum CONNECTION_MASKS
{
	__CON_TO_MT310						=0x00000001,		//naprava, ki je povezana na TFA lahko tudi simulator
	__CON_TO_TERMINAL					=0x00000002,
	__CON_TO_PAT							=0x00000004,		//naprava, ki je povezana na TFA lahko tudi simulator
	__CONNECTION_ESTABLISHED 	=0x00000008,
	__CHECK_TO_MT310					=0x00000010,
	__CHECK_TO_TERMINAL				=0x00000020,
	__CHECK_TO_PAT						=0x00000040,
	__ESTABLISHED_TO_SIM_USB	=0x00000080,
	__ESTABLISHED_TO_MT310		=0x00000100,
	__ESTABLISHED_TO_PAT_USB	=0x00000200,
	__USB_IN_SOCKET						=0x00000400,
	__SERIAL_INITIATED				=0x00000800,
	__USB_PLUG_CHANGED				=0x00001000
};

//definicije za meas_task_control
enum MEAS_TASK_MASKS
{
	__CORD_MEAS_IN_PROG												=0x00000001,
	__MACH_MEAS_IN_PROG												=0x00000002,
	__WELD_MEAS_IN_PROG												=0x00000004,
	__CALIB_MEAS_IN_PROG											=0x00000008,
	
	__RETURN_CURRENT													=0x00000100,
	__RETURN_VOLTAGE													=0x00000200,
	__RETURN_THD_C														=0x00000400,
	__RETURN_THD_V														=0x00000800,
	__RETURN_POWER_R													=0x00001000,
	__RETURN_POWER_A													=0x00002000,
	__RETURN_PF																=0x00004000,


};

#define __MEAS_TASKS_IN_PROG_MASK						0x0000000F
#define __TASK_RETURN_MASKS									0x00007F00

enum SYNCHRO_INTERRUPT_USAGE
{
	__URES_SYNCHRO														=0x00000001,
	__VOLTAGE_MEAS_SYNCHRO										=0x00000002,
	__CONTACTOR_SYNCHRO_ON										=0x00000004,
	__SET_L1_CONTACTOR												=0x00000008,
	__SET_L2_CONTACTOR												=0x00000010,
	__SET_L3_CONTACTOR												=0x00000020,
	__SET_N_CONTACTOR													=0x00000040,
	__SET_PE_CONTACTOR												=0x00000080,
};

#define SYNCHRO_INTERRUPT_MASK		0x00000007

#define	__IDIFF	 	1
#define	__ULN1	 	2
#define	__ULN2	 	3
#define	__ULN3	 	4
#define	__IL1		 	5
#define	__IL2		 	6
#define	__IL3		 	7
#define	__UL1PE	 	8
#define	__UNPE	 	9
#define __T_ULN1	10
#define __T_ULN2	11
#define __T_ULN3	12
#define	__T_IL1		13
#define	__T_IL2		14
#define	__T_IL3		15
#define __PL1			16
#define __PL2			17
#define __PL3			18
#define __SL1			19
#define __SL2			20
#define __SL3			21
#define __PF1			22
#define __PF2			23
#define __PF3			24
#define __ULN1_K	25
#define __ULN2_K	26
#define __ULN3_K	27
#define __ULN1_N	28
#define __ULN2_N	29
#define __ULN3_N	30
#define __IL1_K		31
#define __IL2_K		32
#define __IL3_K		33
#define __IL1_N		34
#define __IL2_N		35
#define __IL3_N		36


//INTERRUPT MASKS
#define __EXTI_9_5			0x00000001

enum IL_GAIN{__GAIN1,	__GAIN40};

//FUNCTIONS
#define	__MT_300__			"MT-300-TFA"

//COMMANDS
#define	__POWER__				"POWER"
#define	__RELAY__				"RELAY"
#define	__TEST__				"TEST"
#define	__CONNECTION__	"COMMUN"
#define __WARNING__			_MESSAGE_WAR
#define	__CORD__				"CORD"
#define __STATUS__			"STATUS"
#define __CALIB__				"CALIB"


//ADDITIONAL CODES
//POWER
#define __START__					"START"
#define __STOP__					"STOP"
#define __START_NO_THD__	"START_NTHD"
#define __ULN1__					"ULN1"
#define __ULN2__					"ULN2"
#define __ULN3__					"ULN3"
#define __IDIFF__					"IDIFF"
#define __UNPE__					"UNPE"
#define __UL1PE__					"UL1PE"
#define __IL1__						"IL1"
#define __IL2__						"IL2"
#define __IL3__						"IL3"

//RELAY
#define __1_38_ON__			"1_38_ON"
#define __1_38_OFF__		"1_38_OFF"
#define __2_ON__				"2_ON"
#define __2_OFF__				"2_OFF"
#define __3_ON__				"3_ON"
#define __3_OFF__				"3_OFF"
#define __4_ON__				"4_ON"
#define __4_OFF__				"4_OFF"
#define __5_ON__				"5_ON"
#define __5_OFF__				"5_OFF"
#define __6_ON__				"6_ON"
#define __6_OFF__				"6_OFF"
#define __7_ON__				"7_ON"
#define __7_OFF__				"7_OFF"
#define __8_ON__				"8_ON"
#define __8_OFF__				"8_OFF"
#define __9_ON__				"9_ON"
#define __9_OFF__				"9_OFF"
#define __10_ON__				"10_ON"
#define __10_OFF__			"10_OFF"
#define __11_ON__				"11_ON"
#define __11_OFF__			"11_OFF"
#define __12_ON__				"12_ON"
#define __12_OFF__			"12_OFF"
#define __13_ON__				"13_ON"
#define __13_OFF__			"13_OFF"
#define __14_ON__				"14_ON"
#define __14_OFF__			"14_OFF"
#define __15_ON__				"15_ON"
#define __15_OFF__			"15_OFF"
#define __16_ON__				"16_ON"
#define __16_OFF__			"16_OFF"
#define __17_ON__				"17_ON"
#define __17_OFF__			"17_OFF"
#define __18_ON__				"18_ON"
#define __18_OFF__			"18_OFF"
#define __19_ON__				"19_ON"
#define __19_OFF__			"19_OFF"
#define __20_ON__				"20_ON"
#define __20_OFF__			"20_OFF"
#define __21_43_ON__		"21_43_ON"
#define __21_43_OFF__		"21_43_OFF"
#define __22_ON__				"22_ON"
#define __22_OFF__			"22_OFF"
#define __23_ON__				"23_ON"
#define __23_OFF__			"23_OFF"
#define __24_ON__				"24_ON"
#define __24_OFF__			"24_OFF"
#define __25_ON__				"25_ON"
#define __25_OFF__			"25_OFF"
#define __26_ON__				"26_ON"
#define __26_OFF__			"26_OFF"
#define __27_ON__				"27_ON"
#define __27_OFF__			"27_OFF"
#define __28_ON__				"28_ON"
#define __28_OFF__			"28_OFF"
#define __29_ON__				"29_ON"
#define __29_OFF__			"29_OFF"
#define __30_ON__				"30_ON"
#define __30_OFF__			"30_OFF"
#define __31_ON__				"31_ON"
#define __31_OFF__			"31_OFF"
#define __32_ON__				"32_ON"
#define __32_OFF__			"32_OFF"
#define __33_ON__				"33_ON"
#define __33_OFF__			"33_OFF"
#define __34_ON__				"34_ON"
#define __34_OFF__			"34_OFF"
#define __35_ON__				"35_ON"
#define __35_OFF__			"35_OFF"
#define __36_ON__				"36_ON"
#define __36_OFF__			"36_OFF"
#define __37_ON__				"37_ON"
#define __37_OFF__			"37_OFF"
#define __39_ON__				"39_ON"
#define __39_OFF__			"39_OFF"
#define __40_ON__				"40_ON"
#define __40_OFF__			"40_OFF"
#define __41_ON__				"41_ON"
#define __41_OFF__			"41_OFF"
#define __42_ON__				"42_ON"
#define __42_OFF__			"42_OFF"
#define __RESET_ALL__		"RESET_ALL"

//TEST
#define __PROTOCOL_TEST__ "PROTOCOL_TEST"

//++++++++++CONNECTION+++++++++++++++
//--additionalcode
#define __CONNECTION_REQUEST__			"CONNECT_REQUEST"
#define __CONNECTION_ESTABLISHED__	"CONNECTION_ESTABLED"
#define __CHECK_CONNECTION__				"CONNECTION_CHECK"
#define __CONNECTION_DENIED__				"CONNECTION_DENIED"
//--add
#define __MT_310__			"MT-310"
#define __PAT__					"PAT"
#define __SIMUMATION__	"SIM"
#define __CON_OK_				"OK"
#define __CON_NOK_			"NOK"
#define __ALREADY_CONNECTED__				"ALREADY_CONNECTED"
#define __DIFF_DEVICE_CONNECTED__		"DIFF_DEVICE_CONNECTED"

//++++++++++MAINS ANALYZE+++++++++++++
#define __START_ANALYZE__			"START_ANALYZE"
#define __ANALYZE_STARTED__		"ANALYZE_STARTED"
#define __GET_VOLTAGE__				"GET_VOLTAGE"
#define __RETURN_VOLTAGE__		"VOLTAGE"
#define __GET_CURRENT__				"GET_CURRENT"
#define __RETURN_CURRENT__		"CURRENT"
#define __GET_THD_C__					"GET_THD_C"
#define __RETURN_THD_C__			"THD_C"
#define __GET_THD_V__					"GET_THD_V"
#define __RETURN_THD_V__			"THD_V"
#define __GET_POWER_R__				"GET_POWER_R"
#define __RETURN_POWER_R__		"POWER_R"
#define __GET_POWER_A__				"GET_POWER_A"
#define __RETURN_POWER_A__		"POWER_A"
#define __GET_PF__						"GET_PF"
#define __RETURN_PF__					"PF"
#define __STOP_ANALYZE__			"STOP_ANALYZE"
#define __ANALYZE_STOPPED__		"ANALYZE_STOPPED"
//----------------------------------------

//+++++++++++++++CORD+++++++++++++++++
//--additionalcode
#define __INIT_CORD__	"INIT"
#define __DEINIT_CORD__		"DEINIT"
#define __DEINITIATED__		"DEINITIATED"
#define __START_CORRECT_WIRING__	"START_C_W"
#define __CORD_CW__					"CW"
#define __CORD_CW_STARTED__	"CW_STARTED"
#define __CORD_CW_RESULT__	"CW_RESULT"
#define __STOP_C__						"STOP"
#define __INIT_C__					"INIT"
#define __SET__							"SET"
#define __SET_COMPLETED__		"SET_COMPLETED"		
#define __CABLE_TYPE__			"CABLE_TYPE"
#define __CORD_INITIATED__			"INITIATED"
#define __CORD_INIT_FAILED__		"INIT_FAILED"
#define __START_RPE_LOW__			"RPE_L_START"
#define __RPE_LOW_STARTED__		"RPE_L_STARTED"
#define __START_RPE_HIGH__		"RPE_H_START"
#define __RPE_HIGH_STARTED__	"RPE_H_STARTED"
#define __STOP_RPE__					"RPE_STOP"
#define __RPE_STOPPED__				"RPE_STOPPED"
#define __GET_RPE_RES__			"RPE_RES_GET"
#define __RPE_RESISTANCE__		"RPE_RES"
#define __START_RISO__			"RISO_START"
#define __RISO_STARTED__		"RISO_STARTED"
#define __GET_RISO_RES__		"RISO_RES_GET"
#define __RISO_RESISTANCE__	"RISO_RES"
#define __RISO_PHASES_TO_PE_OK__		"RISO_ALL-PE|OK"
#define __RISO_PHASES_TO_PE_FAIL__	"RISO_ALL-PE|FAIL"
#define __STOP_RISO__				"RISO_STOP"
#define __RISO_STOPPED__		"RISO_STOPPED"
#define __L1_OPEN__					"L1_OPEN"
#define __L1_1P_OPEN__			"L1_1P_OPEN"
#define __L2_OPEN__					"L2_OPEN"
#define __L3_OPEN__					"L3_OPEN"
#define __N_OPEN__					"N_OPEN"
#define __N_1P_OPEN__				"N_1P_OPEN"
#define __PE_OPEN__					"PE_OPEN"
#define __PE_1P_OPEN__			"PE_1P_OPEN"
#define __PASS__						"PASS"
#define __PASS_1P__					"PASS_1P"
#define __FAIL__						"FAIL"
#define __FAIL_1P__					"FAIL_1P"
#define __STOPED_C__				"STOPPED"
#define __1_PHASE__					"1_P"
#define __3_PHASE__					"3_P"

#define __MULTIFAULT__			"MULTIFLT"
#define __CROSSED_AND_SHORTED__	"CROS_N_SHOR"
#define __CROSSED_AND_OPEN__	"CROS_N_OPEN"
#define __SHORTED_AND_OPEN__	"SHOR_N_OPEN"
#define __MULTI_SHORTED__		"MULTI_SHOR"
#define __MULTI_CROSSED__		"MULTI_CROS"
#define __MULTI_OPEN__			"MULTI_OPEN"


#define __L1_L2_CROSSED__		"L1_L2_CROS"
#define __L1_L3_CROSSED__		"L1_L3_CROS"
#define __L2_L3_CROSSED__		"L2_L3_CROS"
#define __L1_N_CROSSED__		"L1_N_CROS"
#define __L1_PE_CROSSED__		"L1_PE_CROS"
#define __L2_N_CROSSED__		"L2_N_CROS"
#define __L2_PE_CROSSED__		"L2_PE_CROS"
#define __L3_N_CROSSED__		"L3_N_CROS"
#define __L3_PE_CROSSED__		"L3_PE_CROS"
#define __N_PE_CROSSED__		"N_PE_CROS"
#define __L2_L1_CROSSED__		"L2_L1_CROS"
#define __L3_L1_CROSSED__		"L3_L1_CROS"
#define __L3_L2_CROSSED__		"L3_L2_CROS"
#define __N_L1_CROSSED__		"N_L1_CROS"
#define __PE_L1_CROSSED__		"PE_L1_CROS"
#define __N_L2_CROSSED__		"N_L2_CROS"
#define __PE_L2_CROSSED__		"PE_L2_CROS"
#define __N_L3_CROSSED__		"N_L3_CROS"
#define __PE_L3_CROSSED__		"PE_L3_CROS"
#define __PE_N_CROSSED__		"PE_N_CROS"

#define __L1_L2_SHORTED__		"L1_L2_SHOR"
#define __L1_L3_SHORTED__		"L1_L3_SHOR"
#define __L2_L3_SHORTED__		"L2_L3_SHOR"
#define __L1_N_SHORTED__		"L1_N_SHOR"
#define __L1_PE_SHORTED__		"L1_PE_SHOR"
#define __L2_N_SHORTED__		"L2_N_SHOR"
#define __L2_PE_SHORTED__		"L2_PE_SHOR"
#define __L3_N_SHORTED__		"L3_N_SHOR"
#define __L3_PE_SHORTED__		"L3_PE_SHOR"
#define __N_PE_SHORTED__		"N_PE_SHOR"

#define __L1_L2_CRIS_CROSSED__		"L1_L2_CRIS"
#define __L1_L3_CRIS_CROSSED__		"L1_L3_CRIS"
#define __L2_L3_CRIS_CROSSED__		"L2_L3_CRIS"
#define __L1_N_CRIS_CROSSED__		"L1_N_CRIS"
#define __L1_PE_CRIS_CROSSED__		"L1_PE_CRIS"
#define __L2_N_CRIS_CROSSED__		"L2_N_CRIS"
#define __L2_PE_CRIS_CROSSED__		"L2_PE_CRIS"
#define __L3_N_CRIS_CROSSED__		"L3_N_CRIS"
#define __L3_PE_CRIS_CROSSED__		"L3_PE_CRIS"
#define __N_PE_CRIS_CROSSED__		"N_PE_CRIS"
//--PHASES TO PE
#define __START_PHASES_TO_PE__		"START_ALL-PE"
#define __PHASES_TO_PE_STARTED__	"ALL-PE_STARTED"
#define __RISO_PHASES_TO_PE__			"RISO_ALL-PE_RESULT"
//--ONE PHASE TO PE
#define __START_ONE_PHASE_TO_PE__	"START_ONE-PE"
#define __ONE_PHASE_TO_PE_STARTED__	"ONE-PE_STARTED"
#define __RISO_ONE_PHASE_TO_PE__	"RISO_ONE-PE_RESULT"
#define	__L1_PE_FAIL__						"L1-PE_F"
#define	__L2_PE_FAIL__						"L2-PE_F"
#define	__L3_PE_FAIL__						"L3-PE_F"
#define	__N_PE_FAIL__							"N-PE_F"
//--PHASE TO PHASE
#define __START_PHASE_TO_PHASE__	"START_PH_PH"
#define __PHASE_TO_PHASE_STARTED__	"PH_PH_STARTED"
#define __RISO_PHASE_TO_PHASE__		"RISO_PH-PH_RESULT"
#define __L1_N_FAIL__						"L1-N_F"
#define __L2_N_FAIL__						"L2-N_F"
#define __L3_N_FAIL__						"L3-N_F"
#define __L1_L3_FAIL__						"L1-L3_F"
#define __L1_L2_FAIL__						"L1-L2_F"
#define __L2_L3_FAIL__						"L2-L3_F"
//--CONTINUITY
#define __START_CONTINUITY__		"START_CONT"
#define __CONTINUITY_STARTED__	"CONT_STARTED"
#define __CORD_CONTINUITY__			"CONT"
#define __CORD_L1__					"L1"
#define __CORD_L2__					"L2"
#define __CORD_L3__					"L3"
#define __CORD_N__					"N"
#define __CORD_PE__					"PE"

//STATUS
//--additionalcode
#define __PHASE_S__					"PHASE_S"
#define __L_POS__						"L_POS"
#define __PHASE_A__					"PHASE_A"
#define __ULN1_OOR__				"ULN1_OOR"
#define __ILN2_OOR__				"ULN2_OOR"
#define __ILN3_OOR__				"ULN3_OOR"
#define __MAINS__						"MAINS"
//--value
#define __1P_SYS__					"1P_SYS"
#define __3P_SYS__					"3P_SYS"
#define __L__								"L"
#define __N__								"N"
#define __IN__PHASE__				"IN_PHASE"
#define __123__							"123"
#define __132__							"132"
#define __UNKNOWN__					"OUT_OF_PHASE"
#define __S_OK__						"OK"
#define __S_PE_DISC__				"PE_DISC"
#define __S_N_DISC__				"N_DISC"
#define __WRONG_CONN__			"WRONG_CON"
//--add
#define __MAINS_STAT__			"MAINS_STAT"

//+++++++++++++++MACH+++++++++++++++++
#define __MACHINES__					"MACH"
#define __INIT_MACHINES__			"INIT"
#define __MACH_INIT_FAILED__	"INIT_FAILED"
#define __DEINIT_MACHINES__		"DEINIT"
#define __MECH_RPE_START__		"START_RPE"
#define __MECH_RPE_STARTED__	"RPE_STARTED"
#define __MECH_RPE_STOP__			"STOP_RPE"
#define __MECH_RPE_STOPPED__	"RPE_STOPPED"
#define __MACH_URES_START__		"START_URES"
#define __MACH_URES_STARTED__ "URES_STARTED"
#define __MACH_URES_OPEN__		"URES_OPEN"
#define __MACH_URES_OPENED__	"URES_OPENED"
#define __STOP_MACH__					"STOP"
#define __MACH_INITIATED__		"INITIATED"
#define __MACH_TEST__					"TEST"
#define __MACH_URES_FINSHED__	"URES_FINISHED"
#define __MACH_URES_STOPPED__	"URES_STOPPED"
#define __MACH_L1_PE__				"L1-PE"
#define __MACH_L2_PE__				"L2-PE"
#define __MACH_L3_PE__				"L3-PE"
#define __MACH_L1_N__					"L1-N"
#define __MACH_L2_N__					"L2-N"
#define __MACH_L3_N__					"L3-N"
#define __MACH_L1_L2__				"L1-L2"
#define __MACH_L2_L3__				"L2-L3"
#define __MACH_L1_L3__				"L1-L3"
//+++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++WELDING+++++++++++++++++
#define __WELDING__					"WELD"
#define __INIT_WELDING__			"INIT"
#define __WELD_INIT_FAILED__	"INIT_FAILED"
#define __DEINIT_WELDING__		"DEINIT"
#define __WELD_RPE_START__		"START_RPE"
#define __WELD_RPE_STARTED__	"RPE_STARTED"
#define __WELD_RPE_STOP__			"STOP_RPE"
#define __WELD_RPE_STOPPED__	"RPE_STOPPED"
#define __WELD_URES_START__		"START_URES"
#define __WELD_URES_STARTED__ "URES_STARTED"
#define __WELD_URES_OPEN__		"URES_OPEN"
#define __WELD_URES_OPENED__	"URES_OPENED"
#define __STOP_WELD__					"STOP"
#define __WELD_INITIATED__		"INITIATED"
#define __WELD_TEST__					"TEST"
#define __WELD_URES_FINSHED__	"URES_FINISHED"
#define __WELD_URES_STOPPED__	"URES_STOPPED"
#define __WELD_L1_PE__				"L1-PE"
#define __WELD_L2_PE__				"L2-PE"
#define __WELD_L3_PE__				"L3-PE"
#define __WELD_L1_N__					"L1-N"
#define __WELD_L2_N__					"L2-N"
#define __WELD_L3_N__					"L3-N"
#define __WELD_L1_L2__				"L1-L2"
#define __WELD_L2_L3__				"L2-L3"
#define __WELD_L1_L3__				"L1-L3"
#define __MAINS_TO_WELD_START__	"START_MAINS-WELD"
#define __MAINS_TO_WELD_STARTED__	"MAINS-WELD_STARTED"
#define __MAINS_TO_WELD_RESULT__	"MAINS-WELD_RESULT"
#define __MAINS_TO_WELD_STOP__	"MAINS-WELD_STOP"
#define __MAINS_TO_WELD_STOPPED__	"MAINS-WELD_STOPPED"
#define __MAINS_TO_CLASS2_START__	"START_MAINS-CLASS2"
#define __MAINS_TO_CLASS2_STARTED__	"MAINS-CLASS2_STARTED"
#define __MAINS_TO_CLASS2_RESULT__	"MAINS-CLASS2_RESULT"
#define __MAINS_TO_CLASS2_STOP__	"MAINS-CLASS2_STOP"
#define __MAINS_TO_CLASS2_STOPPED__	"MAINS-CLASS2_STOPPED"
#define __WELD_TO_PE_START__	"START_WELD-PE"
#define __WELD_TO_PE_STARTED__	"WELD-PE_STARTED"
#define __WELD_TO_PE_RESULT__	"WELD-PE_RESULT"
#define __WELD_TO_PE_STOP__	"WELD-PE_STOP"
#define __WELD_TO_PE_STOPPED__	"WELD-PE_STOPPED"
#define __CONTINIOUS__				"CONT"
#define __SINGLE__						"SINGLE"
#define __WELD_UNL_RMS_START_		"START_UNL_RMS"
#define __WELD_UNL_RMS_STARTED_		"UNL_RMS_STARTED"
#define __WELD_UNL_RMS_STOP_		"STOP_UNL_RMS"
#define __WELD_UNL_RMS_STOPED_		"UNL_RMS_STOPPED"
#define __WELD_UNL_PEAK_START_		"START_UNL_PEAK"
#define __WELD_UNL_PEAK_STARTED_		"UNL_PEAK_STARTED"
#define __WELD_UNL_PEAK_STOP_		"STOP_UNL_PEAK"
#define __WELD_UNL_PEAK_STOPED_		"UNL_PEAK_STOPPED"
//++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++CALIBRATION++++++++++++++++++++
#define __MEASURE_ULN1__				"MEASURE_ULN1"
#define __MEASURE_ULN2__				"MEASURE_ULN2"
#define __MEASURE_ULN3__				"MEASURE_ULN3"
#define __MEASURE_IL1__					"MEASURE_IL1"
#define __MEASURE_IL2__					"MEASURE_IL2"
#define __MEASURE_IL3__					"MEASURE_IL3"
#define __CALIB_VOLTAGE__				__RETURN_VOLTAGE__
#define __CALIB_CURRENT__				__RETURN_CURRENT__
#define __CALIB_GET_CONSTANTS__	"GET_CONSTANTS"
#define __CONSTANTS__						"CONSTANTS"
#define __CALIB_ULN1__					"ULN1"
#define __CALIB_ULN2__					"ULN2"
#define __CALIB_ULN3__					"ULN3"
#define __CALIB_IL1__						"IL1"
#define __CALIB_IL2__						"IL2"
#define __CALIB_IL3__						"IL3"
#define __CALIB_ULN1K__					"ULN1_K"
#define __CALIB_ULN2K__					"ULN2_K"
#define __CALIB_ULN3K__					"ULN3_K"
#define __CALIB_ULN1N__					"ULN1_N"
#define __CALIB_ULN2N__					"ULN2_N"
#define __CALIB_ULN3N__					"ULN3_N"
#define __CALIB_IL1N__					"IL1_N"
#define __CALIB_IL2N__					"IL2_N"
#define __CALIB_IL3N__					"IL3_N"
#define __CALIB_IL1K__					"IL1_K"
#define __CALIB_IL2K__					"IL2_K"
#define __CALIB_IL3K__					"IL3_K"

#endif
