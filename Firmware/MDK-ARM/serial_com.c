
//Knjiznica s serijskim protokolom za posiljanje in sprejemanje komand po principu:
//>TR:ID:FUNC:COMMAND:PARAM_NAME1|PARAM_VALUE1,PARAM_NAME1|PARAM_VALUE1,PARAM_NAME2|PARAM_VALUE2,PARAM_NAME3|PARAM_VALUE3,...:CODE:C:CRC:#013#010

//&&&&&&&&&&&&&&&&&&&NAVODILA ZA UPORABO PROTOKOLA&&&&&&&&&&&&&&&&&&&&&&&&
//Spodaj so funkcije ki so namenjene prejemanju in posiljanju komand
//zunaj knjiznice so dostopne samo stiri funkcije:
//				- za inicializacijo: 	serial_com_init()
//				- za prejemanje: 			add_command_to_queue(...)
//				- za oddajanje: 			SendComMessage(...)
//				- za preverjanje ACK:	CheckIfACK(...)
//	*****************************************************************************
//	*	najprej v serial_com.h definieamo mozne smeri (vodila) iz katerih lahko dobivamo podatke (Makroji smeri)
//	*	nastavimo TRANSMIT_COMMAND_CHECK na _ON ali _OFF
//	*	nastavimo SYNCHRONUS_TRANSMITT na _ON ali _OFF
//  * preverimo ce so vsi makroji nastavljeni pravilno
//	* ce smo nastavili SYNCHRONUS_TRANSMITT na _OFF potem:
//	*	--	dodamo funkcije za posiljanje v funkcijo SerialSend(..) - dodane funkcije so lahko odvisne od smeri prejema podatkov (dirrection), tako lahko definiramo posiljanje po razlicnih vodilih, kot je USB in RS485. Ne pozabi prilagoditi nastavljanje smeri v primeru posiljanja praznega stringa.
//	* ce smo nastavili SYNCHRONUS_TRANSMITT na _OFF potem:
//	* -- 	napisemo funkcijo z imenom void SendTimerInit(void), v kateri inicializiramo timer
//  * --  prilagodimo funkcijo resetSendTimer()
//  * --  dodaj serialComErrorTimeout++; v SysTich_Handler() in nastavi SERIAL_COM_ERROR_TIMEOUT 
//  * -- 	dodamo SINCHRONUS_TRANSMITT in SYNCHRONUS_PROCESS v tasks.h operacijskega sistema
//  * --	dodamo funkcije za posiljanje v funkcijo SerialSend(..) - dodane funkcije so lahko odvisne od smeri prejema podatkov (dirrection), tako lahko definiramo posiljanje po razlicnih vodilih, kot je USB in RS485. Ne pozabi prilagoditi nastavljanje smeri v primeru posiljanja praznega stringa.
//	*	dodamo svojo kodo za analiziranje prejete komande v command_analyze(..)
//	***************************************************************************
//	* ko imamo vse nastavljeno lahko v main.c - ju poklicemo inicializacijo serial_com_init()
//	* ko dobimo podatke po vodilu poklicemo funkcijo add_command_to_queue(..), ki postavi komando v cakalno vrsto za analiziranje, zapomni pa se tudi od kje je prisla, da zna vrnit ACK
//	* ce zelimo podatke poslati poklicemo funkcijo SendComMessage(...)
//	* funkcija CheckIfACK(...) pa preveri ce je poslano sporocilo dobilo ACK


#include "serial_com.h"
#include "do_task.h"
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <ctype.h>
#include "rel_driver.h"
#include "usbd_cdc_if.h"
#include "comunication.h"
#include "tasks.h"
#include "os.h"
#include <stdbool.h>
#include <stdio.h>
#include "cord.h"
#include "machines.h"
#include "stm32f3xx_it.h"
#include "stm32f3xx_hal.h"
#include "stm32f3xx.h"
#include "com_meas_tasks.h"
#include "welding.h"
#include "calibration.h"
#include "sdadc.h"
//-----------ZUNANJE SPREMENLJIVKE-----------------------
extern uint8_t event_status; 
extern uint32_t connection_control;
extern uint32_t meas_task_control;
extern uint32_t current_URES_measurement;
extern USBD_HandleTypeDef hUsbDeviceFS;
extern uint32_t start_mach_count;
extern uint32_t mach_RISO_count;
extern uint32_t mach_task_control;
extern int start_cord_count;
extern uint32_t cord_RISO_count;
extern uint32_t cord_task_control;
extern uint32_t start_weld_count;
extern uint32_t weld_RISO_count;
extern uint32_t weld_task_control;
extern uint32_t meas_control;
extern uint32_t calib_task_control;

//-----------GLOBALNE SPREMENLJIVKE----------------------
uint8_t SERIAL_direction;
uint32_t synchronus_error_count=0;
uint8_t event_status; 
SERIAL_RX_t SERIAL_in_queue_buffer[SERIAL_IN_QUEUE_SIZE]; //array of UART3 RX commands
volatile uint32_t RxFifoIndex = 0;
static uint8_t INPUT_Buffer[RxBufferSize_MAX];
static void increaseWritePtr(void);
static void SinchronusTransmittFunc(void);
static void transmittEmptyStr(void);
static void SerialSend(uint8_t *,uint16_t msg_size, uint32_t dir);
static void resetSendTimer(void);
static void SynchronusSendInit(void);
void SynchronusProcess(void);
int int_from_str;
char* float_from_str;
uint32_t serialComErrorTimeout=0;
bool serialComErrorDetected=false;
TIM_HandleTypeDef htim2;

int indikator1;
int indikator2;
int indikator3;
int indikator4;
int indikator5;

typedef enum {
    STR2INT_SUCCESS,
    STR2INT_OVERFLOW,
    STR2INT_UNDERFLOW,
    STR2INT_INCONVERTIBLE
} str2int_errno;

typedef struct
{
	char transmitter, reciever;
  int msg_ID;
  char * replay_function;
  char * replay_cmd;
  char * replay_additional_code;
  char * replay_data;
}MESSAGE_CONSTRUCTOR;
#if TRANSMIT_COMMAND_CHECK == _ON
typedef struct 
{
 uint32_t message_ID;        	//ser_ID_message v int obliki
 uint32_t dirrection;
 uint32_t size;
 uint32_t nack_count;
 uint32_t nack_flag;	
 uint32_t buffer_count;
 char transmitt_to_ID;
 char * msg_ptr;
} TRANSMIT_BUFFER;


//---------------------CIKLICEN BUFFER ZA TRANSMIT HANDLE BUFFER------------------------
//stevca za vpisovanje in branje v ciklicen buffer
uint32_t write_count=0;
uint32_t read_count=0;
uint32_t g;
TRANSMIT_BUFFER Transmit_handle_buff[TRANSMIT_HANDLE_BUFF_SIZE];
#endif
//---------------------CIKLICEN BUFFER ZA TRANSMIT OUT BUFFER------------------------
typedef struct
{
	uint32_t dirrection;
	uint32_t size;
	char * msg_ptr;
}TRANSMIT_OUT_BUFFER;

uint32_t write_out_count=0;
uint32_t read_out_count=0;
TRANSMIT_OUT_BUFFER Transmit_out_buff[TRANSMIT_OUT_BUFF_SIZE];




//funkcija za razbijanje stringov po delimiterjih - nadomesti delimiter z 0 zato jih moramo nekako dat nazaj
static char *strtok_single (char * str, char const * delims)
{
  static char  * src = NULL;
  char  *  p,  * ret = 0;

  if (str != NULL)
    src = str;

  if (src == NULL)
    return NULL;

  if ((p = strpbrk (src, delims)) != NULL) {
    *p  = 0;
    ret = src;
    src = ++p;

  } 
	else if (*src) {
    ret = src;
    src = NULL;
  }

  return ret;
}

char m_start_tag[5];
char m_msg_ID[5];
char m_function[50];
char m_command[50];
char m_additional_code[150];
char m_value[50];
char m_is_crc[5];
char m_crc_value[5];
char m_leftover[10];
char transmitter_ID = _ID_NONE;			//tisti ki posilja zadjne prejeto sporocilo	-- se ponastavi ob vsakem prejetem sporocilu
char reciever_ID = _ID_NONE;					//tisti kateremu je prejeto sporocilo namenjeno -- se ponastavi ob vsakem prejetem sporocilu
int ID;
char additionalCode[2][MAX_ADDITIONAL_COMMANDS][MAX_ADDITIONAL_COMMANDS_LENGTH]; //0 so funkcije 1 pa vrednosti : http://stackoverflow.com/questions/19863417/how-to-declare-and-initialize-in-a-4-dimensional-array-in-c

//---------------------------------lokalne funkcije--------------------------------------------
static void SetID(int id);
static int GetID(void);
static void FindAllAdditionalCmdParameters(char *add_param);

static int ParseMessage(char *m_msg, int transmitter);
static void command_return(uint8_t ans, uint8_t dir);
static void serial_send_handler(char * send_buff, uint16_t buffer_size,uint8_t dir);
static void command_do_events(void);
static bool CheckCRC(unsigned char isCRC, unsigned char CRCvalue, char *message);
static int CalculateCRC(char *message);
static void command_analyze(uint8_t dir);
static void transmit_command_handle(void);
static void serial_send_handler(char * send_buff, uint16_t buffer_size,uint8_t dir);
static void recieved_command_analyze(char *command,uint8_t dir);
//------------------------------------------------------------------------------------------
void ConstructProtocolMessage(char * target_string,MESSAGE_CONSTRUCTOR *instance);
MESSAGE_CONSTRUCTOR CreateCommandInstance(char transmitter, char receiver, char * function, char * command, char * code, char * data);
void SendConstructedProtocolMessage(char * ProtocolMsg, int Receiver,MESSAGE_CONSTRUCTOR *instance);
void ConstructProtocolString(char * target_string,MESSAGE_CONSTRUCTOR *instance);

extern uint32_t global_control;
int delimiterArray[8];

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&/
//&&								FUNKCIJE ZA ODDAJNO CAKALNO VRSTO KOMAND									&&/
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&/
//nalozijo se samo ce TRANSMIT_COMMAND_CHECK ==_ON v serial_com.h
#if TRANSMIT_COMMAND_CHECK == _ON

//++++++++++++++Inicializacija preverjanja poslanih komand++++++++++++++++++++++
void communication_init(void)
{ 
	int g;
	for(g=0;g<TRANSMIT_HANDLE_BUFF_SIZE;g++)
	{
		Transmit_handle_buff[g].message_ID=TRANSMIT_SLOT_FREE;
		Transmit_handle_buff[g].nack_flag=0;
		Transmit_handle_buff[g].nack_count=0;
		Transmit_handle_buff[g].msg_ptr=NULL;
	}
}
//++++++++++++funkcija postavi zastavico nack za sporocilo, ki je dobilo nack++++++++++++++
//funkcija vrne:	0 - vse ok
//								1 - prekoraceno stevilo moznih ponovitev posiljanj
//								2 - sporocila ni v ciklicnem bufferju
//								3 - neznan ID
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static uint32_t recieve_nack_func(void)
{
	char desetica=0;
	char enica=0;
	uint32_t i=0;
	uint32_t id=0;
	uint32_t index=500;
	//pretvorimo dvomestni string v integer
	if(((m_msg_ID[0] >=48) && (m_msg_ID[0] <=57))&&
		((m_msg_ID[1] >=48) && (m_msg_ID[1] <=57)))
	{
		desetica = m_msg_ID[0]-48;
		enica = m_msg_ID[1]-48;
		id = (desetica*10)+enica;
		//preiscemo cakalno vrsto. Ce se v njej nahaja sporocilo s tem id-jem postavimo zastavico za nack
		for(i=0;i<TRANSMIT_HANDLE_BUFF_SIZE;i++)
		{
			if(Transmit_handle_buff[i].message_ID==id)
			{
				index=i;
				i=TRANSMIT_HANDLE_BUFF_SIZE; //skocimo ven iz for zanke
			}
		}
		if(index!=500)
		{
			Transmit_handle_buff[index].nack_flag=1;
		}
		else return 2;	//pomeni da sporocila s takim id-jem ni v cakalni vrsti
	}
	else return 3;
	return 0;
}
//++++++++++++++++++++funkcija izbrise sporocilo iz ciklicnega bufferja+++++++++++++++++++++++
//funkcija vrne:	0 - vse ok
//								2 - sporocila ni v ciklicnem bufferju
//								3 - neznan ID
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static uint32_t recieve_ack_func(void)
{
	char desetica=0;
	char enica=0;
	uint32_t i=0;
	uint32_t id=0;
	uint32_t index=500;
	//pretvorimo dvomestni string v integer
	 if(((m_msg_ID[0] >=48) && (m_msg_ID[0] <=57))&&
		 ((m_msg_ID[1] >=48) && (m_msg_ID[1] <=57)))
	 {
		 desetica = m_msg_ID[0]-48;
		 enica = m_msg_ID[1]-48;
		 id = (desetica*10)+enica;
		 for(i=0;i<TRANSMIT_HANDLE_BUFF_SIZE;i++)
		 {
				if(Transmit_handle_buff[i].message_ID==id)	//poiscemo kje se nahaja sporocilo s tem idjem
				{
					index=i;
					i=TRANSMIT_HANDLE_BUFF_SIZE; //skocimo ven iz for zanke
				}
		 }
		 if(index!=500)
			 Transmit_handle_buff[index].message_ID=TRANSMIT_SLOT_FREE;
		 else return 2;	//pomeni da sporocila s takim id-jem ni v cakalni vrsti
	 }
	 else return 3;
	return 0;
}
//++++++++++++++++++++funkcija postavi sporocilo v ciklicen buffer+++++++++++++++++++++++
//funkcija vrne:	0 - vse ok
//								1 - zmanjkalo je HEAP spomina
//								2 - buffer poln
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static uint32_t transmit_func(uint8_t message_size, uint8_t dir,char * ser_ans_buff, char current_ID, char transmitt_to_ID)
{
	uint32_t i;
	//------------------------------preveri ce je buffer poln-------------------------------
	if((write_count==(TRANSMIT_HANDLE_BUFF_SIZE-1))&&(read_count==0)) return 2;
	else if(write_count==(read_count-1)) return 2;
	//--------------------------------------------------------------------------------------
	//ce je poln ne povecamo stevca ampak smo se zmeri na zadnjem mestu zato je zgoraj return
	Transmit_handle_buff[write_count].size=message_size;
	Transmit_handle_buff[write_count].transmitt_to_ID=transmitt_to_ID;
	Transmit_handle_buff[write_count].message_ID=current_ID;
	Transmit_handle_buff[write_count].dirrection =dir;
	Transmit_handle_buff[write_count].buffer_count = 0;
	Transmit_handle_buff[write_count].msg_ptr=(char*)malloc(message_size+1);
	if(Transmit_handle_buff[write_count].msg_ptr==NULL) return 1;	//v HEAP-u je zmanjkalo spomina
	strcpy(Transmit_handle_buff[write_count].msg_ptr, ser_ans_buff);
	if(write_count>=(TRANSMIT_HANDLE_BUFF_SIZE-1))write_count=0;
	else write_count++;
	//omogoci, da ce posiljamo vec podatkov naenkrat timer zalavfa sele po zadnjem, kar da tudi zadnjemu 10ms casa da dobi ACK
	restart_timer(TRANSMIT_COMMAND_HANDLE,TRANSMIT_HANDLE_WAIT,transmit_command_handle);
	return 0;
}
//funkcija gre po vrsti po bufferju in brise mesta ki so dobila ACK, ko naleti na komando 
//ki se ni dobila ACK, jo ponovno poslje (pri tem pogleda tudi ostale vnose do konca v bufferju
//ce so prejeli slucajno NACK. Ce so jo, jih ponovno poslje).
//Vse skupaj za isto komando ponovi 3x ce v tem casu ne dobi ACK gre naprej na
//naslednji vnos v buffer
static void transmit_command_handle(void)
{
	char temp_array[3];
	uint32_t j=0;
	uint32_t i=0;
	uint32_t command_not_ok_flag=0;
	//preveri ce je buffer prazen
	if(read_count==write_count)
	{
		;
	}
	else
	{
		//ce je prostor na katerega kaze read_count dobil ACKNOWLADGE potem se izvede naslednje,
		//ki sprosti buffer iz HEAP spomina, ki rola toliko casa da izprazne ciklicen buffer. 
		//Ce naleti na poln slot, kar pomeni da to sporocilo se ni dobilo ACK zato ga poslje ponovno
		//nato pa se celoten task izvede ponovno, kar se ponavlja dokler ni buffer za posiljanje povsem prazen
		if(Transmit_handle_buff[read_count].message_ID==TRANSMIT_SLOT_FREE)
		{
			do
			{
				//zbrisemo vnos v buffer
				free(Transmit_handle_buff[read_count].msg_ptr);
				Transmit_handle_buff[read_count].message_ID=TRANSMIT_SLOT_FREE;
				Transmit_handle_buff[read_count].transmitt_to_ID=0;
				Transmit_handle_buff[read_count].nack_flag=0;
				Transmit_handle_buff[read_count].nack_count=0;
				Transmit_handle_buff[read_count].buffer_count = 0;
				Transmit_handle_buff[read_count].msg_ptr=NULL;
				Transmit_handle_buff[read_count].size=0;
				
				//postavimo se na naslednji vnos bufferja in preverimo ce je tudi ta prazen ce ni zapustimo zanko
				if((read_count+1)==TRANSMIT_HANDLE_BUFF_SIZE)read_count=0;//gre na zacetek ker je ciklicen buffer
				else read_count++;
				if(read_count==write_count) break;						//buffer spraznjen
			}while(Transmit_handle_buff[read_count].message_ID==TRANSMIT_SLOT_FREE);
			//ce pridemo iz zgornje funkcije preden se buffer sprazni pomeni da smo naleteli na ID, ki se ni dobil ACK zato ga ponovno posljemo
			if(read_count!=write_count)
				if(Transmit_handle_buff[read_count].buffer_count>WAIT_FOR_MSG_ACK)
					command_not_ok_flag=1;
		}
		else 
			if(Transmit_handle_buff[read_count].buffer_count>WAIT_FOR_MSG_ACK)
				command_not_ok_flag=1;
		restart_timer(TRANSMIT_COMMAND_HANDLE,TRANSMIT_HANDLE_WAIT,transmit_command_handle);
		//ce buffer se ni prazen ponovno zazeni funkcijo
			
	}
	if(command_not_ok_flag==1)
	{
		j=0;
//	do	//tole bi prslo v postev ce zelimo pogledat se po cakalni vrsti naprej in poslati vse ki se niso dobili ACK v enem tasku
//	{
//		if(Transmit_handle_buff[read_count+j].message_ID!=TRANSMIT_SLOT_FREE)
		serial_send_handler(Transmit_handle_buff[read_count+j].msg_ptr,(Transmit_handle_buff[read_count+j].size),Transmit_handle_buff[read_count+j].dirrection);	//funkcija izbere vodilo glede na dir in poslje sporocilo
		//serial_send_handler("5555555555555555555555555555555555555555555555555555555555555555",64,Transmit_handle_buff[read_count+j].dirrection);// pazi 64 bytni bufferji ne delajo dobr
		
		//ce presezemo stevilo ponovitev
		if(Transmit_handle_buff[read_count+j].nack_count>=(NUM_NACK_EVENTS-1)) 
		{
			//pretvorba int v str veliko hitrejsa kot sprintf
			if(Transmit_handle_buff[read_count+j].message_ID<100)
			{
				temp_array[0]=(char)(Transmit_handle_buff[read_count+j].message_ID/10)+48;
				temp_array[1]=(char)(Transmit_handle_buff[read_count+j].message_ID%10)+48;
				temp_array[2]=0;
			}
			else
			{
				temp_array[0]='/';
				temp_array[1]='/';
				temp_array[2]=0;
			}
			//*******************************************************************
			//******dolocimo kaj se zgodi ce posiljanje sporocila ne uspe********
			//*******************************************************************
			//sprintf(temp_array,"%u.2",Transmit_handle_buff[read_count].message_ID);
			char additional_command[3];
			sprintf(additional_command,"%.2d",Transmit_handle_buff[read_count+j].message_ID);
			//ce zelimo poslati warning z send kontrolom mora biti zacetni parameter spodaj _ON
			SendComMessage(_ON,_ID_TFA,Transmit_handle_buff[read_count+j].transmitt_to_ID,_MESSAGE_WAR,_MESSAGE_COM_ERR,additional_command,"",Transmit_handle_buff[read_count+j].dirrection);
			//send_com_message(_MESSAGE_WAR,_MESSAGE_COM_ERR,_VALID,temp_array,Transmit_handle_buff[read_count+j].dirrection);
			//ce posiljanje v tretje ne uspe zbrisemo podatke in gremo naprej
			free(Transmit_handle_buff[read_count+j].msg_ptr);
			Transmit_handle_buff[read_count+j].message_ID=TRANSMIT_SLOT_FREE;
			Transmit_handle_buff[read_count+j].transmitt_to_ID=0;
			Transmit_handle_buff[read_count+j].nack_flag=0;
			Transmit_handle_buff[read_count+j].nack_count=0;
			Transmit_handle_buff[read_count+j].buffer_count = 0;
			Transmit_handle_buff[read_count+j].msg_ptr=NULL;
			Transmit_handle_buff[read_count+j].size=0;
			if((read_count+1)==TRANSMIT_HANDLE_BUFF_SIZE)read_count=0;//gre na zacetek ker je ciklicen buffer
			else read_count++;
			//*********************************************************************
		}
		else 
		{
			Transmit_handle_buff[read_count].nack_count++;
	//		i++;
	//	}while((read_count+j)!=write_count);
			
			//pogleda se naslednje komande v bufferju ce so slucajno dobili NACK in ponovi posiljanje se za te
			if((read_count+1)==TRANSMIT_HANDLE_BUFF_SIZE)i=0;//gre na zacetek ker je ciklicen buffer
			else i=read_count+1;
			for(;i!=write_count;)
			{
				//pogleda ce ima postavljeno NACK zastavico, ce jo ima se sporocilo ponovno poslje in zastavica pobrise
				if(Transmit_handle_buff[i].nack_flag==1)
				{
					serial_send_handler(Transmit_handle_buff[i].msg_ptr,Transmit_handle_buff[i].size,Transmit_handle_buff[i].dirrection);	//funkcija izbere vodilo glede na dir in poslje sporocilo
					Transmit_handle_buff[i].nack_flag=0;
					Transmit_handle_buff[i].nack_count++;
				}
				if((i+1)==TRANSMIT_HANDLE_BUFF_SIZE) i=0;
				else i++;
			}
		}
	}
	for (int k=0; k< TRANSMIT_HANDLE_BUFF_SIZE;k++)
    if(Transmit_handle_buff[k].message_ID != TRANSMIT_SLOT_FREE)
       Transmit_handle_buff[k].buffer_count++;
}
#endif


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


//+++++++++++++++++++++++analiziranje komand+++++++++++++++++++++++++++++++++++++
//funkcijo si prilagodi sak sam
/*******************************************************************************/
/**															ANALIZE COMMAND																**/
/*******************************************************************************/
static void command_analyze(uint8_t dir)
{
	commandAnalzye(dir);
}





//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&/
//&&								FUNKCIJE ZA SPREJEMNO CAKALNO VRSTO KOMAND								&&/
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&/

//inicializacija cakalne vrste komand
void serial_com_init(void)
{
	uint8_t queue_cnt;
	event_status=QUEUE_PASS; //first do event is enabled 		
	
	queue_cnt=0;
	while(queue_cnt<SERIAL_IN_QUEUE_SIZE)
  {
   SERIAL_in_queue_buffer[queue_cnt].command_ID=QUEUE_FREE;
   queue_cnt++;
  }
	#if TRANSMIT_COMMAND_CHECK == _ON	
	communication_init();
	#endif
	if(SYNCHRONUS_TRANSMITT == _ON)
	{
		SendTimerInit();
		SynchronusSendInit();
		set_event(SYNCHRONUS_PROCESS,SynchronusProcess);
	}
	
	//koda uporabnika
	connection_control |= __SERIAL_INITIATED;
}

void serial_com_deinit(void)
{
	uint8_t i;
	uint8_t queue_cnt;
	queue_cnt=0;
	while(queue_cnt<SERIAL_IN_QUEUE_SIZE)
  {
   SERIAL_in_queue_buffer[queue_cnt].command_ID=QUEUE_FREE;
   queue_cnt++;
  }
	if(SYNCHRONUS_TRANSMITT == _ON)
	{
		SendTimerDeInit();
		for(i=0;i<TRANSMIT_OUT_BUFF_SIZE;i++)
		{
			if(Transmit_out_buff[i].msg_ptr != NULL)
			{
				free(Transmit_out_buff[i].msg_ptr);
				Transmit_out_buff[i].msg_ptr = NULL;
			}
		}
	}
	#if TRANSMIT_COMMAND_CHECK == _ON	
	for(i=0;i<TRANSMIT_HANDLE_BUFF_SIZE;i++)
  {
		if(Transmit_handle_buff[i].message_ID != TRANSMIT_SLOT_FREE)
		{
			free(Transmit_handle_buff[i].msg_ptr);
			Transmit_handle_buff[i].message_ID=TRANSMIT_SLOT_FREE;
			Transmit_handle_buff[i].transmitt_to_ID=0;
			Transmit_handle_buff[i].nack_flag=0;
			Transmit_handle_buff[i].nack_count=0;
			Transmit_handle_buff[i].buffer_count = 0;
			Transmit_handle_buff[i].msg_ptr=NULL;
			Transmit_handle_buff[i].size=0;
		}
  }
	#endif
	
	//koda uporabnika
	connection_control &= ~__SERIAL_INITIATED;
}
//postavi prejeto komando v cakalno vrsto za analiziranje
//to funkcijo poklices ko prejmes komando iz vodila.
//funkcija postavlja sporocilo v buffer dokler ne dobi \r\n zato lahko komando dodamo tudi po delih
// & Buf - pointer na prejeti buffer
// & Len - dolzina prejetega arraya v Buf
// & dir - int vrednost, ki nam pove iz katerega vodila je komanda prispela, da jo znamo potem tudi vrniti
//!!NE POZABI DEFINIRATI DIR SMERI V serial_com.h
void add_command_to_queue(uint8_t* Buf, uint32_t Len,uint8_t dir)
{
		uint8_t queue_cnt;	
		uint8_t queue_index;	
		uint32_t cnt;	
		uint8_t status;
		static uint32_t counter=0;//counter, ki steje kok polhn je polje
		static uint32_t temp;
		static uint32_t temp1;
		static uint8_t * RxBuffer=INPUT_Buffer;
		for(cnt=0;cnt<Len;cnt++)
		{
			temp1=*(RxBuffer-1);	//predhodna vrednost
			*RxBuffer=Buf[cnt];
			temp=*RxBuffer;
			if(++RxBuffer >= &INPUT_Buffer[RxBufferSize_MAX]) 
			{
				RxBuffer=INPUT_Buffer;	//overflow -> init pointerja
				counter=0;
			}	
			else counter++;
		}
		
		//ko dobi terminatorje skoci tle not
		if((temp1==SERIAL_TERM_CHARACTER_13) && (temp==SERIAL_TERM_CHARACTER_10))    
		{    
			//nadomesti /13 in /10 z 0
			uint8_t * start_sign_position;
			*(RxBuffer-1)=0;
			*(RxBuffer-2)=0;
			for(cnt=0;cnt<(counter-2);cnt++)
			{
				if(INPUT_Buffer[cnt]=='>')
				{
					RxBuffer=&INPUT_Buffer[cnt]; //ponastavi ptr na zacetno lokacijo za kopiranje komande
					start_sign_position=RxBuffer;
					//ko se funkcija konca spet zacne pisat na zacetk bufferja
					cnt=counter;
				}
				else 
				{
					RxBuffer=INPUT_Buffer; //ce ne najde start sajna, potem se postavi na zacetk
					start_sign_position=RxBuffer;
				}
			}
//			RxBuffer = INPUT_Buffer; //ponastavi ptr na zacetno lokacijo
//			 //ko se if stavek konca spet zacne pisat na zacetk
			//RxBuffer=&Buf[0];  //init pointerja                                           
			
			//find command index - (max. index + 1)
			//--------------------------------------          
			queue_index=0;    
			for(queue_cnt=0;queue_cnt<SERIAL_IN_QUEUE_SIZE;queue_cnt++)
			 {
				if(SERIAL_in_queue_buffer[queue_cnt].command_ID>queue_index) queue_index=SERIAL_in_queue_buffer[queue_cnt].command_ID; 			 			 		
			 }
			queue_index+=1;		 
			//--------------------------------------
					 
			//put command to queue - to first free slot
			//--------------------------------------      
			queue_cnt=0;
			status=QUEUE_FAIL; 
			while((queue_cnt<SERIAL_IN_QUEUE_SIZE) && (status==QUEUE_FAIL))
			{
				if(SERIAL_in_queue_buffer[queue_cnt].command_ID==QUEUE_FREE)
				{
					SERIAL_in_queue_buffer[queue_cnt].command_ID=queue_index; //index is: max. index before this + 1 
					SERIAL_in_queue_buffer[queue_cnt].direction = dir;	//vpise iz kje je bila prejeta komanda, da ve vrnit ack po istem vodilu
					for(cnt=0;cnt<(counter-(start_sign_position-INPUT_Buffer+2));cnt++)	//kopira komando v cakalno vrsto (odstejemo naslova '>' in zacetka bufferja zraven pa se 2 zaradi dveh delimiterjev. tako dobimo dolzino stringa protokola
					{
						SERIAL_in_queue_buffer[queue_cnt].command[cnt]=*RxBuffer;
						RxBuffer++;
					}
					for(;cnt<SERIAL_COMMAND_SIZE;cnt++)
					{
						SERIAL_in_queue_buffer[queue_cnt].command[cnt]=0;
					}
					//strcpy(SERIAL_in_queue_buffer[queue_cnt].command,(char *)INPUT_Buffer); 
					status=QUEUE_PASS; 
				}
				queue_cnt++;
			}	
			RxBuffer = INPUT_Buffer;	//postavi ptr na zacetk, kjer caka na nove komande za vpis v njega
			//--------------------------------------
			
			if((status==QUEUE_PASS) && (event_status==QUEUE_PASS)) 
			{ 
				set_event(COMMAND_DO_EVENTS,command_do_events); //UART3 command do events call		 
			}	
			counter=0;
			//timer za izmenjevanje receiverja in transmiterja ponastavimo vsakic ko dobimo komando in tako podaljsamo cas delovanja kot receiver
			//ce dalj casa ne dobimo nobenega sporocila preidemo v stanje transmitterja ob prekinitvi na timerju
			
		}	
		if(SYNCHRONUS_TRANSMITT == _ON)
			{
				resetSendTimer();
				serialComErrorTimeout=0;
				serialComErrorDetected = false;
			}
}
//commands do events
//gre po vseh slotih v polju in najde command z najmanjsim indeksom in izvede ta command string
//to ponavlja dokler je v polju prisotna vsaj en command
//ko sem prisel v to funkcijo je zanesljivo vsaj en command v polju
static void command_do_events(void)
{
 #define QUEUEUE_INDEX_MAX		255
	
 uint8_t queue_cnt;	 
 uint8_t queue_index;	
 uint8_t queue_index_min;		
 event_status=QUEUE_FAIL;
 while(event_status==QUEUE_FAIL)
   {
    
		//find pointer to the command with minimum index - oldest command, first to do event
		//--------------------------------------      
		queue_index=QUEUEUE_INDEX_MAX;    
    for(queue_cnt=0;queue_cnt<SERIAL_IN_QUEUE_SIZE;queue_cnt++)
     {
      if(SERIAL_in_queue_buffer[queue_cnt].command_ID!=QUEUE_FREE) 
			 {
			  if(SERIAL_in_queue_buffer[queue_cnt].command_ID<queue_index)
				 {
					queue_index=SERIAL_in_queue_buffer[queue_cnt].command_ID;
					queue_index_min=queue_cnt; 
				 } 
			 }
     }
    //--------------------------------------      
	   	
		//ce je queue_index ostal na QUEUEUE_INDEX_MAX, potem v polju ni nobenega ukaza vec in lahko prekinemo while zanko
		//drugace pa izvedemo ukaz na katerega kaze pointer queue_index_min in polje nardimo prosto
    //--------------------------------------      
    if(queue_index!=QUEUEUE_INDEX_MAX)
		 {
		  recieved_command_analyze(SERIAL_in_queue_buffer[queue_index_min].command,SERIAL_in_queue_buffer[queue_index_min].direction); 			
			SERIAL_in_queue_buffer[queue_index_min].command_ID=QUEUE_FREE; 
		 }
		else
		 {
		  event_status=QUEUE_PASS;
		 }			 
    //--------------------------------------       		 
		 		      
  }
}
//UART2 command analyze
static void recieved_command_analyze(char *command,uint8_t dir)
{
	//zaenkat se zna bit problem, ce poslje hkrati po dveh vodilih npr PAT in MT skupaj
	uint8_t ans; 
	ans=ParseMessage(command,dir);  
	if(ans == _ANS_MESSAGE_ACK)				//najprej vrnemo ACK nato sele delamo ostalo			
	{
		command_return(ans,dir); 
	  command_analyze(dir);  //message analyze 
	}	
	else if(ans == _ANS_MESSAGE_NACK)
		command_return(ans,dir); 				//vrnemo NACK
	#if TRANSMIT_COMMAND_CHECK == _ON	
	else if(ans==_ANS_MESSAGE_ACK_RECIEVED) 
		recieve_ack_func();
	else if(ans==_ANS_MESSAGE_NACK_RECIEVED) 
		recieve_nack_func();
	#endif
	//ce je v sporocilu acknowladge ali NACK potem ne vracamo ACK sporocila
  
}
static int CalculateCRC(char *message)
{
//	int vsota = 0;	
//	for (int i = 0 ; i < strlen(message); i++)
//		vsota += message[i];
//		
//	return (vsota%256);
	
	int podpi = 0;
	int vsota = 0;
	indikator3 = strlen(message);
	for(int a =0;a < strlen(message); a++)
	{
		if (message[a] == ':')
		{
			delimiterArray[podpi] = a;
			podpi++;
			indikator2= a;
		}		
	}
	delimiterArray[podpi] =  strlen((char*)message);
	for(signed int a =0; (a <  delimiterArray[6]+1 && a < (signed int)strlen(message)); a++)
	{
		vsota = vsota+message[a];
		indikator4= vsota;
		indikator5 = message[a];
	}
	
	return (vsota%256);
}

static bool CheckCRC(unsigned char isCRC, unsigned char CRCvalue, char *message)
{
	int CRCresult=0;
	if(isCRC == 1)
	{
		if(CRCvalue<=255) //CRCvalue>=0 && CRCvalue<=255
		{
			CRCresult= CalculateCRC(message);	   // racunamo crc do petega delimiterja
			if(CRCresult ==	CRCvalue)
				return true;
			else
				return false;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return true;
	}
}

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&/
//&&								FUNKCIJE ZA POSILJANJE KOMANDE														&&/
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&/


//tole funkcijo najbols da si prilagodi vsak sam
//vse kar nardi je to da poslje sporocilo po pravem portu, ki ga izbere glede na dir
//pazi da ni string, ki ga posiljas dolg 64 bytov, ker ne bo delal prov !!!!!!!!!!!!!!!
static void serial_send_handler(char * send_buff, uint16_t buffer_size,uint8_t dir)
{
//	if(buffer_size==64)//neki zajebava ce je 64 dolg
//	{
//		//CDC_Transmit_FS((uint8_t *)"DOLZINA BUFERJA JE 64 BITOV - PROSIM SPREMENI KER NE BO DELAL", 61);
//	}
	if((SYNCHRONUS_TRANSMITT == _ON))// && (connection_control == __CONNECTION_ESTABLISHED))
	{
			//------------------------------preveri ce je buffer poln-------------------------------
		if((write_out_count==(TRANSMIT_OUT_BUFF_SIZE-1))&&(read_out_count==0)) return;
		else if(write_out_count==(read_out_count-1)) return;
		//--------------------------------------------------------------------------------------
		//ce je poln ne povecamo stevca ampak smo se zmeri na zadnjem mestu zato je zgoraj return
		Transmit_out_buff[write_out_count].size=buffer_size;
		Transmit_out_buff[write_out_count].dirrection =dir;
		Transmit_out_buff[write_out_count].msg_ptr=(char*)malloc(buffer_size+1);
		if(!(Transmit_out_buff[write_out_count].msg_ptr==NULL))	//v HEAP-u je zmanjkalo spomina
			strcpy(Transmit_out_buff[write_out_count].msg_ptr, send_buff);
		else 
			return;
		increaseWritePtr();
	}
	else	//ce nimamo vklopljenega sinhroneha posiljanja in sprejemanja potem posljemo takoj
	{
		SerialSend((uint8_t *)send_buff, buffer_size,dir);
	}
	
}
//+++++++++++++++++++++funkcija za posiljanje sporocila po protokolu++++++++++++++++++++++++++++
// & send_control: 	1 - pomeni da uporabimo preverjanje ce je bilo sporocilo poslano (transmitt control)
//								 	0 - pomeni da samo posljemo sporocilo brez postavitve v cakalno vrsto
// & transmitter:	 	char_ID tistega kateremu je sporocilo namenjeno	
// & receiver:	 		char_ID tistega kateremu ki posilja sporocilo
// & function,command, data : pointerji na string za posiljanje
// & dirrection			int ID vodila oziroma smer v kateo hocemo poslati rabimo jo za posiljanje, glej :serial_send_handler
// & additional_code pointer na srting v katerem je dodatna koda. Tukaj lahko posljemo vec vrednosti. Ime parametra in njegovo vrednost locimo z '|' dva parametra pa z ','
// -> vrne: ID, ki ga dodeli sporocilu
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

uint32_t SendComMessage(int send_control,char transmitter, char receiver, char * function, char * command, char * additional_code, char * data, int dirrection)
{
	char temp_str[_SER_BUFFER_SIZE];
	if(receiver == 0)					//ce reciver ni defineiran potem posljemo tistemu, ki nam je poslal zadni msg
		receiver= transmitter_ID;
	if(transmitter == 0)
		transmitter = _ID_TFA;	//ce transmitter ni definiran potem nastavimo da je to TFA spremeni ce zelis drugace
	
	MESSAGE_CONSTRUCTOR message_inst = CreateCommandInstance(transmitter,receiver,function,command,additional_code,data);
	ConstructProtocolString(temp_str,&message_inst);
	if((TRANSMIT_COMMAND_CHECK == _ON))// && (connection_control == __CONNECTION_ESTABLISHED))
		SendConstructedProtocolMessage(temp_str,dirrection,&message_inst);
	else 
		serial_send_handler(temp_str,strlen(temp_str),dirrection);
	return message_inst.msg_ID;
}
//+++++++++++++++++++++funkcija preverja ali je sporocilo ze dobilo ACK++++++++++++++++++++++++++
// & msg_id: 	ID sporocila, za katerega nas zanima ce je ze dobil acknowladge
//	-> vrne:	true 	- ce je sporocilo dobilo ACK
//						false - ce funkcija se ni dobila ACK
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#if TRANSMIT_COMMAND_CHECK == _ON
bool CheckIfACK(uint32_t msg_id)
{
	uint32_t i;
	for(i=0;i<TRANSMIT_HANDLE_BUFF_SIZE;i++)
		if(Transmit_handle_buff[i].message_ID == msg_id) 
			return false;
	return true;
}
#endif
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MESSAGE_CONSTRUCTOR CreateCommandInstance(char transmitter, char receiver, char * function, char * command, char * code, char * data)
{
	static char msg_count=1;
	MESSAGE_CONSTRUCTOR temp_constructor;
	temp_constructor.transmitter = transmitter;
	temp_constructor.reciever = receiver;
	temp_constructor.msg_ID = msg_count;
	temp_constructor.replay_function = function;
	temp_constructor.replay_cmd=command;
	temp_constructor.replay_additional_code=code;
	temp_constructor.replay_data=data;
	if(msg_count >= 99) msg_count = 1;
	else msg_count++;
	return temp_constructor;
}
void ConstructProtocolString(char * target_string,MESSAGE_CONSTRUCTOR *instance)
{
	static char CRCval[10];
	char start_sign[4];
	uint32_t temp_int=0;
	int krneki;
	int krneki2;
	memset(CRCval, 0 , sizeof(CRCval));
	start_sign[0]=_SER_START_SIGN;
	start_sign[1]=instance ->transmitter;
	start_sign[2]=instance ->reciever;
	start_sign[3]=0;
	snprintf(target_string, (_SER_BUFFER_SIZE-7), "%s:%.2d:%s:%s:%s:%s:1:", start_sign, instance -> msg_ID, instance->replay_function, instance-> replay_cmd, instance-> replay_additional_code, instance ->replay_data);		
	temp_int = CalculateCRC(target_string);
	if(temp_int<10)
		snprintf(CRCval, 7,"00%d:\r\n", temp_int);
	else if(temp_int<100)
		snprintf(CRCval, 7,"0%d:\r\n", temp_int);
	else
		snprintf(CRCval, 7,"%d:\r\n", temp_int);
	strcat(target_string, CRCval);
}
void SendConstructedProtocolMessage(char * ProtocolMsg, int Receiver,MESSAGE_CONSTRUCTOR *instance)
{
	//preverimo ce je vse nastavljeno, ce ni potem ne naredimo nic
		serial_send_handler(ProtocolMsg,(strlen(ProtocolMsg)),Receiver);
		#if TRANSMIT_COMMAND_CHECK == _ON
		//postavimo komando v ciklicen buffer za oddajanje
		switch(transmit_func(strlen(ProtocolMsg),Receiver,ProtocolMsg,instance->msg_ID,instance ->reciever))
		{
			//zmanjkalo je HEAP spomina
			case 1:
				break;		
			//buffer poln
			case 2:
				break;
		default:
			break;
		}
		#endif
}
//-------------------FUNKCIJA ZA PARSANJE VHODNIH PODATKOV-----------------
//funkcija vrne 0 - vse OK
//              1 - ni OK NACK
//              2 - ACK recieved
//              3 - NACK recieved
//              4 - ni zame - v tem primeru ne naredimo nic (torej ne vracamo ACK ali NACK)
//							5 - prazen string
static int ParseMessage(char *m_msg, int transmitter)
{
		char message[200];
//		char message2[200];//
		int crc;
		int crcVal;
		char *IN_START_TAG;
		char *token;
		int array_nr = 0;
	  
		strcpy(message, m_msg);
		memset(m_start_tag, 0, sizeof(m_start_tag));
		memset(m_msg_ID, 0, sizeof(m_msg_ID));
		memset(m_function, 0, sizeof(m_function));
		memset(m_command, 0, sizeof(m_command));
		memset(m_additional_code, 0, sizeof(m_additional_code));
		memset(m_value, 0, sizeof(m_value));
		memset(m_is_crc, 0, sizeof(m_is_crc));
		memset(m_crc_value, 0, sizeof(m_crc_value));
		memset(m_leftover, 0, sizeof(m_leftover));
		
		token = strtok_single(message, _SER_DATA_DELIM_STR);
		while (token != NULL)
		{
			switch(array_nr)
			{
				case 0:
					strcpy(m_start_tag, token);
				break;
				case 1:
					strcpy(m_msg_ID, token);
				break;
				case 2:
					strcpy(m_function, token);
				break;
				case 3:
					strcpy(m_command, token);
				break;
				case 4:
					strcpy(m_additional_code, token);			
				break;
				case 5:
					strcpy(m_value, token);
				break;
				case 6:
					strcpy(m_is_crc, token);
				break;
				case 7:
					strcpy(m_crc_value, token);
				break;
				case 8:
					strcpy(m_leftover, token);
				break;
			}					
				
			token = strtok_single(NULL, _SER_DATA_DELIM_STR);
			array_nr++;
		}
	//ce ni 8 delimiterjev je nekje napaka
	if(array_nr<8)
		return _ANS_MESSAGE_NACK;
	reciever_ID=m_start_tag[2];
	transmitter_ID=m_start_tag[1];
	
	//ce ni zame skocmo vn
	if(reciever_ID != _ID_TFA)
		return 4;
	
	SetID(atoi(m_msg_ID));
	if((strlen(m_msg_ID)<2)&&(strlen(m_function)<2))//preverimo ce smo dobil empty str
		return 5;
	else if (!(GetID()>=0 && GetID() < 100))				//ce ni empty str preverimo ce je ID pravilen
		return _ANS_MESSAGE_NACK;
	if(!(strcmp(m_function,_MESSAGE_ACK)))
		return _ANS_MESSAGE_ACK_RECIEVED;
	else if(!(strcmp(m_function,_MESSAGE_NACK)))
		return _ANS_MESSAGE_NACK_RECIEVED;
	
	FindAllAdditionalCmdParameters(m_additional_code);
	
//	if (!CheckFunctionExistion(m_function))
//		return 1;
//	
//	if(!CheckCommandExistion(m_command))
//		return 1;
	
	crc = atoi((char*)m_is_crc);
	crcVal = atoi((char*)m_crc_value);
	if (!CheckCRC(crc, crcVal, m_msg))
		return _ANS_MESSAGE_NACK;
	
	return _ANS_MESSAGE_ACK;
}

static void SetID(int id)
{
	ID = id;
}

static int GetID(void)
{
	return ID;
}

static void FindAllAdditionalCmdParameters(char *add_param)
{
	int valueCntr = 0;
	int funcCntr = 0;
	int valuepos = 0;
	int funcpos = 0;
	bool FillFunctionArray = true;
	
	for (int i = 0; i < strlen(add_param); i++)
	{
		if (add_param[i] == '|')
		{
			FillFunctionArray = false;
			continue;
		}
		
		if (add_param[i] == ',')
		{
			additionalCode[0][funcCntr][funcpos] = 0;
			additionalCode[1][valueCntr][valuepos] = 0;
			
			funcCntr++;  valueCntr++;		
			funcpos = 0; valuepos = 0;
			FillFunctionArray = true;
			continue;
		}
		
		if (FillFunctionArray == true)
		{
			additionalCode[0][funcCntr][funcpos] = add_param[i];
			funcpos++;
			additionalCode[0][funcCntr][funcpos]=0;
		}
		else
		{
			additionalCode[1][valueCntr][valuepos] = add_param[i];
			valuepos++;
			additionalCode[1][valueCntr][valuepos] = 0;
		}
	}	
}

//return answer (acknowledge or not acknowledge)
static void command_return(uint8_t ans, uint8_t dir)
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
 ser_ans_buffer[2]= transmitter_ID;
 ser_ans_buffer[3]=_SER_DATA_DELIMITER;  
 ser_ans_buffer[4]=0;  	
 strcat(ser_ans_buffer,m_msg_ID);		
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
 checksum = 0;
 checksum=CalculateCRC(ser_ans_buffer);
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
  
 //tukaj dodamo tiste pogoje pri katerih ne zelimo vracati ACK
	if(!(!strcmp(m_command,_SER_TELEGRAM_CALIB))) //zaenkrat samo v kalibraciji ne posiljam acknowledge stringa
  {    
		serial_send_handler(ser_ans_buffer,cd,dir);
  }

}

static void increaseWritePtr(void)
{
	if((write_out_count==(TRANSMIT_OUT_BUFF_SIZE-1))&&(read_out_count==0)) ;//buffer je poln
	else if(write_out_count==(read_out_count-1));//buffer je poln
	//ce je poln ne povecamo stevca ampak smo se zmeri na zadnjem mestu
	else if(write_out_count>=(TRANSMIT_OUT_BUFF_SIZE-1))write_out_count=0;//povecanje stevca ce je buffer poln
	else write_out_count++;
}
//RS485 TIMER
void TIM2_IRQHandler(void)  
{
	HAL_TIM_Base_Stop_IT(&htim2);
	restart_timer(SINCHRONUS_TRANSMITT, 1,SinchronusTransmittFunc);
}

//ko je procesor na vrsti za posiljanje posilja dokler ne izprazni izhodnega bufferja
static void SinchronusTransmittFunc(void)
{
	if(write_out_count != read_out_count)//izvede se ce buffer ni prazen
	{
		SerialSend((uint8_t *)Transmit_out_buff[read_out_count].msg_ptr, Transmit_out_buff[read_out_count].size,Transmit_out_buff[read_out_count].dirrection);
		//--------povecanje read pointerja ciklicnega bufferja------------
		free(Transmit_out_buff[read_out_count].msg_ptr);								//sprostitev alociranega spomina
		Transmit_out_buff[read_out_count].msg_ptr=NULL;
		Transmit_out_buff[read_out_count].size=0;
		Transmit_out_buff[read_out_count].dirrection = _EMPTY_DIR;
		if((read_out_count+1)==TRANSMIT_OUT_BUFF_SIZE)read_out_count=0;	//gre na zacetek ker je ciklicen buffer
		else read_out_count++;
		//----------------------------------------------------------------
		//printToDebugMsg
		if(write_out_count != read_out_count)
			restart_timer(SINCHRONUS_TRANSMITT,1, SinchronusTransmittFunc);
	}
	//ce v izhodnemu bufferju ni nic posljemo prazen string
	else
		transmittEmptyStr();
}
//oddajanje praznega stringa
static void transmittEmptyStr(void)
{
	char tmp_msg[18];
//	uint32_t dir;
	snprintf(tmp_msg,18,">%c%c::::::0:000:%c%c",_ID_TFA,transmitter_ID,'\r','\n');
//	strcat(tmp_msg,(char*)_ID_TFA);
//	strcat(tmp_msg,&transmitter_ID);
//	strcat(tmp_msg,"::::::0:000:");
//	strcat(tmp_msg,(char*)13);
//	strcat(tmp_msg,(char*)10);
//	strcat(tmp_msg,"\0");
	SerialSend((uint8_t *)tmp_msg, 18,_EMPTY_DIR);	//zadnji podatek "dir" je namenjen identifikaciji posiljanja praznega stringa dir spremenljivko v tem primeru v funkciji SerialSend(..) nastavimo po svoje
}
static void SynchronusSendInit(void)
{
	unsigned int i;
	read_out_count=0;
	write_out_count=0;
	synchronus_error_count=0;
	for(i=0;i<TRANSMIT_OUT_BUFF_SIZE;i++)
	{
		Transmit_out_buff[i].msg_ptr = NULL;
		Transmit_out_buff[i].dirrection = _EMPTY_DIR;
		Transmit_out_buff[i].size = 0;
	}
}
void SerialSend(uint8_t * msg_ptr,uint16_t msg_size, uint32_t dir)
{
	uint8_t result;
//	char test_str[20];
//	snprintf(test_str,20,"adr:%x",(unsigned int)msg_ptr);
//	printToDebugMsg((char*)msg_ptr);
	if(connection_control & __CONNECTION_ESTABLISHED)
	{
		//smer ce posiljamo prazen string je dir spremenljivka brezpredmetna, zato ji znova dolocimo vrednost
		if(dir == _EMPTY_DIR)
		{
			//v mojem primeru se za smer posiljanja praznega stringa odlocim glede na posiljatelja zadnjega prejetega sporocila
			if(transmitter_ID==_ID_NONE || transmitter_ID==_ID_DEBUG || transmitter_ID == _ID_PAT || transmitter_ID == _ID_SIMULATION)
				dir = _UART_DIR_USB;
			else	//velja za MT
				dir = _UART_DIR_485;
		}
		if(dir == _UART_DIR_MT || dir == _UART_DIR_DEBUG || dir==_UART_DIR_USB)
		{
	//		for(int i=0; i <= (msg_size/10);i++)
	//		{
			
			if(hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED)
			{
//				//zajebava ce je string dolg 64, zato posljemo 2x pa se zmeri jebe
//				if(msg_size==64)
//				{
//					do
//					{
//						result = CDC_Transmit_FS(msg_ptr, (msg_size/2));
//					}while(result == USBD_BUSY);
//					do
//					{
//						result = CDC_Transmit_FS((msg_ptr+(msg_size/2)), msg_size/2);
//					}while(result == USBD_BUSY);
//				}
//				else
//				{
						do
						{
							result = CDC_Transmit_FS(msg_ptr, msg_size);//dokler ni enako 0
							//result = CDC_Transmit_FS(msg_ptr+(i*10), 10);//dokler ni enako 0
							indikator1=result;
						}while(result == USBD_BUSY);
	//				}
			}
	//		}
		}
	}
	
	
//	else if(dir == neki_tvojga || ...)
//		;
//	else if
//		...
}
void SynchronusProcess(void)
{
	if(serialComErrorTimeout >= SERIAL_COM_ERROR_TIMEOUT)
	{
		if(synchronus_error_count>=SYNCHRONUS_ERROR_CNT)
		{
			serialComErrorDetected = true;
			//serial_com_deinit();
			//zaenkrat tkole pol bo treba spremenit
			disconnect_function(__CON_TO_MT310);
		}
		else
		{
			transmittEmptyStr();
		}			
		synchronus_error_count++;
		serialComErrorTimeout=0;
	}
	if(serialComErrorDetected==false)
		restart_timer(SYNCHRONUS_PROCESS,5,SynchronusProcess);
}
//Inicializacija timerja za sinhrono posiljanje
//Ce zelis definirati drug timer ali uporabljas drug procesor potem napisi svojo inicializacijo, ki ima enako ime kot ta
//ker je funkcija definirana kot __weak, bo na novo definirana funkcija zamenjala spodnjo. Timer inicializiraj s podobnimi nastavitvami kot v tem primeru. 
__weak void SendTimerInit(void)
{
	/* TIM2 init function -- v mojem primeru uporabljen timer 2*/ 
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 71;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = SYNCHRONUS_SEND_WAIT*1000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	HAL_Delay(100);

//  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
//  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	HAL_Delay(100);
}
__weak void SendTimerDeInit(void)
{
	if (HAL_TIM_Base_DeInit(&htim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}

static void resetSendTimer(void)
{
	htim2.Instance -> CNT=0;				//postavi counter na 0
	HAL_TIM_Base_Start_IT(&htim2);	//ponovno zazene interrupt
}