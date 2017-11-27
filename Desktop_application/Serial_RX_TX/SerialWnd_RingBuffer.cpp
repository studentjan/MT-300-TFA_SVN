#include "StdAfx.h"
#include "SerialWnd_RingBuffer.h"


SerialWnd_RingBuffer::SerialWnd_RingBuffer(void)
{
	first_empty = &msg_ringbuff[0];
	first_full = &msg_ringbuff[0];
}




