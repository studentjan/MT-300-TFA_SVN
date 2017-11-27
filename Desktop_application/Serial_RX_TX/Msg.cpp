#include "StdAfx.h"
#include "Msg.h"
#include "RS485.h"

CMsg::CMsg(void)
{
}

CMsg::~CMsg(void)
{
}
static const WCHAR FUNCTION_COMMUNICATON_NAMES[20];
static const WCHAR POWER_TYPE_NAMES[20];
static const WCHAR POWER_CODE_NAMES[20];
static const WCHAR RELAY_TYPE_NAMES[100];

class parse_command_names
{
public:
	static const WCHAR FUNCTION_COMMUNICATON_NAMES[MAX_VALID_FUNCTIONS][50];
	static const WCHAR COMMAND_COMMUNICATON_NAMES[MAX_VALID_COMMANDS][50];
	static const WCHAR POWER_TYPE_NAMES[MAX_VALID_TYPES][50];
	static const WCHAR POWER_CODE_NAMES[MAX_VALID_COMMANDS][50];
	static const WCHAR RELAY_TYPE_NAMES[MAX_VALID_TYPES][50];
};

//																		0           1           2           3           4           5           6           7           8           9           10          11          12          13          14          15          16          17          18          19          20          21          22          23          24          25          26          27          28          29          30          31          32          33          34          35          36       
const WCHAR parse_command_names::FUNCTION_COMMUNICATON_NAMES[MAX_VALID_FUNCTIONS][50] = { {L"POWER"}, {L"RELAY"} };
const WCHAR parse_command_names::COMMAND_COMMUNICATON_NAMES[MAX_VALID_COMMANDS][50] = { { L"ACK" },{ L"ERROR" },{ L"RESULT" },{ L"START" },{ L"GET" },{ L"EXIT" },{ L"VALUE" },{ L"EVENT" },{ L"STOP" },{ L"CALIBRATION" },{ L"INIT" },{ L"VERSION" },{ L"FAST" },{ L"AUTO_STARTED" },{ L"RESULTS" },{ L"FUNC" },{ L"TYPE" } ,{ L"POL" },{ L"CURRENT" },{ L"MEAS" },{ L"MODE" },{ L"RAMP" } ,{ L"STEP" },{ L"SHORT" },{ L"START_KEY_DOWN" },{ L"START_KEY_UP" },{ L"SAVE_KEY_DOWN" },{ L"SAVE_KEY_UP" },{ L"NEXT_KEY_DOWN" },{ L"NEXT_KEY_UP" },{ L"REMOTE_DISCONNECTED" },{ L"PARAMETERS" },{ L"1" },{ L"0" } }; //Always add At the end
const WCHAR parse_command_names::POWER_TYPE_NAMES[MAX_VALID_TYPES][50] = { L"POWER" };
const WCHAR parse_command_names::POWER_CODE_NAMES[MAX_VALID_COMMANDS][50] = { L"START", L"STOP", L"START_NTHD" };
const WCHAR parse_command_names::RELAY_TYPE_NAMES[MAX_VALID_TYPES][50] = { {L"RESET_ALL"},{ L"1_38_ON"},{ L"1_38_OFF" },   { L"2_ON" },   { L"2_OFF" }, { L"3_ON" }, { L"3_OFF" }, { L"4_ON" }, { L"4_OFF" }, { L"5_ON" }, { L"5_OFF" },
{ L"6_ON" },{ L"6_OFF" } ,{ L"7_ON" } ,    { L"7_OFF" },{ L"8_ON" },{ L"8_OFF" },{ L"9_ON" },{ L"9_OFF" },{ L"10_ON" },{ L"10_OFF" } ,{ L"11_ON" } ,
{ L"11_OFF" },{ L"12_ON" },{ L"12_OFF" },{ L"13_ON" },{ L"13_OFF" },{ L"14_ON" },{ L"14_OFF" },{ L"15_ON" },{ L"15_OFF" },{ L"16_ON" } ,{ L"16_OFF" },
{ L"17_ON" },{ L"17_OFF" } ,{ L"18_ON" } ,{ L"18_OFF" } ,{ L"19_ON" },{ L"19_OFF" },{ L"20_ON" },{ L"20_OFF" },{ L"21_43_ON" },{ L"21_43_OFF" },{ L"22_ON" },
{ L"22_OFF" },{ L"23_ON" } ,{ L"23_OFF" } ,{ L"24_ON" } ,{ L"24_OFF" },{ L"25_ON" },{ L"25_OFF" },{ L"26_ON" },{ L"26_OFF" } ,{ L"27_ON" },{ L"27_OFF" },
{ L"28_ON" },{ L"28_OFF" },{ L"29_ON" } ,{ L"29_OFF" },{ L"30_ON" },{ L"30_OFF" },{ L"31_ON" },{ L"31_OFF" },{ L"32_ON" },{ L"32_OFF" },{ L"33_ON" },
{ L"33_OFF" } ,{ L"34_ON" } ,{ L"34_OFF" },{ L"35_ON" },{ L"35_OFF" } ,{ L"36_ON" },{ L"36_OFF" },{ L"37_ON" },{ L"37_OFF" } ,{ L"1_38_ON" },{ L"1_38_OFF" },
{ L"39_ON" } ,{ L"39_OFF" } ,{ L"40_ON" },{ L"40_OFF" },{ L"41_ON" },{ L"41_OFF" } ,{ L"42_ON" },{ L"42_OFF" },{ L"21_43_ON" },{ L"21_43_OFF" } };

bool CMsg::ParseMessage(const char *message)
{

	bool retState = false;
	this->isLn = false;
	this->_ackCounter = 0;
	this->_resendCounter = 0;
	this->_discarded = false;

	this->_valid = false;
	this->_ack = false;
	this->_error = false;
	this->_result = false;
	this->_start = false;
	this->_get = false;
	this->_exit = false;
	this->_value = false;
	this->_event = false;
	this->_stop = false;
	this->_calibration = false;
	this->_init = false;
	this->_version = false;


	//7 podpicij plus zandnja cifra je dolžina msg-ja

	if (message == NULL)
	{
		return retState;
	}

	int podpi = 0;
	for (unsigned int a = 0; a < strlen(message); a++)
	{
		if (message[a] == ':') {
			delimiterArray[podpi] = a;
			podpi++;
		}
	}
	delimiterArray[podpi] = strlen(message);

	// Convert to a wchar_t*
	size_t origsize = strlen(message) + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, m_message, origsize, message, _TRUNCATE);

	wcscpy(m_start_tag, L"");
	wcscpy(m_msg_ID, L"");
	wcscpy(m_function, L"");
	wcscpy(m_command, L"");
	wcscpy(m_additional_code, L"");
	wcscpy(m_value, L"");
	wcscpy(m_is_crc, L"");
	wcscpy(m_crc_value, L"");
	wcscpy(m_leftover, L"");

	WCHAR temp[10];
	wcscpy(temp, L"");

	int delimiter_position = 0;
	for (int a = 0; a< (signed int)wcslen(m_message); a++) {
		if (a == delimiterArray[delimiter_position]) {
			delimiter_position++;
			continue;
		}
		//
		temp[0] = m_message[a];
		temp[1] = 0;

		if (delimiter_position == 0) {
			wcscat(m_start_tag, temp);
		}
		if (delimiter_position == 1) {
			wcscat(m_msg_ID, temp);
		}
		if (delimiter_position == 2) {
			wcscat(m_function, temp);
		}

		if (delimiter_position == 3) {
			wcscat(m_command, temp);
		}
		if (delimiter_position == 4) {
			wcscat(m_additional_code, temp);
		}
		if (delimiter_position == 5) {
			wcscat(m_value, temp);
		}
		if (delimiter_position == 6) {
			wcscat(m_is_crc, temp);
		}

		if (delimiter_position == 7) {
			wcscat(m_crc_value, temp);
		}

		if (delimiter_position == 8) {
			wcscat(m_leftover, temp);
		}
	}
	//check start string
	if (wcscmp(m_start_tag, IN_START_TAG) != 0) {
		//printf("Start TAG NOT OK");
		return retState;
	}

	SetID(_wtoi((WCHAR*)m_msg_ID));
	if (!(GetID() >= 0 && GetID() < 100)) {
		//printf("ID %d NOT OK",ID);
		return retState;
	}

	//check function string
	bool functionExists = false;
	for (int a = 0; a < MAX_VALID_FUNCTIONS; a++) {
		if (wcscmp(m_function, (const wchar_t *)parse_command_names::FUNCTION_COMMUNICATON_NAMES[a]) == 0) {
			functionExists = true;
			if (a == 0) {
			}
			else if (a == 1) {
			}
		}
	}

	if (!functionExists) {
		//printf("function Does not Exists. NOT OK");
		return retState;
	}

	//check function string
	bool commandExists = false;
	for (int a = 0; a < MAX_VALID_FUNCTIONS; a++) {
		if (wcscmp(m_command, (const wchar_t *)parse_command_names::COMMAND_COMMUNICATON_NAMES[a]) == 0) {
			commandExists = true;
			if (a == 0) {
				this->_ack = true;
			}
			else if (a == 1) {
				this->_error = true;
			}
			else if (a == 2) {
				this->_result = true;
			}
			else if (a == 3) {
				this->_start = true;
			}
			else if (a == 4) {
				this->_get = true;
			}
			else if (a == 5) {
				this->_exit = true;
			}
			else if (a == 6) {
				this->_value = true;
			}
			else if (a == 7) {
				this->_event = true;
			}
			else if (a == 8) {
				this->_stop = true;
			}
			else if (a == 9) {
				this->_calibration = true;
			}
			else if (a == 10) {
				this->_init = true;
			}
			else if (a == 11) {
				this->_version = true;
			}
			break;
		}
	}
	if (!commandExists) {
		//printf("function Does not Exists. NOT OK");
		return retState;
	}
	// we don't check additional code anymore, because it not well defined anymore
	int crc;
	int crcVal;
	crc = _wtoi((WCHAR*)m_is_crc);
	crcVal = _wtoi((WCHAR*)m_crc_value);
	if (crc == 1)
	{
		if (crcVal >= 0 && crcVal <= 255)
		{
			int crcRes = this->CalculateCRC();	   // racunamo crc do petega delimiterja
			if (crcRes == crcVal)
			{
				//printf("crc 0K");
				retState = true;
			}
			else
			{
				return retState;
			}
		}
		else
		{
			return retState;
		}
	}
	else
	{
		retState = true;
	}
	this->_valid = retState;
	return retState;
}

int CMsg::CalculateCRC()
{
	int delimiter_position = 0;
	int podpi = 0;
	for (unsigned int a = 0; a < wcslen(m_message); a++)
	{
		if (m_message[a] == ':') {
			delimiterArray[podpi] = a;
			podpi++;
		}

	}


	delimiterArray[podpi] = wcslen(m_message);
	int vsota = 0;

	for (signed int a = 0; (a < delimiterArray[6] + 1 && a < (signed int)wcslen(m_message)); a++)
	{
		vsota = vsota + (WCHAR)m_message[a];
	}

	return (vsota % 256);
}

byte CMsg::CalculateCRC2(char* message)
{
	int vsota = 0;

	for (unsigned int a = 0; a < strlen(message); a++)
	{
		vsota = vsota + (char)message[a];
	}

	return (vsota % 256);
}


CMsg::CMsg(bool direction, int ID, WCHAR* function, WCHAR* command, WCHAR* additionalCode, WCHAR* value, bool isCRC, bool needsToBeAnswered, int ack_delay, int sendingMsgDelay)
{
	bool retState = false;
	this->_valid = true;
	this->_ack = false;
	this->_version = false;
	this->isLn = false;
	this->_result = false;
	this->_error = false;
	this->_ackCounter = 0;
	this->_resendCounter = 0;
	this->_discarded = false;
	this->_first_inspection = false;
	this->_max_ack_delay = ack_delay;
	this->delay_for_send_message = sendingMsgDelay;

	for (int a = 0; a < NUMBER_OF_DELIMITERS; a++) {
		delimiterArray[a] = 0;
	}

	if (direction) {
		wcscpy(m_start_tag, OUT_START_TAG);
	}
	else {
		wcscpy(m_start_tag, IN_START_TAG);
	}
	WCHAR oo[10];
	wcscpy(oo, L"");
	wcscpy(oo, L"%2d");
	wsprintf((LPSTR)m_msg_ID, (LPCSTR)oo, ID);

	if (ID < 10)
	{
		m_msg_ID[0] = 48;
	}

	this->SetID(ID);
	wcscpy(m_function, function);
	this->_needsToBeAnswered = needsToBeAnswered;
	wcscpy(m_command, command);
	wcscpy(m_additional_code, additionalCode);
	wcscpy(m_value, value);
	wcscpy(m_leftover, END_TAG);
	wcscpy(m_is_crc, L"1");
	wcscpy(m_crc_value, L"000");
	if (isCRC) {
		wsprintf((LPSTR)m_message, (LPCSTR)COMAND_FORMAT, m_start_tag, m_msg_ID, m_function, m_command, m_additional_code, value, m_is_crc, m_crc_value, m_leftover);	 // for calculateing crc
		wcscpy(m_is_crc, L"1");
		int crcRes = this->CalculateCRC();
		WCHAR nn[10];
		wcscpy(nn, L"");
		wcscpy(nn, L"%3d");
		wsprintf((LPSTR)m_crc_value, (LPCSTR)nn, crcRes);
		if (crcRes < 100)
		{
			m_crc_value[0] = 48;
		}
		if (crcRes < 10)
		{
			m_crc_value[1] = 48;
		}
	}
	else {
		wcscpy(m_is_crc, L"0");
		wcscpy(m_crc_value, L"000");
	}


	wsprintf((LPSTR)m_message, (LPCSTR)COMAND_FORMAT, m_start_tag, m_msg_ID, m_function, m_command, m_additional_code, value, m_is_crc, m_crc_value, m_leftover);

	int wLen = wcslen(this->m_message);
	for (int a = 0; a< wLen; a++) {
		if (this->m_message[a]>0 && this->m_message[a]<255) {
			this->char_message[a] = static_cast<char>(this->m_message[a]);
		}
		else {
			//printf("ERROR converting WString to string");
		}
	}
	//this->char_message[wLen] = 10;
	//this->char_message[wLen+1] = 13;
	this->char_message[wLen] = 0;
}

//tega mislem da zaenkat ne rabmo
//void CMsg::SendToCOMPort1() {
//
//	this->_isAcknoledged = false;
//	this->_isAnswered = false;
//
//
//	port1->Write(this->char_message);
//}
//
//void CMsg::SendToCOMPort2() {
//
//	this->_isAcknoledged = false;
//	this->_isAnswered = false;
//
//	rs485->RS485_Write(this->char_message);
//}

float CMsg::GetResult()
{
	return (float)_wtof(this->m_value);
}