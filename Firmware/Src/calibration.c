#include "calibration.h"
#include <stdint.h>
#include "do_task.h"
#include "defines.h"
#include "os.h"
#include "tasks.h"
#include "sdadc.h"
#include "serial_com.h"
#include <stdio.h>
#include "stm32f3xx_hal.h"


static void transmittRequestedResult(void);

extern uint32_t meas_task_control;
uint32_t calib_task_control = 0;
uint32_t calibMeasCount =0;
static struct connected_device device;

void calibMeasure(void)
{
	switch(calibMeasCount)
	{
		case 0:
			device = get_connected_device();
			meas_task_control |=  __CALIB_MEAS_IN_PROG;
			start_measure_no_THD();
			calibMeasCount++;
			break;
		case 1:
			transmittRequestedResult();
			stop_measure();
			meas_task_control &= ~__CALIB_MEAS_IN_PROG;
			calibMeasCount++;
			break;
		default:
			break;
	}
	if(calibMeasCount<2)
		restart_timer(CALIBRATION_MEAS_TASK,CALIB_MEAS_TIMEOUT, calibMeasure);
	else
	{
		calibMeasCount=0;
		end_task(CALIBRATION_MEAS_TASK);
	}
}

static void transmittRequestedResult(void)
{
	float temp_float;
	char temp_array[20];
	if(calib_task_control & __CALIB_REQ_VOLTAGE_MASKS)
	{
		switch(calib_task_control & __CALIB_REQUESTED_MASKS)
		{
			case __CALIB_ULN1_REQUESTED:
				calib_task_control &= ~__CALIB_ULN1_REQUESTED;
				temp_float = get_value(__ULN1);
				snprintf(temp_array,15,"%s|%.3f",__CALIB_ULN1__,temp_float);
				break;
			case __CALIB_ULN2_REQUESTED:
				calib_task_control &= ~__CALIB_ULN2_REQUESTED;
				temp_float = get_value(__ULN2);
				snprintf(temp_array,15,"%s|%.3f",__CALIB_ULN2__,temp_float);
				break;
			case __CALIB_ULN3_REQUESTED:
				calib_task_control &= ~__CALIB_ULN3_REQUESTED;
				temp_float = get_value(__ULN3);
				snprintf(temp_array,15,"%s|%.3f",__CALIB_ULN3__,temp_float);
				break;
			case __CALIB_UL1PE_REQUESTED:
				calib_task_control &= ~__CALIB_UL1PE_REQUESTED;
				temp_float = get_value(__UL1PE);
				snprintf(temp_array,15,"%s|%.3f",__CALIB_UL1PE__,temp_float);
				break;
			case __CALIB_UNPE_REQUESTED:
				calib_task_control &= ~__CALIB_UNPE_REQUESTED;
				temp_float = get_value(__UNPE);
				snprintf(temp_array,15,"%s|%.3f",__CALIB_UNPE__,temp_float);
				break;
			default:
				snprintf(temp_array,15,"NOT_SUPPORTED");
				calib_task_control &= ~__CALIB_REQUESTED_MASKS;
		}
		SendComMessage(_ON,_ID_TFA,device.device_ID,__CALIB__,__CALIB_VOLTAGE__,temp_array,"",device.device_dir);
	}
	else if(calib_task_control & __CALIB_REQ_CURRENT_MASKS)
	{
		switch(calib_task_control & __CALIB_REQUESTED_MASKS)
		{
			case __CALIB_IL1_REQUESTED:
				calib_task_control &= ~__CALIB_IL1_REQUESTED;
				temp_float = get_value(__IL1);
				snprintf(temp_array,15,"%s|%.3f",__CALIB_IL1__,temp_float);
				break;
			case __CALIB_IL2_REQUESTED:
				calib_task_control &= ~__CALIB_IL2_REQUESTED;
				temp_float = get_value(__IL2);
				snprintf(temp_array,15,"%s|%.3f",__CALIB_IL2__,temp_float);
				break;
			case __CALIB_IL3_REQUESTED:
				calib_task_control &= ~__CALIB_IL3_REQUESTED;
				temp_float = get_value(__IL3);
				snprintf(temp_array,15,"%s|%.3f",__CALIB_IL3__,temp_float);
				break;
			default:
				snprintf(temp_array,15,"NOT_SUPPORTED");
				calib_task_control &= ~__CALIB_REQUESTED_MASKS;
		}
		SendComMessage(_ON,_ID_TFA,device.device_ID,__CALIB__,__CALIB_CURRENT__,temp_array,"",device.device_dir);
	}
	else
		SendComMessage(_ON,_ID_TFA,device.device_ID,__CALIB__,"NOT_SUPPORTED","NOT_SUPPORTED","",device.device_dir);
}

void transmittCalibConstants(void)
{
	char temp_str[110];
	snprintf(temp_str,110,"%s|%.8f,%s|%.8f,%s|%.8f,%s|%.8f,%s|%.8f,%s|%.8f,%s|%.8f,%s|%.8f,%s|%.8f,%s|%.8f",__CALIB_ULN1K__,get_value(__ULN1_K),__CALIB_ULN2K__,get_value(__ULN2_K),__CALIB_ULN3K__,get_value(__ULN3_K),__CALIB_ULN1N__,get_value(__ULN1_N),__CALIB_ULN2N__,get_value(__ULN2_N),__CALIB_ULN3N__,get_value(__ULN3_N),__CALIB_UL1PEK__,get_value(__UL1PE_K),__CALIB_UL1PEN__,get_value(__UL1PE_N),__CALIB_UNPEK__,get_value(__UNPE_K),__CALIB_UNPEN__,get_value(__UNPE_N));
	SendComMessage(_ON,_ID_TFA,device.device_ID,__CALIB__,__CONSTANTS__,temp_str,"",device.device_dir);
	snprintf(temp_str,110,"%s|%.8f,%s|%.8f,%s|%.8f,%s|%.8f",__CALIB_UL1PEK__,get_value(__UL1PE_K),__CALIB_UL1PEN__,get_value(__UL1PE_N),__CALIB_UNPEK__,get_value(__UNPE_K),__CALIB_UNPEN__,get_value(__UNPE_N));
	SendComMessage(_ON,_ID_TFA,device.device_ID,__CALIB__,__CONSTANTS__,temp_str,"",device.device_dir);
//	snprintf(temp_str,90,"%s|%.8f,%s|%.8f,%s|%.8f,%s|%.8f,%s|%.8f,%s|%.8f",__CALIB_IL1K__,get_value(__IL1_K),__CALIB_IL2K__,get_value(__IL2_K),__CALIB_IL3K__,get_value(__IL3_K),__CALIB_IL1N__,get_value(__IL1_N),__CALIB_IL2N__,get_value(__IL2_N),__CALIB_IL3N__,get_value(__IL3_N));
//	SendComMessage(_ON,_ID_TFA,device.device_ID,__CALIB__,__CONSTANTS__,temp_str,"",device.device_dir);
}

//void Write_Flash(uint8_t data)
//{
//     HAL_FLASH_Unlock();
//     __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP |  FLASH_FLAG_WRPERR | FLASH_FLAG_PGERR );
//     FLASH_Erase_Sector(FLASH_SECTOR_6, VOLTAGE_RANGE_3);
//     HAL_FLASH_Program(TYPEPROGRAM_WORD, FlashAddress, data);
//     HAL_FLASH_Lock();
//}

