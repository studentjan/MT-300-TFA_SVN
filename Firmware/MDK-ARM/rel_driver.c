

#include "rel_driver.h"
#include "defines.h"
//#include "stm32f3xx_hal_gpio.h"
#include "timer.h"
#include "comunication.h"
//#include "stm32f3xx_hal_spi.h"
#include "stm32f3xx_hal.h"

static uint8_t Driver1Reg=0;
static uint8_t Driver2Reg=0;
static uint8_t Driver3Reg=0;
static uint8_t Driver4Reg=0;
static uint8_t Driver5Reg=0;
extern SPI_HandleTypeDef hspi1;
uint8_t testiranje;

void init_REL(void)
{	
		REL_CS1_OFF;
		REL_CS2_OFF;
		REL_CS3_OFF;
		REL_CS4_OFF;
		REL_CS5_OFF;
		reset_all_REL();
		REL_RESET_OFF;
}
void reset_all_REL(void)
{
	uint32_t i;
	REL_RESET_ON;
	for(i=0;i<1000;i++){;}
	REL_RESET_OFF;
	Driver1Reg=0;
	Driver2Reg=0;
	Driver3Reg=0;
	Driver4Reg=0;
	Driver5Reg=0;
}

static void sel_REL_DRIVER(uint8_t rel_driver_nr)	//izbor rele driverja za spi komunikacijo, 0 pomeni izklop vseh
{
	switch(rel_driver_nr)
	{
		case 1: {REL_CS1_ON;break;}
		case 2: {REL_CS2_ON;break;}
		case 3: {REL_CS3_ON;break;}
		case 4: {REL_CS4_ON;break;}
		case 5: {REL_CS5_ON;break;}
	  default: {REL_CS1_OFF;REL_CS2_OFF;REL_CS3_OFF;REL_CS4_OFF;REL_CS5_OFF;break;}
	}
}
uint8_t set_REL(uint8_t rel_nr)
{
	//uint32_t i;
	uint32_t selected=0;
	//pazi tole je mal zajeban na relejski tiskanini, ker so premenjani CS pini. Prilagojeo tako da pase relejski
	if((rel_nr==8)||(rel_nr==16)||(rel_nr==22)||(rel_nr==19)||(rel_nr==18)||(rel_nr==15)||(rel_nr==9)||(rel_nr==40)) {sel_REL_DRIVER(1);	selected=1;}										//izbor rel driverja
	else if((rel_nr==26)||(rel_nr==34)||(rel_nr==25)||(rel_nr==23)||(rel_nr==27)||(rel_nr==41)||(rel_nr==28)||(rel_nr==33)) {sel_REL_DRIVER(2);	selected=2;}							//izbor rel driverja
	else if((rel_nr==37)||(rel_nr==5)||(rel_nr==6)||(rel_nr==4)||(rel_nr==3)||(rel_nr==2)||(rel_nr==38)||(rel_nr==1)) {sel_REL_DRIVER(3);	selected=3;}									//izbor rel driverja
	else if((rel_nr==11)||(rel_nr==12)||(rel_nr==17)||(rel_nr==13)||(rel_nr==20)||(rel_nr==39)||(rel_nr==24)||(rel_nr==29)) {sel_REL_DRIVER(4);	selected=4;}							//izbor rel driverja
	else if((rel_nr==10)||(rel_nr==21)||(rel_nr==42)||(rel_nr==35)||(rel_nr==36)||(rel_nr==30)||(rel_nr==7)||(rel_nr==14)||(rel_nr==43)) {sel_REL_DRIVER(5);	selected=5;}	//izbor rel driverja
	else return 1;		//rele ne obstaja
	testiranje = rel_nr;
	switch(selected)
	{
		case 1:
		{
			switch(rel_nr)
			{
				case 8: {Driver1Reg |= REL_OUT1; break;}
				case 16: {Driver1Reg |= REL_OUT2; break;}
				case 22: {Driver1Reg |= REL_OUT3; break;}
				case 19: {Driver1Reg |= REL_OUT4; break;}
				case 18: {Driver1Reg |= REL_OUT5; break;}
				case 15: {Driver1Reg |= REL_OUT6; break;}
				case 9: {Driver1Reg |= REL_OUT7; break;}
				case 40: {Driver1Reg |= REL_OUT8; break;}
				default: return 1; 
			}
			SPI1_Send_data(&Driver1Reg);
			break;
		}
		case 2:
		{
			switch(rel_nr)
			{
				case 26: {Driver2Reg |= REL_OUT1; break;}
				case 34: {Driver2Reg |= REL_OUT2; break;}
				case 25: {Driver2Reg |= REL_OUT3; break;}
				case 23: {Driver2Reg |= REL_OUT4; break;}
				case 27: {Driver2Reg |= REL_OUT5; break;}
				case 41: {Driver2Reg |= REL_OUT6; break;}
				case 28: {Driver2Reg |= REL_OUT7; break;}
				case 33: {Driver2Reg |= REL_OUT8; break;}
				default: return 1;
			}
			SPI1_Send_data(&Driver2Reg);
			break;
		}
		case 3:
		{
			
			switch(rel_nr)
			{
				case 37: {Driver3Reg |= REL_OUT1; break;}
				case 5: {Driver3Reg |= REL_OUT2; break;}
				case 6: {Driver3Reg |= REL_OUT3; break;}
				case 4: {Driver3Reg |= REL_OUT4; break;}
				case 3: {Driver3Reg |= REL_OUT5; break;}
				case 2: {Driver3Reg |= REL_OUT6; break;}
				case 1: 
				case 38: {Driver3Reg |= REL_OUT7; break;}
				default: return 1;
			}
			SPI1_Send_data(&Driver3Reg);
			break;
		}
		case 4:
		{
			switch(rel_nr)
			{
				case 11: {Driver4Reg |= REL_OUT1; break;}
				case 12: {Driver4Reg |= REL_OUT2; break;}
				case 17: {Driver4Reg |= REL_OUT3; break;}
				case 13: {Driver4Reg |= REL_OUT4; break;}
				case 20: {Driver4Reg |= REL_OUT5; break;}
				case 39: {Driver4Reg |= REL_OUT6; break;}
				case 24: {Driver4Reg |= REL_OUT7; break;}
				case 29: {Driver4Reg |= REL_OUT8; break;}
				default: return 1;
			}
			SPI1_Send_data(&Driver4Reg);
			break;
		}
		case 5:
		{
			switch(rel_nr)
			{
				case 10: {Driver5Reg |= REL_OUT1; break;}
				case 21: 
				case 43: {Driver5Reg |= REL_OUT2; break;}
				case 42: {Driver5Reg |= REL_OUT3; break;}
				case 35: {Driver5Reg |= REL_OUT4; break;}
				case 36: {Driver5Reg |= REL_OUT5; break;}
				case 30: {Driver5Reg |= REL_OUT6; break;}
				case 7:  {Driver5Reg |= REL_OUT7; break;}
				case 14: {Driver5Reg |= REL_OUT8; break;}
				default: return 1;//Driver5Reg =0;	
			}
			SPI1_Send_data(&Driver5Reg);
			break;
		}
		default: return 1;
	}
	while(HAL_SPI_GetState(&hspi1)==HAL_SPI_STATE_BUSY){;}
	REL_CS1_OFF;
	REL_CS2_OFF;
	REL_CS3_OFF;
	REL_CS4_OFF;
	REL_CS5_OFF;
	return 0;
}
uint8_t rst_REL(uint8_t rel_nr)
{
	//uint32_t i;
	uint32_t selected=0;
	if((rel_nr==8)||(rel_nr==16)||(rel_nr==22)||(rel_nr==19)||(rel_nr==18)||(rel_nr==15)||(rel_nr==9)||(rel_nr==40)) {sel_REL_DRIVER(1);	selected=1;}										//izbor rel driverja
	else if((rel_nr==26)||(rel_nr==34)||(rel_nr==25)||(rel_nr==23)||(rel_nr==27)||(rel_nr==41)||(rel_nr==28)||(rel_nr==33)) {sel_REL_DRIVER(2);	selected=2;}							//izbor rel driverja
	else if((rel_nr==37)||(rel_nr==5)||(rel_nr==6)||(rel_nr==4)||(rel_nr==3)||(rel_nr==2)||(rel_nr==38)||(rel_nr==1)) {sel_REL_DRIVER(3);	selected=3;}									//izbor rel driverja
	else if((rel_nr==11)||(rel_nr==12)||(rel_nr==17)||(rel_nr==13)||(rel_nr==20)||(rel_nr==39)||(rel_nr==24)||(rel_nr==29)) {sel_REL_DRIVER(4);	selected=4;}							//izbor rel driverja
	else if((rel_nr==10)||(rel_nr==21)||(rel_nr==42)||(rel_nr==35)||(rel_nr==36)||(rel_nr==30)||(rel_nr==7)||(rel_nr==14)||(rel_nr==43)) {sel_REL_DRIVER(5);	selected=5;}	//izbor rel driverja
	else return 1;		//rele ne obstaja
	testiranje = rel_nr;
	switch(selected)
	{
		case 1:
		{
			switch(rel_nr)
			{
				case 8:  {Driver1Reg &= (~REL_OUT1); break;}
				case 16: {Driver1Reg &= (~REL_OUT2); break;}
				case 22: {Driver1Reg &= (~REL_OUT3); break;}
				case 19: {Driver1Reg &= (~REL_OUT4); break;}
				case 18: {Driver1Reg &= (~REL_OUT5); break;}
				case 15: {Driver1Reg &= (~REL_OUT6); break;}
				case 9:  {Driver1Reg &= (~REL_OUT7); break;}
				case 40: {Driver1Reg &= (~REL_OUT8); break;}
				default: break;	
			}
			SPI1_Send_data(&Driver1Reg);
			break;
		}
		case 2:
		{
			switch(rel_nr)
			{
				case 26: {Driver2Reg &= (~REL_OUT1); break;}
				case 34: {Driver2Reg &= (~REL_OUT2); break;}
				case 25: {Driver2Reg &= (~REL_OUT3); break;}
				case 23: {Driver2Reg &= (~REL_OUT4); break;}
				case 27: {Driver2Reg &= (~REL_OUT5); break;}
				case 41: {Driver2Reg &= (~REL_OUT6); break;}
				case 28: {Driver2Reg &= (~REL_OUT7); break;}
				case 33: {Driver2Reg &= (~REL_OUT8); break;}
				default: break;	
			}
			SPI1_Send_data(&Driver2Reg);
			break;
		}
		case 3:
		{
			
			switch(rel_nr)
			{
				case 37: {Driver3Reg &= (~REL_OUT1); break;}
				case 5:  {Driver3Reg &= (~REL_OUT2); break;}
				case 6:  {Driver3Reg &= (~REL_OUT3); break;}
				case 4:  {Driver3Reg &= (~REL_OUT4); break;}
				case 3:  {Driver3Reg &= (~REL_OUT5); break;}
				case 2:  {Driver3Reg &= (~REL_OUT6); break;}
				case 1: 
				case 38: {Driver3Reg &= (~REL_OUT7); break;}
				default: break;	
			}
			SPI1_Send_data(&Driver3Reg);
			break;
		}
		case 4:
		{
			switch(rel_nr)
			{
				case 11: {Driver4Reg &= (~REL_OUT1); break;}
				case 12: {Driver4Reg &= (~REL_OUT2); break;}
				case 17: {Driver4Reg &= (~REL_OUT3); break;}
				case 13: {Driver4Reg &= (~REL_OUT4); break;}
				case 20: {Driver4Reg &= (~REL_OUT5); break;}
				case 39: {Driver4Reg &= (~REL_OUT6); break;}
				case 24: {Driver4Reg &= (~REL_OUT7); break;}
				case 29: {Driver4Reg &= (~REL_OUT8); break;}
				default: break;	
			}
			SPI1_Send_data(&Driver4Reg);
			break;
		}
		case 5:
		{
			switch(rel_nr)
			{
				case 10: {Driver5Reg &= (~REL_OUT1); break;}
				case 21: 
				case 43: {Driver5Reg &= (~REL_OUT2); break;}
				case 42: {Driver5Reg &= (~REL_OUT3); break;}
				case 35: {Driver5Reg &= (~REL_OUT4); break;}
				case 36: {Driver5Reg &= (~REL_OUT5); break;}
				case 30: {Driver5Reg &= (~REL_OUT6); break;}
				case 7:  {Driver5Reg &= (~REL_OUT7); break;}
				case 14: {Driver5Reg &= (~REL_OUT8); break;}
				default: break;	
			}
			SPI1_Send_data(&Driver5Reg);
			break;
		}
		default: return 1;
	}
	while(HAL_SPI_GetState(&hspi1)==HAL_SPI_STATE_BUSY){;}
	REL_CS1_OFF;
	REL_CS2_OFF;
	REL_CS3_OFF;
	REL_CS4_OFF;
	REL_CS5_OFF;
	return 0;
}
