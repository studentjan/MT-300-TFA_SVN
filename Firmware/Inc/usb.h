#ifndef USB_H_
#define USB_H_

#include "stm32f3xx_hal.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

int _read(int fd, char *instring, uint32_t count);
int _write(int,char*,int);
int kbhit(void);
uint8_t USB_Receive_FS (uint8_t* Buf, uint32_t Len);


#endif /* USB_H_ */
