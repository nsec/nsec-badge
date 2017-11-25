//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef USB_COMMON_H
#define USB_COMMON_H

#include <libopencm3/usb/usbd.h>

extern usbd_device *g_usbd_dev;

void usb_disconnect(void);

#endif
