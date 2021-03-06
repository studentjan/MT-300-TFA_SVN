// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define VybridVFxx		//toradex VybridVFxx modul	// Test for Vybrid module	---> T20 and VybridVFxx Are mutually excluded if one is selected other should be commented
//#define T20					//toradex T20 modul			// Test for t20 module		---> T20 and VybridVFxx Are mutually excluded if one is selected other should be commented
//#define iMX6 

#define THREAD_PRIORITY_CE 140

//setting for Qwerty texts
#define MAX_COMMENT_LENGHT	500
#define MAX_NAME_LENGHT	29
#define MAX_DESCRIPTION_LENGHT	MAX_COMMENT_LENGHT

//#define MAX_ENTRIES_FROM_DATABASE 100
#define MAX_APPLIANCES 10000
#define MAX_MEASUREMENTS 1000
#define MAX_AUTO_TESTS 1000
#define ALERT_PROCENTAGE 0.95

#define MAX_DB_NAME_LEN		MAX_NAME_LENGHT
#define MAX_DISPLAY_NAME_LEN	MAX_NAME_LENGHT
#define MAX_KEYBOARD_DISPLAY_NAME_LEN	MAX_NAME_LENGHT
#define KEYBOARD_DISPLAY_MARGIN 6
#define KEYBOARD_DISPLAY_SMALL_MARGIN 2

#define MULTI_KEYBOARD_SIZE 15

#define WARNING_MESSAGE_SLEEP 1800
//Keyboard SETTINGS
#define KEY_LONG_PRESS_TIME 1080

#define MEASURE_TIME_OUT_TIMER	3600000

//SOUND SETTINGS
#define SOUND_OFF				0
#define SOUND_ON_25				8//16
#define SOUND_ON_50				25
#define SOUND_ON_75				40
#define SOUND_ON_100			64
#define KEYBOARD_BUTTON			1
#define BUTTONS_BUTTON			2
#define COMMANDER_BUTTON		3
#define VOLUME_BUTTON			4
#define FAIL_BEEP_BUTTON		5
#define RESULT_FAIL_BEEP_BUTTON 6
#define RESULTOFF_BUTTON		0
#define DURATION_1S_BUTTON		1000
#define DURATION_3S_BUTTON		3000

//UNIVERSAL WIN DEFINES
#define MAX_SETTING_LEN		32
#define MAX_F_NAME_LEN		32
#define MAX_EDIT_TEXT_LEN	80

#define R_BIG_FONT			54
#define R_SMALL_FONT		44
#define R_SMALLEST_FONT		34
#define LIMIT_OFFSET		1000
#define LIMIT_OFFSET_1		2000

#define TRIMMED_TEXT_LEN_19	19

#define RECALL_TEXT_BIG_FONT		31
#define RECALL_TEXT_SMALL_FONT		23
#define RECALL_TEXT_SMALLEST_FONT	15

#define EDIT_TEXT_BIG_FONT			26
#define EDIT_TEXT_SMALL_FONT		16
#define EDIT_TEXT_SMALLEST_FONT		10

#define NORMAL_EDIT_FONT	24

#define TIMER_START_DELAY	300

#define MAX_LISTBOX_ELEMENTS 5
enum LIST_BOX_MOVE
	{
		NO_MOVE,
		MOVE_UP,
		MOVE_DOWN
		};

enum F_G_LAYOUT
	{
		PRIM_F,
		ALT_1_,
		ALT_2_,
		ALT_3_,
		ALT_4_,
		ALT_5_,
		ALT_6_,
		ALT_7_,
		ALT_8_,
		ALT_9_,
		ALT_10_,
		ALT_11_,
		ALT_AUTO_TEST_EXECUTING_,
		NOT_VISIBLE,
		SPECIAL_NOT_USED,
		NO_SPECIAL,
		_UP_VISIBLE = 100,
		_DOWN_VISIBLE,
		_LEFT_VISIBLE,
		_RIGHT_VISIBLE,
		_ENTER_VISIBLE,
		_SEARCH_VISIBLE,
		_SPACE_VISIBLE,
		_NAVIGATION_VISIBLE,
		
	};

	enum BUTTON
	{
		F1,
		F2,
		F3,
		F4,
		F5,
		NONE_BUTT
	};

	enum AUTO_TEST_STEP_STATE
	{
		STEP_IS_TEMPLATE = -1,
		STEP_SINGLE_MEAS,
		STEP_NOT_STARTED,
		STEP_STARTED,
		STEP_SKIPPED,
		STEP_FINISHED,
		
	};
	enum AUTO_TEST_PROGRAM_STATE
	{
		AUTO_TEST_NONE,
		AUTO_TEST_CREATING,
		AUTO_TEST_PREPARING_EXECUTE,
		AUTO_TEST_EXECUTING,
		AUTO_TEST_EXECUTING_CHG_PARAMS,
		AUTO_TEST_FINISHED,
		AUTO_TEST_STARTED,
		
	};

	enum START_PRESSED_MODE
	{
		SHORT_PRESS,
		LONG_PRESS,
		DOUBLE_PRESS,
	};

		enum QWERTY_STATE
	{
		NUM_BOX,
		LETTERS,
		NUMBERS,
		MULTI_BUTTONS,
		ALL_HIDDEN,
	};

	enum QWERTY_MULTI_BUTTON_STATE
	{
		MULTI_NONE,
		MULTI_RPE_LIMIT_TOLERANCE,
		
	};

		enum AUTO_TEST_STATE
	{
		 AUTO_STATE_INIT,
		 AUTO_STATE1,
		 AUTO_STATE2,
		 AUTO_STATE3,
		 AUTO_STATE4,
		 AUTO_STATE5,
		 AUTO_STATE6,
		 AUTO_STATE_FAIL,
		 AUTO_STATE_PASS,
		 AUTO_STATE_CANCELED,
	};

	enum START_STATE
	{
		START_NOTSTARTED,
		START_FAST,
		START_SLOW,
		START_NORMAL,
		START_LONG,
		START_TIMER,
		START_OTHER,
		START_COMPENSATION,
		START_RECALL,
		START_RAMP,
		START_RAMP_NO_TRIP_TIME,
		START_RAMP_TRIPOUT_TIME,
		START_T_NO_TRIP_TIME,
		START_Z_REF,
		
	};

	enum DB_CLASS_MODE
	{
		MODE_LIVE,
		MODE_LIVE_SPECIAL,
		MODE_RECALL,
		MODE_LIVE_AUTO,
	};

	enum STANDARD
	{
		STD_MACHINERY,
		STD_ARC_WELDING,
		STD_SWITCHGEAR,
		STD_ALL,
		STD_NONE,
		STD_STEP = 100, // do not change this....
	};

	enum AVAILABILITY
	{
		AVAILABLE,
		NOT_SUPPORTED_YET,
		NOT_AVAILIBLE,
	};

	enum WIN_MODE
	{
		F_NOT_COLORED_SETTINGS_PAGE,
		F0_SETTINGS_PAGE,
		F1_SETTINGS_PAGE,
		F2_SETTINGS_PAGE,
		F3_SETTINGS_PAGE,
		F4_SETTINGS_PAGE,
		F5_SETTINGS_PAGE,
		F6_SETTINGS_PAGE,
		F7_SETTINGS_PAGE,
		F8_SETTINGS_PAGE,
		MEASUREMENT_PAGE,
		MEASUREMENT_PAGE_WITH_RESULT,
		RECALL_PAGE_1,
		RECALL_PAGE_2,
		RECALL_PAGE_3,
		RECALL_PAGE_4,
		MANAGE_LIMIT_PAGE,
		OUT_OF_SIGHT
	};


	enum DB_ENUM
	{
		SETTINGS_DB,
		MEAS_DB,
		AUTO_DB,
		APPLIANCE_DB,
	};


#define EMPTY_NAME_DOTS L"......"

#define DEFAULT_DELAY_COMMAND 50

//END UNIVERSAL WIN DEFINES

#define BETAVERSION

#ifdef BETAVERSION
	#define VERSION_STR L"01 BETA 13.6"
#endif

#ifndef BETAVERSION
  #define VERSION_STR L"P01"
#endif
#define HARDWARE_VERSION L"P02"

#ifdef VybridVFxx

#define IOCTL_DISK_BASE FILE_DEVICE_DISK
#define IOCTL_DISK_FLUSH_CACHE \
	CTL_CODE(IOCTL_DISK_BASE, 0x715, METHOD_BUFFERED, FILE_ANY_ACCESS)


#endif

#define BARCODE

#pragma comment(linker, "/nodefaultlib:libc.lib")
#pragma comment(linker, "/nodefaultlib:libcd.lib")

#pragma  warning(disable:4800)
#pragma  warning(disable:4996)
#pragma  warning(disable:4985)
#pragma  warning(disable:4482)
#pragma  warning(disable:4603)
#pragma  warning(disable:4005)
#pragma  warning(disable:4002)
#pragma  warning(disable:4100)
#pragma  warning(disable:4244)
// Windows Header Files:

#define BLINK_ON_TIME	500 //vasilij 850
#define BLINK_OFF_TIME	550

#define IMP_REAL_TIME

#include <windows.h>
#include <windowsx.h>

//#include "bluetooth.h"
//#include "Functions.h"

#include <bt_api.h>
#include <bthutil.h>
#include "zip.h"


#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <iomanip>



#ifdef VybridVFxx  


#include "gpio.h"   //for Vybrid
//#include "gpio_imx6.h" //for imx6  TODO 


#include "coproc.h"
#include "int.h"
#include "pwm.h"
#include "adc.h"


#elif defined(T20)

#include "GpioLibxxx.h"
#include "intlib.h"
#include "ADCLibxxx.h"
#include "CoprocLibxxx.h"

#elif defined(iMX6)



#else

#endif

// Xaml Runtime Header Files
#include <XamlRuntime.h>
#include <XRDelegate.h>
#include <XRPtr.h>
#include <XRCustomControl.h>
#include <PnP.h>
#include <storemgr.h>
#include <msgqueue.h>
#include <comutil.h>
#include <ctime>
#include "Sound.h"
#include "HookerDefines.h"


// IUnknown
extern "C" const GUID __declspec(selectany)IID_IUnknown = __uuidof(IUnknown);
struct Fuse;
// Resource type for XAML files
#define RT_XAML L"XAML"
extern int debug_level;
using namespace _com_util;

#include "DeviceInfo.h"
#include <sstream>
#include "Language.h"

#include "CREATE_TABLES.h"
// Application headers
#include "App.h"
#include "resource.h"

#include <crtdbg.h>
#include <tchar.h>
#include <math.h>

#include "decoder.h"
#include "LEDS.h"
#include "Setting.h"
#include "DataBaseAccess.h"

#include "SerialPort.h"

//#define SERIAL_NO_OVERLAPPED
#include "Msg.h"


#include "USBDrive.h"
#include "SerialWnd.h"
// TODO: reference additional headers your program requires here

#include "HardKeys.h"

#include "Drawing.h"
#include "Lang.h"
#include "MEASUREMENT_ENUMS.h"


struct Fuse
{
	int inominal;
	float value[4];
};
//#define OLD_PROTO

extern class HardKeys		*hk;
extern class CSerialWnd		*port1;
extern class CSerialWnd		*port2;
extern class DataBaseAccess *database;
extern class USBDrive		*drive1;
extern class USBDrive		*drive2;
extern class USBtoPC		*pcConnection;
extern class Operater		*operater;
//extern class CSerialWnd	*bluetooth;
extern class CSerialWnd *PrinterLPT1;
//extern class CSerialWnd *PrinterLPT2;
extern class Language *lang;
extern CRITICAL_SECTION CriticalSection;
extern Fuse FuseGGTabele[50];
extern Fuse FuseAmTabele[50];

#include "USBtoPC.h"
#include "Customer.h"
#include "MemoryErrors.h"

#include "Machine.h"


#include "Result.h"
#include "DataBaseIDs.h"
#include "DefaultValues.h"

#include "Converter.h"
#include "DBResult.h"

#include "Site.h"
#include "LimitNew.h"
#include "Description.h"
#include "ConnNum.h"
#include "LocationNew.h"
#include "SubPart.h"
#include "AUTOTest.h"

#include "TesterInfo.h"
#include <comutil.h>

#include "ResultBox.h"

#include "DBResult.h"
#include "DBLimit.h"
#include "DBValue.h"

#include "SaveMeasurement.h"


#include "SelectProbeMode.h"

//New database files
#include "DBVis_Insp.h"
#include "DB_Rpe.h"
#include "DBRCD.h"
#include "DBRiso.h"
#include "DBLoop.h"
#include "DBUdrop.h"
#include "DB_HV_Adapter.h"
#include "DB_Ures.h"
#include "DB_Ileak.h"
#include "DB_UP.h"




#include "BootScreen.h"
#include "MainPage.h"


#include "Limit.h"

#include "sqlite3.h"
#include "Customer.h"
#include "WindowManager.h"
#include "AppHooker.h"
#include <sipapi.h>
#include "Operater.h"
#include "UniversalWin.h"
#include "Universal.h"
#include "UniversalRPE.h"

#include "Universal_HK_Vis_Insp.h"
#include "Universal_HK_Loop.h"
#include "Universal_HK_Udrop.h"
#include "Universal_HK_RCD.h"
#include "Universal_HK_RPE.h"
#include "Universal_HK_RISO.h"
#include "Universal_HK_HV_Adapter.h"
#include "Universal_HK_Ures.h"
#include "Universal_HK_Ileak.h"
#include "Universal_HK_UP.h"
#include "Cursor.h"
#include "QWERTY.h"

#define SQRT3	1.73205080757



