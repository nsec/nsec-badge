#include <libopencm3/usb/usbd.h>
#include <libopencm3/stm32/tools.h>
#include <libopencm3/stm32/st_usbfs.h>
#include <libopencm3/cm3/nvic.h>
#include <../lib/usb/usb_private.h>
#include "usb_common.h"

usbd_device *g_usbd_dev __attribute__((aligned));

void usb_isr(void) {
    usbd_poll(g_usbd_dev);
}

void usb_disconnect(void) {
    CLR_REG_BIT(USB_BCDR_REG, USB_BCDR_DPPU);

    _usbd_reset(g_usbd_dev);

    // Give some time to the host to discover that the USB device is gone.
    for (uint32_t i = 0; i < 0x200000; i++) {
        __asm__("nop");
    }
}
