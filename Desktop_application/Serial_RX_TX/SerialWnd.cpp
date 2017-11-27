//	SerialWnd.cpp - Implementation of the CSerialWnd class
//
//	Copyright (C) 1999-2003 Ramon de Klein (Ramon.de.Klein@ict.nl)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


//////////////////////////////////////////////////////////////////////
// Include the standard header files

#define STRICT
#include "StdAfx.h"
#include <crtdbg.h>
#include <tchar.h>
#include <vector>
#include <windows.h>
#include "Msg.h"
#include <string>
#include <algorithm>    // For std::remove()
#include <xstring>

//#include "IAP.h"
#include "RS485.h"
//#include "SelfTest.h"

//////////////////////////////////////////////////////////////////////
// Include module headerfile

#include "SerialWnd.h"
#include "SerialWnd_RingBuffer.h"

//////////////////////////////////////////////////////////////////////
class SerialWnd_RingBuffer *ringbufferA = new SerialWnd_RingBuffer();
class SerialWnd_RingBuffer *ringbufferB = new SerialWnd_RingBuffer();

//////////////////////////////////////////////////////////////////////
// Disable warning C4127: conditional expression is constant, which
// is generated when using the _RPTF and //_ASSERTE macros.
#pragma warning(disable: 4127)


//////////////////////////////////////////////////////////////////////
// Enable debug memory manager

#ifdef _DEBUG

#ifdef THIS_FILE
#undef THIS_FILE
#endif

static const char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW

#endif



//////////////////////////////////////////////////////////////////////
// Code

// Register the standard CSerialWnd COM message
const UINT CSerialWnd::mg_nDefaultComMsg = ::RegisterWindowMessage(_T("CSerialWnd_DefaultComMsg"));


CSerialWnd::CSerialWnd()
: m_hwndDest(0)
, m_nComMsg(WM_NULL)
, m_lParam(0)
{
}

CSerialWnd::~CSerialWnd()
{
  // Check if the thread handle is still there. If so, then we
  // didn't close the serial port. We cannot depend on the
  // CSerial destructor, because if it calls Close then it
  // won't call our overridden Close. //
  
  if (m_hThread)
  {
    // Display a warning
    //wprintf(L"CSerialWnd::~CSerialWnd - Serial port not closed\n");
    
    // Close implicitly
    Close();
  }
  
  
}

LONG CSerialWnd::Open (LPCTSTR lpszDevice, HWND hwndDest, UINT nComMsg, LPARAM lParam, DWORD dwInQueue, DWORD dwOutQueue)
{
  // Call the base class first
  long lLastError = CSerialEx::Open(lpszDevice,dwInQueue,dwOutQueue);
  if (lLastError != ERROR_SUCCESS)
    return lLastError;
  this->PortName = lpszDevice;
  // Save the window handle, notification message and user message
  m_hwndDest = hwndDest;
  m_nComMsg  = nComMsg?nComMsg:mg_nDefaultComMsg;
  m_lParam   = lParam;
  
  // Start the listener thread
  lLastError = StartListener();
  if (lLastError != ERROR_SUCCESS)
  {
    // Close the serial port
    Close();
    
    // Return the error-code
    m_lLastError = lLastError;
    return m_lLastError;
  }
  
  // Return the error
  m_lLastError = ERROR_SUCCESS;
  return m_lLastError;
}

LONG CSerialWnd::Close (void)
{
  // Reset all members
  m_hwndDest   = 0;
  m_nComMsg    = WM_NULL;
  
  // Call the base class
  return CSerialEx::Close();
}
template <typename T>
void move_range(size_t start, size_t length, size_t dst, std::vector<T> & v)
{
  const size_t final_dst = dst > start ? dst - length : dst;
  
  std::vector<T> tmp(v.begin() + start, v.begin() + start + length);
  v.erase(v.begin() + start, v.begin() + start + length);
  v.insert(v.begin() + final_dst, tmp.begin(), tmp.end());
}

//#define USB_BLUETOOTH
extern char lastSendData[255];
#define AB_BUFFER_SIZE	400
char  charbufferA[AB_BUFFER_SIZE] = {0};
char  charbufferB[AB_BUFFER_SIZE] = {0};
char tmBuffer[AB_BUFFER_SIZE];
char  partCharbuffer[AB_BUFFER_SIZE] = {0};

#define COMM_TERMINATOR	"\n"
#define COMM_BEGINNER	">"


std::string incompleteMessageA = "";
std::string incompleteMessageB = "";



void CSerialWnd::ParseMsg(CMsg obj[], int &msg_cntr, char buffer[], std::string &incomplete_msg, DWORD dwbytesread)
{
	std::string strmessage;
	std::string strmessage1(buffer);
	strmessage = strmessage1.substr(0, dwbytesread);


	//strmessage.erase(std::remove(strmessage.begin(),strmessage.end(),-36),strmessage.end());  ///Problem, ko laufata obe linije hkrati in ena slisi drugo
	std::vector<std::string> RecivedMessages;
	std::size_t prev_pos = 0,pos, begin_pos = 0;
	
	//OutputDebugString(L"-----------------------------------------------\n");

	while((pos = strmessage.find_first_of(COMM_TERMINATOR,prev_pos))!= std::string::npos) //dokler najdemo cel string
	{
	   RecivedMessages.push_back(incomplete_msg.append(strmessage.substr(prev_pos,pos-prev_pos)));
	   incomplete_msg.clear();
	   prev_pos = pos+1;
	}
	
	if (prev_pos < strmessage.length()) 
	{
		incomplete_msg.append(strmessage.substr(prev_pos, strmessage.length()-prev_pos));
	}


	for(std::vector<string>::iterator it = RecivedMessages.begin(); it!=RecivedMessages.end(); it++)
	{
		for (int i = 0; i < sizeof(obj[msg_cntr].m_message); i++)  //clear object message
			obj[msg_cntr].m_message[i] = 0x00;

		if (iap != NULL)
		{
			for (int i = 0; i < strlen(it->c_str()); i++)
				obj[msg_cntr].m_message[i] = (WCHAR)it->c_str()[i];
		}
 		else
		{
			obj[msg_cntr].ParseMessage(it->c_str());
		}
		/*WCHAR debug[200];
		wsprintf(debug, L"IZ PARSE MSG FUNKCIJE: %s\n", obj[msg_cntr].m_message);
		OutputDebugString(debug);*/

	    
		if (msg_cntr == MAX_MSG_IN_QUE-1)
		  break;
		(msg_cntr)++;
	} 
}

void CSerialWnd::OnEvent (EEvent eEvent, EError eError)
{
  #define MAX_MSG_IN_QUE	10

  CMsg messagesA[MAX_MSG_IN_QUE]; //najvec 10 sporocil v vrsti 
  CMsg messagesB[MAX_MSG_IN_QUE]; //najvec 10 sporocil v vrsti
  DWORD dwBytesReadA = 0;
  DWORD dwBytesReadB = 0;
  int MessagesCounterA  = 0;
  int MessagesCounterB  = 0;

/*  do
  {  */ 
	 if (wcscmp(this->PortName,L"COM1:") == 0)
	 {
		for(int a = 0; a < AB_BUFFER_SIZE; a++) charbufferA[a] = 0x00; 
		this->Read(charbufferA,sizeof(charbufferA),&dwBytesReadA);
		
		if (dwBytesReadA > 0)
		{
			ParseMsg(messagesA, MessagesCounterA, charbufferA, incompleteMessageA, dwBytesReadA);
			for(int i = 0; i < MessagesCounterA; i++)
			{
				if (messagesA[i].IsValid() == true)
					
				while ((ringbufferA->first_empty + 1 == ringbufferA->first_full) || ((ringbufferA->first_empty + 1 == &ringbufferA->msg_ringbuff[RB_MAX_MESSAGES] && ringbufferA->first_full == &ringbufferA->msg_ringbuff[0])))
				{
					//DEBUG OUT 
					Sleep(1); 
				}
				*ringbufferA->first_empty = messagesA[i];
				ringbufferA->first_empty++;

				if (ringbufferA->first_empty >= &ringbufferA->msg_ringbuff[RB_MAX_MESSAGES]) //ce je empty > max messages ga daj na 0 ce je frej (da ni full tam), drugace cakaj da ga thread obdela
					ringbufferA->first_empty = &ringbufferA->msg_ringbuff[0];
			}
		}
	 }
	 else if (wcscmp(this->PortName,L"COM2:") == 0)
	 {
		for(int b = 0; b < AB_BUFFER_SIZE; b++)  charbufferB[b] = 0x00;
		this->Read(charbufferB,sizeof(charbufferB),&dwBytesReadB);

		if (dwBytesReadB > 0)
		{	
			rs485->RS485_SetRS485_Received(true); //za connection status
			ParseMsg(messagesB, MessagesCounterB, charbufferB, incompleteMessageB, dwBytesReadB);
			for(int i = 0; i < MessagesCounterB; i++)
			{
				if (messagesB[i].IsValid() == true)		
				while ((ringbufferB->first_empty + 1 == ringbufferB->first_full) || ((ringbufferB->first_empty + 1 == &ringbufferB->msg_ringbuff[RB_MAX_MESSAGES] && ringbufferB->first_full == &ringbufferB->msg_ringbuff[0])))
				{
					//DEBUG OUT 
					Sleep(1);
				}
				*ringbufferB->first_empty = messagesB[i];
				ringbufferB->first_empty++;

				if (ringbufferB->first_empty >= &ringbufferB->msg_ringbuff[RB_MAX_MESSAGES]) //ce je empty > max messages ga daj na 0 ce je frej (da ni full tam), drugace cakaj da ga thread obdela
					ringbufferB->first_empty = &ringbufferB->msg_ringbuff[0];
			}

		}
	 }
  //}  //do end
  //while ((dwBytesReadA == sizeof(charbufferA)));
  //while ((dwBytesReadB == sizeof(charbufferB)));
  //while ((dwBytesReadA == sizeof(charbufferA)) || (dwBytesReadB == sizeof(charbufferB)));
}
