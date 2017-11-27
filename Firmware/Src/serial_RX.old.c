

#include "serial_RX.h"
#include "do_task.h"
#include <stdint.h>
#include <string.h>
#include "serial_TX.h"
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <ctype.h>
#include "rel_driver.h"
#include "usbd_cdc_if.h"
#include "comunication.h"
#include "tasks.h"
#include "os.h"
//parser for serial protocol command
//return values
// _ANS_MESSAGE_NACK   - message not acknowleged
// _ANS_MESSAGE_ACK    - message acknowleged
extern uint8_t UART_direction;

typedef enum {
    STR2INT_SUCCESS,
    STR2INT_OVERFLOW,
    STR2INT_UNDERFLOW,
    STR2INT_INCONVERTIBLE
} str2int_errno;

#if TRANSMIT_COMMAND_CHECK == _ON
typedef struct
{
 uint32_t message_ID;        	//ser_ID_message v int obliki
 uint32_t dirrection;
 uint32_t size;
 uint32_t nack_count;
 char message_buff[TRANSMIT_BUFF_SIZE];	
} TRANSMIT_BUFFER;

TRANSMIT_BUFFER Transmit_handle_buff[TRANSMIT_HANDLE_BUFF_SIZE];
uint32_t write_count=0;
uint32_t read_count=0;
#endif



int int_from_str;
char* float_from_str;

uint8_t last_recieved_dir=0;
uint8_t ser_ID_sender;
static char ser_ID_message[_SER_DATA_SIZE];
static char ser_ID_function[_SER_DATA_SIZE];
static char ser_ID_type[_SER_DATA_SIZE];  
static char ser_ID_code[_SER_DATA_SIZE];
static char ser_ID_data[_SER_DATA_SIZE];
static char ser_ID_code_p1[_SER_DATA_SIZE];
static char ser_ID_code_p2[_SER_DATA_SIZE];
static char ser_ID_code_p3[_SER_DATA_SIZE];
extern uint32_t global_control;

uint32_t indicator1;
uint32_t indicator2;
uint32_t indicator3;
uint32_t indicator4;
uint8_t 	indicator_polje[100];
uint8_t dir_buff;

uint8_t command_parser(char *cmd, uint8_t dir)
{    
 uint8_t start_sign; 
 uint8_t ser_ID_receiver;     
 uint8_t delimiter_pointers[8];
 char cmd_loc[_SER_BUFFER_SIZE];	
 uint8_t data_delimiter_pos;	
 int8_t delimiter_ptr;	
	
 char checksum_state[_SER_DATA_SIZE];
 uint8_t checksum_read;
 uint8_t checksum_compute;  	
	
 uint8_t cc,cd;
 uint8_t terminator_pos;
 uint8_t d1,d2,d3;	
	
 //parse data
 start_sign=cmd[0];
 ser_ID_receiver=cmd[2];
 
 //reject message if start sign not mached or not for me
 if(start_sign!=_SER_START_SIGN) return _ANS_MESSAGE_NACK;
 if(ser_ID_receiver!=_ID_TFA) return _ANS_MESSAGE_NACK;
	
 //ce v message-u (dolzina message-a je 100 znakov) ni 8 delimiterjev ':', potem answer _ANS_MESSAGE_NACK
 //ce so, potem najdi pointerje na delimiterje
 //pointerje shrani v delimiter_pointers[8] 	
 //------------------------------------------------------------------------
 strcpy(cmd_loc,cmd);
 
 for(delimiter_ptr=7;delimiter_ptr>=0;delimiter_ptr--)
  {
   if(strrchr(cmd_loc,_SER_DATA_DELIMITER)==NULL) return _ANS_MESSAGE_NACK;
   data_delimiter_pos=strrchr(cmd_loc,_SER_DATA_DELIMITER)-cmd_loc;     
	 //TxDec(data_delimiter_pos);Crlf(); //test
	 delimiter_pointers[delimiter_ptr]=data_delimiter_pos;
	 cmd_loc[data_delimiter_pos]=0; //rezanje teksta od zadnjega delimiterja				 
	}	  
 //------------------------------------------------------------------------
	
 
 //parse additional data: function, type, code
 //------------------------------------------------------------------------      
 ser_ID_sender=cmd[1];   
 *ser_ID_message=0;
 strncat(ser_ID_message,cmd+delimiter_pointers[0]+1,(delimiter_pointers[1]-delimiter_pointers[0]-1));    	  
 //TxStr(ser_ID_message);Crlf(); //test    
 *ser_ID_function=0;
 strncat(ser_ID_function,cmd+delimiter_pointers[1]+1,(delimiter_pointers[2]-delimiter_pointers[1]-1));    	  
 //TxStr(ser_ID_function);Crlf(); //test   
 *ser_ID_type=0;
 strncat(ser_ID_type,cmd+delimiter_pointers[2]+1,(delimiter_pointers[3]-delimiter_pointers[2]-1));    	  
 //TxStr(ser_ID_type);Crlf(); //test    	 
 *ser_ID_code=0;
 strncat(ser_ID_code,cmd+delimiter_pointers[3]+1,(delimiter_pointers[4]-delimiter_pointers[3]-1));    	  
 //TxStr(ser_ID_code);Crlf(); //test        
 //------------------------------------------------------------------------

 //parse data string
 //------------------------------------------------------------------------ 
 *ser_ID_data=0;
 strncat(ser_ID_data,cmd+delimiter_pointers[4]+1,(delimiter_pointers[5]-delimiter_pointers[4]-1));    	  
 //TxStr(ser_ID_data);Crlf(); //test    
 //------------------------------------------------------------------------	
	
 //parse checksum state
 //------------------------------------------------------------------------ 
 *checksum_state=0;
 strncat(checksum_state,cmd+delimiter_pointers[5]+1,(delimiter_pointers[6]-delimiter_pointers[5]-1)); 	  
 //TxStr(checksum_state);Crlf(); //test    
 //------------------------------------------------------------------------	

 //ce je checksum state 1, potem preverjamo checksum
 //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 if(*checksum_state=='1') 
 { 
	 	
 //get checksum value -> checksum_read
 //------------------------------------------------------------------------	
 terminator_pos=strchr(cmd,0x00)-cmd;
 terminator_pos--; //da se ne uposteva zadnji _SER_DATA_DELIMITER znak	 
 //TxDec(terminator_pos);Crlf(); 
 cd=0;
 for(cc=delimiter_pointers[6]+1;cc<terminator_pos;cc++) 
  {
   switch(cd)
    { 
     case 0: d1=cmd[cc]-48;break;
     case 1: d2=cmd[cc]-48;break;
     case 2: d3=cmd[cc]-48;break;
    default:break;
    }  
   cd++;
  }
 switch(cd)
  { 
   case 1: checksum_read=d1;break;
   case 2: checksum_read=10*d1+d2;break;
   case 3: checksum_read=100*d1+10*d2+d3;break;
   default:break;
  }
 //TxDec(checksum_read);Crlf();
 //------------------------------------------------------------------------	
	
 //get checksum value -> checksum_compute
 //------------------------------------------------------------------------	
 checksum_compute=0;
 for(cc=0;cc<=delimiter_pointers[6];cc++) checksum_compute+=cmd[cc];   
 //TxDec(checksum_compute);Crlf(); 
 //------------------------------------------------------------------------	 
 
 if(checksum_read!=checksum_compute) return _ANS_MESSAGE_NACK;  
	
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 

 return _ANS_MESSAGE_ACK;
}  


//return answer (acknowledge or not acknowledge)
void command_return(uint8_t ans, uint8_t dir)
{  
 uint8_t checksum;
 uint32_t cnt;
 uint8_t cc,cd,d1,d2,d3;
 uint8_t dir_tmp; 
 char ser_ans_buffer[_SER_BUFFER_SIZE];
 char checksum_buffer[4];	
  
 //construct message
 //------------------------------------------------------------------------		
 ser_ans_buffer[0]=_SER_START_SIGN;
 ser_ans_buffer[1]=	_ID_TFA;  
 ser_ans_buffer[2]=ser_ID_sender;
 ser_ans_buffer[3]=_SER_DATA_DELIMITER;  
 ser_ans_buffer[4]=0;  	
 strcat(ser_ans_buffer,ser_ID_message);		
 ser_ans_buffer[6]=_SER_DATA_DELIMITER;  
 ser_ans_buffer[7]=0;  	
 	
 switch(ans)
  {
   case _ANS_MESSAGE_NACK:strcat(ser_ans_buffer,_MESSAGE_NACK); break;
   case _ANS_MESSAGE_ACK: strcat(ser_ans_buffer,_MESSAGE_ACK); break;	  	
   default:break;
  }
 
 cd=strlen(ser_ans_buffer);	
 ser_ans_buffer[cd++]=_SER_DATA_DELIMITER;  
 ser_ans_buffer[cd++]=_SER_DATA_DELIMITER;  
 ser_ans_buffer[cd++]=_SER_DATA_DELIMITER;  
 ser_ans_buffer[cd++]=_SER_DATA_DELIMITER;  
 ser_ans_buffer[cd++]='1'; //checksum status is always ON  
 ser_ans_buffer[cd++]=_SER_DATA_DELIMITER;  	
 ser_ans_buffer[cd]=0;  		
 //------------------------------------------------------------------------		

 
 //compute and append checksum
 //------------------------------------------------------------------------	
 //TxDec(strlen(ser_ans_buffer));Crlf(); //test	   
 checksum=0;
 for(cc=0;cc<strlen(ser_ans_buffer);cc++) checksum+=ser_ans_buffer[cc]; 
 //TxDec(checksum);Crlf(); //test
 d1=checksum/100;
 d2=(checksum%100)/10;
 d3=(checksum%100)%10;   
 //TxChr(d1+48);Crlf();
 //TxChr(d2+48);Crlf();
 //TxChr(d3+48);Crlf(); 
 checksum_buffer[0]=d1+48;
 checksum_buffer[1]=d2+48;
 checksum_buffer[2]=d3+48; 
 checksum_buffer[3]=0; 
 strcat(ser_ans_buffer,checksum_buffer);
 //TxStr(ser_ans_buffer);Crlf(); //test	    

 cd=strlen(ser_ans_buffer);
 ser_ans_buffer[cd++]=_SER_DATA_DELIMITER;
 ser_ans_buffer[cd++]=13;
 ser_ans_buffer[cd++]=10;
 ser_ans_buffer[cd]=0;
 //TxStr(ser_ans_buffer); //test	    
 //------------------------------------------------------------------------	
  
 
 //TxStr(ser_ID_type);Crlf(); //test	    	
 //NACK or ACK poslejmo v vsakem primeru, razen ce je telegram type enak RESULT, CALIB ali function enak FIRMWARE
 //if(!((!strcmp(ser_ID_type,_SER_TELEGRAM_RESULT)) || (!strcmp(ser_ID_function,_SER_TELEGRAM_FIRMWARE)) || (!strcmp(ser_ID_type,_SER_TELEGRAM_CALIB))))
 if(!(!strcmp(ser_ID_type,_SER_TELEGRAM_CALIB))) //zaenkrat samo v kalibraciji ne posiljam acknowledge stringa
  {    
		indicator1=cd;
		for(cnt=0;cnt<cd;cnt++)
		{
			indicator_polje[cnt]=ser_ans_buffer[cnt];
		}
//   dir_tmp=UART_direction;
//   UART_direction=dir;
//   TxStr(ser_ans_buffer); 
//	 UART_direction=dir_tmp;
		if((dir==_UART_DIR_USB)||(dir==_UART_DIR_DEBUG))
		{
//			dir_tmp=UART_direction;
//			UART_direction=dir;
			//TxStr(ser_ans_buffer); 
			CDC_Transmit_FS((uint8_t *)ser_ans_buffer, cd);
//			UART_direction=dir_tmp;
		}
  }

}


//send message to module or terminal
void send_com_message(char * ans_type, char * ans_code, uint8_t num_valid, char *num, uint8_t dir)
{  
 uint8_t checksum;
 uint8_t cc,cd,d1,d2,d3;
 uint8_t dir_tmp; 
 char ser_ans_buffer[_SER_BUFFER_SIZE];
 char checksum_buffer[4];	
 static uint32_t msg_count=1;
 char current_ID[2];
 
 //doloci ID za novo komando
 if(TRANSMIT_COMMAND_CHECK==_ON) sprintf(current_ID,"%u.2",msg_count);
	
 //construct message
 //------------------------------------------------------------------------		
 ser_ans_buffer[0]=_SER_START_SIGN;
 ser_ans_buffer[1]=_ID_TFA;						//ce pises kodo za drug aparat popravi tole!!!!!!!!!!!!!  
 ser_ans_buffer[2]=ser_ID_sender;			//tisti od katerega sem nazadnje dobil ukaz
 ser_ans_buffer[3]=_SER_DATA_DELIMITER;  
 ser_ans_buffer[4]=0;  	
 if(TRANSMIT_COMMAND_CHECK==_ON) strcat(ser_ans_buffer,current_ID);
 else strcat(ser_ans_buffer,ser_ID_message);
 ser_ans_buffer[6]=_SER_DATA_DELIMITER;  
 ser_ans_buffer[7]=0;  	

 //function string
 strcat(ser_ans_buffer,ser_ID_function);			
 cd=strlen(ser_ans_buffer);	
 ser_ans_buffer[cd++]=_SER_DATA_DELIMITER;  
 ser_ans_buffer[cd]=0;

 //type string
 strcat(ser_ans_buffer,ans_type);			
 cd=strlen(ser_ans_buffer);	
 ser_ans_buffer[cd++]=_SER_DATA_DELIMITER;  
 ser_ans_buffer[cd]=0; 
 
 //code string
 strcat(ser_ans_buffer,ans_code);			
 cd=strlen(ser_ans_buffer);	
 ser_ans_buffer[cd++]=_SER_DATA_DELIMITER;  
 ser_ans_buffer[cd]=0; 
 
 //data string goes here
 switch(num_valid)
  { 
   case _NAN: 
		break; 
   case _VALID:
    strcat(ser_ans_buffer,num);			
    cd=strlen(ser_ans_buffer);	
    ser_ans_buffer[cd++]=_SER_DATA_DELIMITER;  
    ser_ans_buffer[cd]=0; 
    break;  
   default:break;
  }
    
 
 //checksum status byte 	
 //------------------------------------------------------------------------		
 cd=strlen(ser_ans_buffer);	 
 ser_ans_buffer[cd++]='1'; //checksum status is always ON  
 ser_ans_buffer[cd++]=_SER_DATA_DELIMITER;  	
 ser_ans_buffer[cd]=0;  		
 //------------------------------------------------------------------------		
 
 //compute and append checksum
 //------------------------------------------------------------------------	
 //TxDec(strlen(ser_ans_buffer));Crlf(); //test	   
 checksum=0;
 for(cc=0;cc<strlen(ser_ans_buffer);cc++) checksum+=ser_ans_buffer[cc]; 
 //TxDec(checksum);Crlf(); //test
 d1=checksum/100;
 d2=(checksum%100)/10;
 d3=(checksum%100)%10;   
 //TxChr(d1+48);Crlf();
 //TxChr(d2+48);Crlf();
 //TxChr(d3+48);Crlf(); 
 checksum_buffer[0]=d1+48;
 checksum_buffer[1]=d2+48;
 checksum_buffer[2]=d3+48; 
 checksum_buffer[3]=0; 
 strcat(ser_ans_buffer,checksum_buffer);
 //TxStr(ser_ans_buffer);Crlf(); //test	    

 cd=strlen(ser_ans_buffer);
 ser_ans_buffer[cd++]=_SER_DATA_DELIMITER;
 ser_ans_buffer[cd++]=13;
 ser_ans_buffer[cd++]=10;
 ser_ans_buffer[cd]=0;
 //TxStr(ser_ans_buffer); //test	    
 //------------------------------------------------------------------------	
		 
 //send string
	if((dir==_UART_DIR_USB)||(dir==_UART_DIR_DEBUG))
	{
		CDC_Transmit_FS((uint8_t *)ser_ans_buffer, cd);
	}
	
// dir_tmp=UART_direction;
// UART_direction=dir;
// TxStr(ser_ans_buffer); 
// UART_direction=dir_tmp; 
	
	
//	#if TRANSMIT_COMMAND_CHECK == _ON
//	//deluje kot ciklicen buffer
//	Transmit_handle_buff[write_count].message_ID=msg_count;
//	Transmit_handle_buff[write_count].dirrection =dir;
//	strcpy(Transmit_handle_buff[write_count].message_buff, ser_ans_buffer);
//	if(write_count>=(TRANSMIT_HANDLE_BUFF_SIZE-1))write_count=0;
//	else write_count++;
//	set_event(TRANSMIT_COMMAND_HANDLE,transmit_command_handle);
//	if(msg_count >= 99) msg_count = 1;
//	else msg_count++;
//	#endif
}

str2int_errno str2int(int *out, char *s, int base) {
    char *end;
		long l = strtol(s, &end, base);
    if (s[0] == '\0' || isspace((unsigned char) s[0]))
        return STR2INT_INCONVERTIBLE;
    errno = 0;
    /* Both checks are needed because INT_MAX == LONG_MAX is possible. */
    if (l > INT_MAX || (errno == ERANGE && l == LONG_MAX))
        return STR2INT_OVERFLOW;
    if (l < INT_MIN || (errno == ERANGE && l == LONG_MIN))
        return STR2INT_UNDERFLOW;
    if (*end != '\0')
        return STR2INT_INCONVERTIBLE;
    *out = l;
    return STR2INT_SUCCESS;
}
//#if TRANSMIT_COMMAND_CHECK == _ON
//void transmit_command_handle(void)
//{
//	//poglej ce je kaj v ciklicnem bufferju
//	if(write_count!=read_count)
//	{
//		Transmit_handle_buff[write_count].message_ID
//	}
//	
//}
//#endif

/*******************************************************************************/
/**															ANALIZE COMMAND																**/
/*******************************************************************************/
void command_analyze(uint8_t dir)
{
 uint8_t direction;
 if(!strcmp(ser_ID_function,__MT_300__))
	{
/*******************************************************************************/
/**													INPUT CONNECTION NOT OK														**/
/*******************************************************************************/
		if(global_control & __INIT_TEST_FAIL)
		{
			set_event(SEND_WARNING_MSG,send_warning_MSG);
		}
/*******************************************************************************/
/**															MEASURING METODS															**/
/*******************************************************************************/
		else if(!strcmp(ser_ID_type,__POWER__))
	  {     
			if(!strcmp(ser_ID_code,__START__))
			{
				start_measure();
			}
			else if(!strcmp(ser_ID_code,__STOP__))
			{
				stop_measure();
			}
			else if(!strcmp(ser_ID_code,__START_NO_THD__))
			{
				start_measure_no_THD();
			}
			
		} 
/*******************************************************************************/
/**																		RELAYS																	**/
/*******************************************************************************/
		else if(!strcmp(ser_ID_type,__RELAY__))
		{ 
			if(!strcmp(ser_ID_code,__1_38_ON__))				set_REL(1);
			else if(!strcmp(ser_ID_code,__1_38_OFF__))	rst_REL(1);
			else if(!strcmp(ser_ID_code,__2_ON__))	set_REL(2);
			else if(!strcmp(ser_ID_code,__2_OFF__))	rst_REL(2);
			else if(!strcmp(ser_ID_code,__3_ON__))	set_REL(3);		
			else if(!strcmp(ser_ID_code,__3_OFF__))	rst_REL(3);
			else if(!strcmp(ser_ID_code,__4_ON__))	set_REL(4);
			else if(!strcmp(ser_ID_code,__4_OFF__))	rst_REL(4);
			else if(!strcmp(ser_ID_code,__5_ON__))	set_REL(5);
			else if(!strcmp(ser_ID_code,__5_OFF__))	rst_REL(5);
			else if(!strcmp(ser_ID_code,__6_ON__))	set_REL(6);		
			else if(!strcmp(ser_ID_code,__6_OFF__))	rst_REL(6);
			else if(!strcmp(ser_ID_code,__7_ON__))	set_REL(7);
			else if(!strcmp(ser_ID_code,__7_OFF__))	rst_REL(7);
			else if(!strcmp(ser_ID_code,__8_ON__))	set_REL(8);
			else if(!strcmp(ser_ID_code,__8_OFF__))	rst_REL(8);
			else if(!strcmp(ser_ID_code,__9_ON__))	set_REL(9);		
			else if(!strcmp(ser_ID_code,__9_OFF__))	rst_REL(9);
			else if(!strcmp(ser_ID_code,__10_ON__))	set_REL(10);
			else if(!strcmp(ser_ID_code,__10_OFF__))	rst_REL(10);
			else if(!strcmp(ser_ID_code,__11_ON__))	set_REL(11);
			else if(!strcmp(ser_ID_code,__11_OFF__))	rst_REL(11);
			else if(!strcmp(ser_ID_code,__12_ON__))	set_REL(12);		
			else if(!strcmp(ser_ID_code,__12_OFF__))	rst_REL(12);
			else if(!strcmp(ser_ID_code,__13_ON__))	set_REL(13);
			else if(!strcmp(ser_ID_code,__13_OFF__))	rst_REL(13);
			else if(!strcmp(ser_ID_code,__14_ON__))	set_REL(14);
			else if(!strcmp(ser_ID_code,__14_OFF__))	rst_REL(14);
			else if(!strcmp(ser_ID_code,__15_ON__))	set_REL(15);		
			else if(!strcmp(ser_ID_code,__15_OFF__))	rst_REL(15);
			else if(!strcmp(ser_ID_code,__16_ON__))	set_REL(16);
			else if(!strcmp(ser_ID_code,__16_OFF__))	rst_REL(16);
			else if(!strcmp(ser_ID_code,__17_ON__))	set_REL(17);
			else if(!strcmp(ser_ID_code,__17_OFF__))	rst_REL(17);
			else if(!strcmp(ser_ID_code,__18_ON__))	set_REL(18);		
			else if(!strcmp(ser_ID_code,__18_OFF__))	rst_REL(18);
			else if(!strcmp(ser_ID_code,__19_ON__))	set_REL(19);
			else if(!strcmp(ser_ID_code,__19_OFF__))	rst_REL(19);
			else if(!strcmp(ser_ID_code,__20_ON__))	set_REL(20);
			else if(!strcmp(ser_ID_code,__20_OFF__))	rst_REL(20);
			else if(!strcmp(ser_ID_code,__21_43_ON__))	set_REL(21);		
			else if(!strcmp(ser_ID_code,__21_43_OFF__))	rst_REL(21);
			else if(!strcmp(ser_ID_code,__22_ON__))	set_REL(22);
			else if(!strcmp(ser_ID_code,__22_OFF__))	rst_REL(22);
			else if(!strcmp(ser_ID_code,__23_ON__))	set_REL(23);
			else if(!strcmp(ser_ID_code,__23_OFF__))	rst_REL(23);
			else if(!strcmp(ser_ID_code,__24_ON__))	set_REL(24);		
			else if(!strcmp(ser_ID_code,__24_OFF__))	rst_REL(24);
			else if(!strcmp(ser_ID_code,__25_ON__))	set_REL(25);
			else if(!strcmp(ser_ID_code,__25_OFF__))	rst_REL(25);
			else if(!strcmp(ser_ID_code,__26_ON__))	set_REL(26);
			else if(!strcmp(ser_ID_code,__26_OFF__))	rst_REL(26);
			else if(!strcmp(ser_ID_code,__27_ON__))	set_REL(27);		
			else if(!strcmp(ser_ID_code,__27_OFF__))	rst_REL(27);
			else if(!strcmp(ser_ID_code,__28_ON__))	set_REL(28);
			else if(!strcmp(ser_ID_code,__28_OFF__))	rst_REL(28);
			else if(!strcmp(ser_ID_code,__29_ON__))	set_REL(29);
			else if(!strcmp(ser_ID_code,__29_OFF__))	rst_REL(29);
			else if(!strcmp(ser_ID_code,__30_ON__))	set_REL(30);
			else if(!strcmp(ser_ID_code,__30_OFF__))	rst_REL(30);
			else if(!strcmp(ser_ID_code,__31_ON__))	set_REL(31);
			else if(!strcmp(ser_ID_code,__31_OFF__))	rst_REL(31);
			else if(!strcmp(ser_ID_code,__32_ON__))	set_REL(32);
			else if(!strcmp(ser_ID_code,__32_OFF__))	rst_REL(32);
			else if(!strcmp(ser_ID_code,__33_ON__))	set_REL(33);
			else if(!strcmp(ser_ID_code,__33_OFF__))	rst_REL(33);
			else if(!strcmp(ser_ID_code,__34_ON__))	set_REL(34);
			else if(!strcmp(ser_ID_code,__34_OFF__))	rst_REL(34);
			else if(!strcmp(ser_ID_code,__35_ON__))	set_REL(35);
			else if(!strcmp(ser_ID_code,__35_OFF__))	rst_REL(35);
			else if(!strcmp(ser_ID_code,__36_ON__))	set_REL(36);
			else if(!strcmp(ser_ID_code,__36_OFF__))	rst_REL(36);
			else if(!strcmp(ser_ID_code,__37_ON__))	set_REL(37);
			else if(!strcmp(ser_ID_code,__37_OFF__))	rst_REL(37);
			else if(!strcmp(ser_ID_code,__39_ON__))	set_REL(39);
			else if(!strcmp(ser_ID_code,__39_OFF__))	rst_REL(39);
			else if(!strcmp(ser_ID_code,__40_ON__))	set_REL(40);
			else if(!strcmp(ser_ID_code,__40_OFF__))	rst_REL(40);
			else if(!strcmp(ser_ID_code,__41_ON__))	set_REL(41);
			else if(!strcmp(ser_ID_code,__41_OFF__))	rst_REL(41);
			else if(!strcmp(ser_ID_code,__42_ON__))	set_REL(42);
			else if(!strcmp(ser_ID_code,__42_OFF__))	rst_REL(42);
			else if(!strcmp(ser_ID_code,__RESET_ALL__))	reset_all_REL();
		}
/*******************************************************************************/
/**															ADITIONAL SETTINGS														**/
/*******************************************************************************/
			
  }
}

	                     
                                                 
  




