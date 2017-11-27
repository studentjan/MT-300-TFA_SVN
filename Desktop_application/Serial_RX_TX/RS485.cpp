#include "StdAfx.h"
#include "RS485.h"
#include "SelfTest.h"

#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "Serial.h"


RS485::RS485()
{
	RS485_TimerID = NULL;
	RS485_TimerCheckOutID  = NULL;
	RS485_Received = false;
	RS485_TxInProgress = false;
	memset(RS485_TxBuffer, 0, sizeof(RS485_TxBuffer));
	RS485_SendZero_arr[0] = 0;
	RS485_SendZero_arr[1] = '\n';
	RS485_Send_IAP_Pack = false;
}

void RS485::Init_RS485(void)
{
	port2->SetMask(CSerial::EEventRcvEv);
	port2->SetEventChar('\n',false);
	port2->Open(_T("COM2:"),0,0,false);
	port2->Setup(CSerial::EBaud115200,CSerial::EData8,CSerial::EParNone,CSerial::EStop1); 
	port2->SetupHandshaking(CSerial::EHandshakeOff);
}

void RS485::RS485_WritePack(char *buffer, int length)
{
	for (int i = 0; i < length; i++)
		RS485_TxBuffer[i] = buffer[i];
}

void RS485::RS485_Write(char *buffer) 
{
	int i = 0;
	while(buffer[i] != '\0')
	{
		RS485_TxBuffer[i] = buffer[i];
		i++;
	}
	RS485_TxBuffer[i] = '\0';
}

void CALLBACK RS485::RS485_SendToAdapterControl(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	Gpio_SetLevel(HardKeys::gpioHandle, HardKeys::RS485_DE, ioHigh);
	while (Gpio_GetLevel(HardKeys::gpioHandle, HardKeys::RS485_DE) != ioHigh);
	port2->Write(rs485->RS485_SendZero(), 2);
	Gpio_SetLevel(HardKeys::gpioHandle, HardKeys::RS485_DE, ioLow);
}

char *RS485::RS485_GetTxBufferFirstSlot(void)
{
	return RS485_TxBuffer;
}

void RS485::RS485_StartTimer(void)
{
	timeKillEvent(RS485_TimerID);
	RS485_TimerID = timeSetEvent(1, 1, RS485_SendToAdapterControl, NULL, TIME_ONESHOT); 
}

void RS485::RS485_StopTimer(void)
{
	timeKillEvent(RS485_TimerID);
}

bool RS485::RS485_GetTxInProgressStatus(void)
{
	return RS485_TxInProgress;
}

void RS485::RS485_SetTxInProgressStatus(bool status)
{
	RS485_TxInProgress = status;
}

char *RS485::RS485_SendZero(void)
{
	return RS485_SendZero_arr;
}


bool RS485::RS485_GetSendIapPackFlag(void)
{
	return RS485_Send_IAP_Pack;
}

void RS485::RS485_SetSendIapPackFlag(bool status)
{
	RS485_Send_IAP_Pack = status;
}

void CALLBACK RS485::RS485_CheckAdapterConnection(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	if (rs485->RS485_Received == true)
	{
		selftest->SetAdapterControl_DeviceCheck(true);
		rs485->RS485_Received = false;
	}
	else
	{
		selftest->SetAdapterControl_DeviceCheck(false);
	}
}

void RS485::RS485_StartTimer_TestConnection(void)
{
	timeKillEvent(RS485_TimerCheckOutID);
	RS485_TimerCheckOutID = timeSetEvent(500, 1, RS485_CheckAdapterConnection, NULL, TIME_PERIODIC); 
}

void RS485::RS485_SetRS485_Received(bool state)
{
	RS485_Received = state; 
}

void CALLBACK RS485::SendData(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	Gpio_SetLevel(HardKeys::gpioHandle, HardKeys::RS485_DE, ioHigh);
	while (Gpio_GetLevel(HardKeys::gpioHandle, HardKeys::RS485_DE) != ioHigh);

	if (rs485->RS485_GetSendIapPackFlag() == true)
	{
		port2->Write(rs485->RS485_TxBuffer, 1029);
		rs485->RS485_TxBuffer[0] = '\0';
		rs485->RS485_SetSendIapPackFlag(false);
	}
	else if (rs485->RS485_TxBuffer[0] != '\0')
	{	
		port2->Write(rs485->RS485_TxBuffer);
		rs485->RS485_TxBuffer[0] = '\0';
	}
	else if (iap == NULL)
	{
		port2->Write(rs485->RS485_SendZero(), 2);
	}
	Gpio_SetLevel(HardKeys::gpioHandle, HardKeys::RS485_DE, ioLow);
}
