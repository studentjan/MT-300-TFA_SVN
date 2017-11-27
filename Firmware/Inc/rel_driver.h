  //******************************************************************************
  //* File Name          : rel_driver.h
  //* Description        : Za rele driver TPL9201PWPR
  //* Created			 : Jan Koprivec
  //* Date				 : 8.9.2017
  //******************************************************************************
  
#ifndef __REL_DRIVER_H
#define __REL_DRIVER_H
#include <stdint.h>

void init_REL(void);
void reset_all_REL(void);
uint8_t set_REL(uint8_t rel_nr);
uint8_t rst_REL(uint8_t rel_nr);


#endif
