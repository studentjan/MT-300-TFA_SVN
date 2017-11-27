
class RS485
{
	public:
	RS485();
	void Init_RS485(void);
	void RS485_WritePack(char *buffer, int length);
	void RS485_Write(char *buffer);
	static void CALLBACK RS485_SendToAdapterControl(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2);
	char *RS485_GetTxBufferFirstSlot(void);
	void RS485_StartTimer(void);
	void RS485_StopTimer(void);
	bool RS485_GetTxInProgressStatus(void);
	void RS485_SetTxInProgressStatus(bool status);
	char *RS485_SendZero(void);
	bool RS485_GetSendIapPackFlag(void);
	void RS485_SetSendIapPackFlag(bool status);
	static void CALLBACK RS485_CheckAdapterConnection(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2);
	void RS485_StartTimer_TestConnection(void);
	void RS485_SetRS485_Received(bool state);
	static void CALLBACK SendData(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2);

	private:
	bool RS485_Send_IAP_Pack;
	bool RS485_TxInProgress;
	char RS485_TxBuffer[1300];
	int RS485_TimerID;
	int RS485_TimerCheckOutID; 
	bool RS485_Received;
	int RS485_ConnectionCntr;
	char RS485_SendZero_arr[2];
};