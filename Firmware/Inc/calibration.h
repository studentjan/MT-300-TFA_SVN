//define to prevent recursive inclusion
#ifndef __CALIBRATION_H
#define __CALIBRATION_H

#define CALIB_MEAS_TIMEOUT				100


void calibMeasure(void);
void transmittCalibConstants(void);
//---------------------------------zastavice za calib_task_control------------------------
#define __CALIB_ULN1_REQUESTED				0x00000001
#define __CALIB_ULN2_REQUESTED				0x00000002
#define __CALIB_ULN3_REQUESTED				0x00000004
#define __CALIB_IL1_REQUESTED					0x00000008
#define __CALIB_IL2_REQUESTED					0x00000010
#define __CALIB_IL3_REQUESTED					0x00000020


#define __CALIB_REQUESTED_MASKS				0x0000003F	//tuki so cist vse requested maske
#define __CALIB_REQ_VOLTAGE_MASKS			0x00000007	//vsi requesti napetosti
#define __CALIB_REQ_CURRENT_MASKS			0x00000038	//vsi requesti tokov
#endif