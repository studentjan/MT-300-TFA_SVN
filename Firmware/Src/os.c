//FULLTEST MERILNA HTITALIA
//programed by Janez Jazbar
//Last revision: 05.02.2013
//version _FW_VERSION
//uC: NXP LPC1756 - MERILNA PCB
//asem.comp.link.: IAR ARM 6.30
//CMSIS-CM3 library
//module: os.c

//#include "includes.h"
#include "os.h"
#include <stdint.h>
#include "defines.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx.h"
#include "tasks.h"

uint32_t CurrentWorkingOSEvent = 0xFFFFFFFF;
uint8_t OS_SET_OUT_Priority = _NO;

typedef struct
{
 uint32_t event_ID;
 void (*event_fp)(void);    //pointer to time_out_fp()
} event_t;

typedef struct
{
 uint32_t timer_ID;        	//timer ID Number
 uint32_t timer;            //timer count value
 void (*time_out_fp)(void); //pointer to time_out_fp()
} timer_t;


uint32_t TimerTick;
event_t event_Q[MAX_EVENTS];          //array of events
timer_t ms100_timer[MAX_100MS_TIMER]; //array of timers
uint32_t os_test;

//init operating system
void init_OS(void)
{  	
	TimerTick = 0;
  init_timers();  //initialise the timers
  init_event_Q(); //initialise the event Q
  
  //10ms timer tick	
	if (SysTick_Config(SystemCoreClock / 100))
  {     
   while (1); //Capture error
  }
}

//run main OS loop
void run_OS(void)
{
 while(1)
  {
          
   #ifdef _WDT_defined
    wdt_state=0x55555555;
    wdt_a();
   #endif
   			
   service_timers();
   do_events();
   
   #ifdef _WDT_defined
    wdt_state+=0x22222222;
    wdt_b();
   #endif
   
   //go to sleep mode and wait for wake up from interrupt
   //CLKPWR_Sleep();         
  }
}

//init event queue
void init_event_Q(void)
{
 unsigned int e_cnt;
 
 e_cnt=0;
 while(e_cnt<MAX_EVENTS)
  {
   event_Q[e_cnt].event_ID=FREE_EVENT;
   e_cnt++;
  }
}

//main events loop
void do_events(void)
{
 unsigned int e_cnt;
   
 
// e_cnt=0;
// if(OS_SET_OUT_Priority==_YES)
//  {
//   OS_SET_OUT_Priority=_NO;
//   for(e_cnt=0;e_cnt<MAX_EVENTS;e_cnt++)
//    {
//     if((event_Q[e_cnt].event_ID==OS_R3_INT_TASK) ||  (event_Q[e_cnt].event_ID==OS_R2_INT_TASK))
//      {
//       event_Q[e_cnt].event_ID=FREE_EVENT;  
//       (*event_Q[e_cnt].event_fp)();    
//      }
//    }
//  }
   
 e_cnt=0;
 
  while(e_cnt<MAX_EVENTS)
  {
    if(event_Q[e_cnt].event_ID!=FREE_EVENT)
    {
     CurrentWorkingOSEvent = e_cnt;      
     (*event_Q[e_cnt].event_fp)();    
     event_Q[e_cnt].event_ID=FREE_EVENT;   //free the event space
		 //os_test = e_cnt;
    }
  e_cnt++;
  }  
}

//set event
//najde prvi prosti slot
//ne preverja polno zasedenega bufferja
char set_event(unsigned int e_ID,void (*e_fp)(void))
{
  unsigned int e_cnt;
  unsigned char status;
  status=EVENT_FAIL;
  e_cnt=0;
  while((e_cnt<MAX_EVENTS) && (status==EVENT_FAIL))
  {
    if(event_Q[e_cnt].event_ID==FREE_EVENT && e_cnt!=CurrentWorkingOSEvent)
    {
      event_Q[e_cnt].event_ID=e_ID;
      event_Q[e_cnt].event_fp=e_fp;
      status=EVENT_STARTED;
    }
    e_cnt++;
  }
  return(status);
}

//free event from queue
//vedno vrne 1
//ne preverja, ce ni eventa v queue
char free_event(unsigned int e_ID)
{
 unsigned int e_cnt;
 e_cnt=0;
 while(e_cnt<MAX_EVENTS)
  {
   if(event_Q[e_cnt].event_ID==e_ID)
    {
     event_Q[e_cnt].event_ID=FREE_EVENT;
    }
  e_cnt++;
  }
 return(EVENT_STARTED);
}


//init - clear all timers
void init_timers(void)
{
 unsigned int tmr_cnt;
 
 tmr_cnt=0;
 while(tmr_cnt<MAX_100MS_TIMER)
  {
   ms100_timer[tmr_cnt].timer_ID=FREE_TIMER;
   tmr_cnt++;
  }
}

//service timers 
void service_timers(void)
{  
  unsigned int tmr_cnt = 0;;  
  if(TimerTick>0)
  {            
    TimerTick = 0;
    while(tmr_cnt<MAX_100MS_TIMER)
    {
      if(ms100_timer[tmr_cnt].timer_ID>FREE_TIMER)
      {
        if(ms100_timer[tmr_cnt].timer) ms100_timer[tmr_cnt].timer--;
        if(ms100_timer[tmr_cnt].timer==0)   //timer just expired
        {
          set_event(ms100_timer[tmr_cnt].timer_ID, ms100_timer[tmr_cnt].time_out_fp ); //place function into the event queue
          ms100_timer[tmr_cnt].timer_ID=FREE_TIMER;
        }
      }
      tmr_cnt++;
    }       
  }
}

//set timer
//najde prvi prosti slot za nov timer
//ne preverja polne zasedenosti queue
char set_timer(unsigned int t_ID,unsigned int t_cnt,void (* t_fp)(void))
{
 unsigned int tmr_cnt;
 unsigned char status;
 status=TMR_FAIL;
 tmr_cnt=0;
// if (CanBeepError == t_ID)
//   return 0;
 while((tmr_cnt<MAX_100MS_TIMER) && (status == TMR_FAIL))
  {
   if(ms100_timer[tmr_cnt].timer_ID==FREE_TIMER)
    {
     ms100_timer[tmr_cnt].timer_ID=t_ID;
     ms100_timer[tmr_cnt].timer=t_cnt;
     ms100_timer[tmr_cnt].time_out_fp=t_fp;
     status=TMR_STARTED;
    }
   tmr_cnt++;
  }
 return (status);
}


//restart timer
//ce je ta timer id ze v vrsti ga restarta z novim casom in novo funkcijo
//ce ga ni v vrsti, potem doda novega
//ne preverja polne zasedenosti timer queue
char restart_timer(unsigned int t_ID,unsigned int t_cnt,void (* t_fp)(void))
{
 unsigned int tmr_cnt;
 unsigned int tmp_free_ID;
 unsigned char status;
 
 status=TMR_FAIL;
 tmr_cnt=0;
 tmp_free_ID=MAX_100MS_TIMER + 1;

 while((tmr_cnt<MAX_100MS_TIMER) && (status==TMR_FAIL))
  {
   if(ms100_timer[tmr_cnt].timer_ID==FREE_TIMER) tmp_free_ID=tmr_cnt;  //remember a free timer slot      
   
   if(ms100_timer[tmr_cnt].timer_ID==t_ID )
    {
     ms100_timer[tmr_cnt].timer=t_cnt;
     ms100_timer[tmr_cnt].time_out_fp=t_fp;
     status=TMR_STARTED;
    }
   
   tmr_cnt++;
  }

 //new slot
 if((status==TMR_FAIL) && (tmp_free_ID<MAX_100MS_TIMER))
  {
   ms100_timer[tmp_free_ID].timer_ID=t_ID;
   ms100_timer[tmp_free_ID].timer=t_cnt;
   ms100_timer[tmp_free_ID].time_out_fp=t_fp;
   status=TMR_STARTED;
  }
 return(status);
}

//free timer
char free_timer(unsigned int t_ID)
{
 unsigned int tmr_cnt;
 unsigned char status;
 tmr_cnt=0;
 while(tmr_cnt<MAX_100MS_TIMER)
  {
   if(ms100_timer[tmr_cnt].timer_ID==t_ID)
    {
     ms100_timer[tmr_cnt].timer_ID=FREE_TIMER;
     status=TMR_FAIL;
    }
   tmr_cnt++;
  }
 return (status);
}


//end task
char end_task(unsigned int t_ID)
{
 free_timer(t_ID);
 free_event(t_ID);
 return (1);
}

