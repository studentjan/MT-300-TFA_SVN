#pragma once
#define RB_MAX_MESSAGES (10*MAX_MSG_IN_QUE)

class SerialWnd_RingBuffer
{
	public:
		SerialWnd_RingBuffer(void);

		CMsg msg_ringbuff[RB_MAX_MESSAGES];
		CMsg *first_full;
		CMsg *first_empty;

	private:

};



