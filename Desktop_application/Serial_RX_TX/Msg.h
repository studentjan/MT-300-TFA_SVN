#pragma once

#include "StdAfx.h"


#ifndef __MESSAGE_
#define __MESSAGE_


#define MSG_START_TAG			'>'
#define MSG_BREAK_TAG			':'
#define MSG_ADDCODE_BREAK_TAG	'|'


#define IN_START_TAG			L">MP"
#define OUT_START_TAG			L">PM"
#define END_TAG					L"\n" //L"#013#010"


#define MAX_MSG_IN_QUE			10
#define MAX_VALID_TAGS			8
#define MAX_VALID_FUNCTIONS		35
#define MAX_VALID_TYPES			100 
#define MAX_VALID_COMMANDS		35
#define NUMBER_OF_DELIMITERS	9

#define MAX_MSG_RESENDS			2
#define FAST_RESPONSE_TIME					100
#define NORMAL_RESPONSE_TIME				300
#define SLOW_RESPONSE_TIME					500
#define EXTRA_SLOW_RESPONSE_TIME			2000
#define MSG_RPEAT_DELAY_INTERVAL			500 // ms



const WCHAR COMAND_FORMAT[] = L"%s:%s:%s:%s:%s:%s:%s:%s:%s";

class CMsg
{

private:
	byte CRC;

	bool _valid;
	bool _ack;
	bool _error;
	bool _result;
	bool _start;
	bool _get;
	bool _exit;
	bool _value;
	bool _event;
	bool _stop;
	bool _calibration;
	bool _init;
	bool _version;

	bool _isAcknoledged;
	bool _isAnswered;
	int _resendCounter;
	int _ackCounter;
	bool _needsToBeAnswered;
	bool _discarded;
	bool _first_inspection;
	int	_max_ack_delay;
	int delay_for_send_message;


public:
	int ID;

	char char_message[200];
	WCHAR m_message[200];
	WCHAR m_start_tag[4];
	WCHAR m_msg_ID[3];
	WCHAR m_function[31];
	WCHAR m_command[31];
	WCHAR m_additional_code[150];
	WCHAR m_value[51];
	WCHAR m_is_crc[2];
	WCHAR m_crc_value[4];
	WCHAR m_leftover[10];

	int int_additional_code;

	int  delimiterArray[NUMBER_OF_DELIMITERS];
	int CalculateCRC();
	byte CalculateCRC2(char* message);
	bool isLn;


	CMsg(void);

	CMsg(bool direction, int ID, WCHAR* function, WCHAR* command, WCHAR* additionalCode, WCHAR* value, bool isCRC, bool needsToBeAnswered, int	ack_delay, int sendingMsgDelay);
	void SendToCOMPort1();
	void SendToCOMPort2();
	~CMsg(void);
	bool ParseMessage(const char *message);



	void  SetID(int id)
	{
		this->ID = id;
	}
	int GetID()
	{
		return this->ID;
	}




	void  SetMax_ack_delay(int ack_delay)
	{
		this->_max_ack_delay = ack_delay;
	}
	int GetMax_ack_delay()
	{
		return this->_max_ack_delay;
	}



	void  SetResendCounter(int rst_c)
	{
		this->_resendCounter = rst_c;
	}
	int GetResendCounter()
	{
		return this->_resendCounter;
	}


	void  SetAckCounter(int ack_c)
	{
		this->_ackCounter = ack_c;
	}
	int GetAckCounter()
	{
		return this->_ackCounter;
	}

	bool IsAnswered()
	{
		return this->_isAnswered;
	}
	void SetAnswered(bool ack)
	{
		this->_isAnswered = ack;
	}

	bool IsAcknowledged()
	{
		return this->_isAcknoledged;
	}
	void SetAcknowledged(bool answ)
	{
		this->_isAcknoledged = answ;
	}



	bool IsDiscarded()
	{
		return this->_discarded;
	}
	void SetDiscarded(bool ack)
	{
		this->_discarded = ack;
	}


	bool IsFirst_inspection()
	{
		return this->_first_inspection;
	}
	void SetFirst_inspection(bool ack)
	{
		this->_first_inspection = ack;
	}

	bool IsValid()
	{
		return this->_valid;
	}


	bool IsError()
	{
		return this->_error;
	}

	bool IsResult()
	{
		return this->_result;
	}
	bool IsAck()
	{
		return this->_ack;
	}

	void SetAck(bool ack)
	{
		this->_ack = ack;
	}

	bool IsNAck()
	{
		return !this->_ack;
	}
	bool IsStart()
	{
		return this->_start;
	}
	bool IsGet()
	{
		return this->_get;
	}
	bool IsExit()
	{
		return this->_exit;
	}
	bool IsValue()
	{
		return this->_value;
	}
	bool IsEvent()
	{
		return this->_event;
	}
	bool IsStop()
	{
		return this->_stop;
	}


	bool IsCalibration()
	{
		return this->_calibration;
	}
	bool IsInit()
	{
		return this->_init;
	}
	bool IsVersion()
	{
		return this->_version;
	}

	float GetResult();


	WCHAR* GetResultText()
	{
		return this->m_value;
	}

	WCHAR* GetMessageResult()
	{
		return this->m_message;
	}

	WCHAR* GetCommand()
	{
		return this->m_command;
	}

	WCHAR* GetAdditionalCommand()
	{
		return this->m_additional_code;
	}


	void  SetDelay_for_send_message(int delay)
	{
		this->delay_for_send_message = delay;
	}
	int GetDelay_for_send_message()
	{
		return this->delay_for_send_message;
	}


};


#endif