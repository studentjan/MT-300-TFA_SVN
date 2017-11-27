//MT204PLUS MERILNA AMPROBE FLUKE
//programed by Janez Jazbar
//Last revision: 18.08.2016
//version _FW_VERSION
//uC: ST STM32F373VCT6 - MERILNA PCB
//asem.comp.link.: KEIL UVISION V5.15
//module: test.c
#include "test.h"
#include "os.h"
#include "tasks.h"
#include "sdadc.h"
#include "usbd_cdc_if.h"
#include <stdint.h>
	
	uint32_t glob_test1;
	uint32_t glob_test2;
	uint32_t glob_test3;
	uint32_t glob_test4;
	uint32_t glob_test5;
	uint32_t led_blink_time;

void test_task(void)
{
	//set_event(OS_TEST,test_task);
	restart_timer(OS_TEST,10,test_task);
}	

void Led_flash_task(void)
{   
 static uint8_t led_state=0;
 set_timer(TEST_LED,led_blink_time,Led_flash_task);

 if(led_state==0)
	 {LED_TEST_on;led_state=1;}
 else
  {LED_TEST_off;led_state=0;} 		
		
} 


