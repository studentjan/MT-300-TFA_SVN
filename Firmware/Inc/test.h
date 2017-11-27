//MT204PLUS MERILNA AMPROBE FLUKE
//programed by Janez Jazbar
//Last revision: 18.08.2016
//version _FW_VERSION
//uC: ST STM32F373VCT6 - MERILNA PCB
//asem.comp.link.: KEIL UVISION V5.15
//module: test.h

//define to prevent recursive inclusion
#ifndef __TEST_H
#define __TEST_H
#include "includes.h"

void test_task(void);
void Led_flash_task(void);

#endif // __TEST_H
