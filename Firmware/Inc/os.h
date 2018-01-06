//FULLTEST MERILNA HTITALIA
//programed by Janez Jazbar
//Last revision: 05.02.2013
//version _FW_VERSION
//uC: NXP LPC1756 - MERILNA PCB
//asem.comp.link.: IAR ARM 6.30
//CMSIS-CM3 library
//module: os.h

//define to prevent recursive inclusion
#ifndef __OS_H
#define __OS_H

//#include "includes.h"

#define MAX_EVENTS         30  //30
#define FREE_EVENT          0
#define EVENT_FAIL          0
#define EVENT_STARTED       1

//timer defines
#define MAX_100MS_TIMER    30
#define FREE_TIMER          0
#define TMR_FAIL            0
#define TMR_STARTED         1


void init_OS(void);
void init_event_Q(void);
void init_timers(void);
void do_events(void);
void service_timers(void);
void run_OS(void);
char set_event(unsigned int e_ID,void (*e_fp)(void));
char set_timer(unsigned int t_ID,unsigned int t_cnt,void (* t_fp)(void));
char restart_timer(unsigned int t_ID,unsigned int t_cnt,void (* t_fp)(void));
char free_timer(unsigned int t_ID);
char end_task(unsigned int t_ID);

#endif // __OS_H
