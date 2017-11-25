// Copyright (c) 2017
// Benjamin Vanheuverzwijn <bvanheu@gmail.com>
// Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
// License: MIT (see LICENSE for details)

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>

#include "circbuf.h"
#include "delay.h"
#include "usb_common.h"

#include "usb_cdc_acm.h"

static volatile on_data_in_cb_t g_on_data_in_cb;
static uint8_t buffer[256] = {0};
static struct circbuf circbuf;

static void usbcdcacm_set_modem_state(usbd_device *dev,
        int iface,
        bool dsr,
        bool dcd) {
    char buf[10];
    struct usb_cdc_notification *notif = (void*)buf;
    /* We echo signals back to host as notification */
    notif->bmRequestType = 0xA1;
    notif->bNotification = USB_CDC_NOTIFY_SERIAL_STATE;
    notif->wValue = 0;
    notif->wIndex = iface;
    notif->wLength = 2;
    buf[8] = (dsr ? 2 : 0) | (dcd ? 1 : 0);
    buf[9] = 0;
    usbd_ep_write_packet(dev, 0x82 + iface, buf, 10);
}

static const struct usb_device_descriptor dev_desc = {
    .bLength = USB_DT_DEVICE_SIZE,
    .bDescriptorType = USB_DT_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = USB_CLASS_CDC,
    .bDeviceSubClass = 0,
    .bDeviceProtocol = USB_CDC_PROTOCOL_NONE,
    .bMaxPacketSize0 = 64,
    .idVendor = 0x1453,
    .idProduct = 0x4026,
    .bcdDevice = 0x0200,
    .iManufacturer = 1,
    .iProduct = 2,
    .iSerialNumber = 3,
    .bNumConfigurations = 1,
};

/*
 * This notification endpoint isn't implemented. According to CDC spec its
 * optional, but its absence causes a NULL pointer dereference in Linux
 * cdc_acm driver.
 */
static const struct usb_endpoint_descriptor endp_desc_comm[] = {{
    .bLength = USB_DT_ENDPOINT_SIZE,
        .bDescriptorType = USB_DT_ENDPOINT,
        .bEndpointAddress = 0x83,
        .bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT,
        .wMaxPacketSize = 16,
        .bInterval = 255,
}};

static const struct usb_endpoint_descriptor endp_desc_data[] = {{
    .bLength = USB_DT_ENDPOINT_SIZE,
        .bDescriptorType = USB_DT_ENDPOINT,
        .bEndpointAddress = 0x01,
        .bmAttributes = USB_ENDPOINT_ATTR_BULK,
        .wMaxPacketSize = 64,
        .bInterval = 1,
}, {
    .bLength = USB_DT_ENDPOINT_SIZE,
        .bDescriptorType = USB_DT_ENDPOINT,
        .bEndpointAddress = 0x82,
        .bmAttributes = USB_ENDPOINT_ATTR_BULK,
        .wMaxPacketSize = 64,
        .bInterval = 1,
}};

static const struct {
    struct usb_cdc_header_descriptor header;
    struct usb_cdc_call_management_descriptor call_mgmt;
    struct usb_cdc_acm_descriptor acm;
    struct usb_cdc_union_descriptor cdc_union;
} __attribute__((packed)) cdcacm_functional_descriptors = {
    .header = {
        .bFunctionLength = sizeof(struct usb_cdc_header_descriptor),
        .bDescriptorType = CS_INTERFACE,
        .bDescriptorSubtype = USB_CDC_TYPE_HEADER,
        .bcdCDC = 0x0110,
    },
    .call_mgmt = {
        .bFunctionLength = sizeof(struct usb_cdc_call_management_descriptor),
        .bDescriptorType = CS_INTERFACE,
        .bDescriptorSubtype = USB_CDC_TYPE_CALL_MANAGEMENT,
        .bmCapabilities = 0,
        .bDataInterface = 1,
    },
    .acm = {
        .bFunctionLength = sizeof(struct usb_cdc_acm_descriptor),
        .bDescriptorType = CS_INTERFACE,
        .bDescriptorSubtype = USB_CDC_TYPE_ACM,
        .bmCapabilities = 0,
    },
    .cdc_union = {
        .bFunctionLength = sizeof(struct usb_cdc_union_descriptor),
        .bDescriptorType = CS_INTERFACE,
        .bDescriptorSubtype = USB_CDC_TYPE_UNION,
        .bControlInterface = 0,
        .bSubordinateInterface0 = 1,
    },
};

static const struct usb_interface_descriptor iface_desc_comm[] = {{
    .bLength = USB_DT_INTERFACE_SIZE,
        .bDescriptorType = USB_DT_INTERFACE,
        .bInterfaceNumber = 0,
        .bAlternateSetting = 0,
        .bNumEndpoints = 1,
        .bInterfaceClass = USB_CLASS_CDC,
        .bInterfaceSubClass = USB_CDC_SUBCLASS_ACM,
        .bInterfaceProtocol = USB_CDC_PROTOCOL_AT,
        .iInterface = 0,
        .endpoint = endp_desc_comm,
        .extra = &cdcacm_functional_descriptors,
        .extralen = sizeof(cdcacm_functional_descriptors),
}};

static const struct usb_interface_descriptor iface_desc_data[] = {{
    .bLength = USB_DT_INTERFACE_SIZE,
        .bDescriptorType = USB_DT_INTERFACE,
        .bInterfaceNumber = 1,
        .bAlternateSetting = 0,
        .bNumEndpoints = 2,
        .bInterfaceClass = USB_CLASS_DATA,
        .bInterfaceSubClass = 0,
        .bInterfaceProtocol = 0,
        .iInterface = 0,
        .endpoint = endp_desc_data,
}};

static const struct usb_interface ifaces[] = {{
    .num_altsetting = 1,
        .altsetting = iface_desc_comm,
}, {
    .num_altsetting = 1,
        .altsetting = iface_desc_data,
}};

static const struct usb_config_descriptor config_desc = {
    .bLength = USB_DT_CONFIGURATION_SIZE,
    .bDescriptorType = USB_DT_CONFIGURATION,
    .wTotalLength = 0,
    .bNumInterfaces = 2,
    .bConfigurationValue = 1,
    .iConfiguration = 0,
    .bmAttributes = 0x80,
    .bMaxPower = 0x32,
    .interface = ifaces,
};

static const char *usb_strings[] = {
    "NorthSec Technologies",
    "NorthSec 2017 CTF",
    "Badge",
};

/* Buffer to be used for control requests. */
static uint8_t usbd_control_buffer[1024] __attribute__((aligned(4)));

static int usbcdcacm_control_request(usbd_device *usbd_dev,
        struct usb_setup_data *req,
        uint8_t **buf,
        uint16_t *len,
        void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req)) {
    (void)complete;
    (void)buf;
    (void)usbd_dev;

    switch (req->bRequest) {
        case USB_CDC_REQ_SET_CONTROL_LINE_STATE:
            usbcdcacm_set_modem_state(usbd_dev, req->wIndex, true, true);
        case USB_CDC_REQ_SET_LINE_CODING:
            if (*len < sizeof(struct usb_cdc_line_coding)) {
                return 0;
            }
            return 1;
    }

    return 0;
}

static void usbcdcacm_data_rx_cb(usbd_device *usbd_dev, uint8_t ep) {
    (void)ep;
    (void)usbd_dev;

    uint8_t buf[64] = {0};

    int len = usbd_ep_read_packet(usbd_dev, 0x01, buf, 64);

    if (!g_on_data_in_cb && len) {
        for (int i=0; i<len; i++) {
            circbuf_put(&circbuf, buf[i]);
        }
    }
    else if (g_on_data_in_cb && len) {
        g_on_data_in_cb(buf, len);
    }
}

static void usbcdcacm_set_config(usbd_device *usbd_dev, uint16_t wValue) {
    (void)wValue;

    usbd_ep_setup(usbd_dev, 0x01, USB_ENDPOINT_ATTR_BULK, 64, usbcdcacm_data_rx_cb);
    usbd_ep_setup(usbd_dev, 0x82, USB_ENDPOINT_ATTR_BULK, 64, NULL);
    usbd_ep_setup(usbd_dev, 0x83, USB_ENDPOINT_ATTR_INTERRUPT, 16, NULL);

    usbd_register_control_callback(
            usbd_dev,
            USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
            USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
            usbcdcacm_control_request);

    usbcdcacm_set_modem_state(usbd_dev, 0, true, true);
}

void usbcdcacm_set_on_data_in_cb(on_data_in_cb_t on_data_in_cb) {
    g_on_data_in_cb = on_data_in_cb;
}

void usbcdcacm_send_data(uint8_t *data, uint16_t size) {
    while (usbd_ep_write_packet(g_usbd_dev, 0x82, data, size) != size);
}

int usbcdcacm_init(void) {
    rcc_set_usbclk_source(RCC_HSI48);

    circbuf_init(&circbuf, buffer, 256);

    g_usbd_dev = usbd_init(&st_usbfs_v2_usb_driver,
            &dev_desc,
            &config_desc,
            usb_strings,
            3,
            usbd_control_buffer,
            sizeof(usbd_control_buffer));

    usbd_register_set_config_callback(g_usbd_dev, usbcdcacm_set_config);

    nvic_enable_irq(NVIC_USB_IRQ);

    delay(0x800000);

    return 0;
}

int _read(int file, char *ptr, int len);
int _read(int file, char *ptr, int len) {
    (void) file;
    (void) len;

    while (!circbuf_get(&circbuf, (uint8_t *)ptr));

    if (*ptr == ' ') {
        *ptr = '\n';
    }

    return 1;
}

int _write(int file, char *ptr, int len);
int _write(int file, char *ptr, int len) {
    (void) file;

    int i;
    if (file == STDOUT_FILENO || file == STDERR_FILENO) {
        for (i = 0; i < len; i++) {
            if (ptr[i] == '\n') {
                usbcdcacm_send_data((uint8_t *)" ", 1);
            }
            usbcdcacm_send_data((uint8_t *)&(ptr[i]), 1);
        }
        return i;
    }
    return -1;
}

