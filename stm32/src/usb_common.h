#ifndef USB_COMMON_H
#define USB_COMMON_H

#include <libopencm3/usb/usbd.h>

extern usbd_device *g_usbd_dev;

void usb_disconnect(void);

#endif
