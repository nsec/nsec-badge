//
// Copyright (c) 2017
// Benjamin Vanheuverzwijn <bvanheu@gmail.com>
// Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
// License: MIT (see LICENSE for details)
//

#include <stdint.h>
#include <stdlib.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/hid.h>
#include <libopencm3/usb/dfu.h>

#include "usb_common.h"

#include "usb_hid_kbd.h"

// Doc
//  http://www.usb.org/developers/hidpage/HID1_11.pdf

#define USB_HID_INTERFACE_SUBCLASS_BOOT     1
#define USB_HID_INTERFACE_PROTOCOL_KEYBOARD 1
#define USB_HID_INTERFACE_PROTOCOL_MOUSE    2

#define USB_HID_KBD_VENDOR      0x0483
#define USB_HID_KBD_PRODUCT     0x5710
#define USB_HID_KBD_DEVICE      0x0200

uint8_t usbd_control_buffer[128] __attribute__((aligned(4)));

const struct usb_device_descriptor dev_desc = {
    .bLength = USB_DT_DEVICE_SIZE,
    .bDescriptorType = USB_DT_DEVICE,
    .bcdUSB = 0x0200,
    // Next fields are not used, they are defined in the interface instead
    .bDeviceClass = 0,
    .bDeviceSubClass = 0,
    .bDeviceProtocol = 0,
    .bMaxPacketSize0 = 64,
    .idVendor = USB_HID_KBD_VENDOR,
    .idProduct = USB_HID_KBD_PRODUCT,
    .bcdDevice = USB_HID_KBD_DEVICE,
    // Indexes from string
    .iManufacturer = 1,
    .iProduct = 2,
    .iSerialNumber = 3,
    .bNumConfigurations = 1,
};

static uint8_t hid_report_descriptor[] = {
        0x05, 0x01, // Usage Page (Generic Desktop)
        0x09, 0x06, // Usage (Keyboard)
        0xA1, 0x01, // Collection (Application)
        0x05, 0x07, //  Usage Page (Keyboard)
        0x19, 0xE0, //  Usage Minimum (224)
        0x29, 0xE7, //  Usage Maximum (231)
        0x15, 0x00, //  Logical Minimum (0)
        0x25, 0x01, //  Logical Maximum (1)
        0x75, 0x01, //  Report Size (1)
        0x95, 0x08, //  Report Count (8)
        0x81, 0x02, //  Input (Data, Variable, Absolute)
        0x81, 0x01, //  Input (Constant)
        0x19, 0x00, //  Usage Minimum (0)
        0x29, 101,  //  Usage Maximum (101)
        0x15, 0x00, //  Logical Minimum (0)
        0x25, 101,  //  Logical Maximum (101)
        0x75, 0x08, //  Report Size (8)
        0x95, 0x06, //  Report Count (6)
        0x81, 0x00, //  Input (Data, Array)
        0x05, 0x08, //  Usage Page (LED)
        0x19, 0x01, //  Usage Minimum (1)
        0x29, 0x05, //  Usage Maximum (5)
        0x15, 0x00, //  Logical Minimum (0)
        0x25, 0x01, //  Logical Maximum (1)
        0x75, 0x01, //  Report Size (1)
        0x95, 0x05, //  Report Count (5)
        0x91, 0x02, //  Output (Data, Variable, Absolute)
        0x95, 0x03, //  Report Count (3)
        0x91, 0x01, //  Output (Constant)
        0xC0        // End Collection
};

static const uint8_t USBD_HID_Desc[] = {
/* 18 */
  0x09, // bLength: HID Descriptor size
  0x21, // bDescriptorType: HID
  0x11, // bcdHID: HID Class Spec release number
  0x01,
  0x00, // bCountryCode: Hardware target country
  0x01, // bNumDescriptors: Number of HID class descriptors to follow
  0x22, //bDescriptorType
  65,   // wItemLength: Total length of Report descriptor
  0x00,
};

static const struct {
    struct usb_hid_descriptor hid_descriptor;
    struct {
        uint8_t bReportDescriptorType;
        uint16_t wDescriptorLength;
    } __attribute__((packed)) hid_report;
} __attribute__((packed)) hid_function = {
    .hid_descriptor = {
        .bLength = sizeof(hid_function),
        .bDescriptorType = USB_DT_HID,
        .bcdHID = 0x0100,
        .bCountryCode = 0,
        .bNumDescriptors = 1,
    },
    .hid_report = {
        .bReportDescriptorType = USB_DT_REPORT,
        .wDescriptorLength = sizeof(hid_report_descriptor),
    }
};

const struct usb_endpoint_descriptor endp_desc = {
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType = USB_DT_ENDPOINT,
    .bEndpointAddress = 0x81,
    .bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT,
    .wMaxPacketSize = 64,
    .bInterval = 0x20,
};

const struct usb_interface_descriptor iface_desc = {
    .bLength = USB_DT_INTERFACE_SIZE,
    .bDescriptorType = USB_DT_INTERFACE,
    .bInterfaceNumber = 0,
    .bAlternateSetting = 0,
    .bNumEndpoints = 1,
    .bInterfaceClass = USB_CLASS_HID,
    .bInterfaceSubClass = USB_HID_INTERFACE_SUBCLASS_BOOT,
    .bInterfaceProtocol = USB_HID_INTERFACE_PROTOCOL_KEYBOARD,
    .iInterface = 0,
    .endpoint = &endp_desc,
    .extra = &hid_function,
    .extralen = sizeof(hid_function),
};

static int usbhidkbd_control_request(usbd_device *dev,
        struct usb_setup_data *req,
        uint8_t **buf,
        uint16_t *len,
        void (**complete)(usbd_device *dev, struct usb_setup_data *req)) {
    (void)complete;
    (void)dev;

    // wValue -> [DescriptorType & 0xff00 + DescriptorIndex & 0x00ff]
    if ((req->bmRequestType != 0x81) ||
        (req->bRequest != USB_REQ_GET_DESCRIPTOR) ||
        ((req->wValue != 0x2200) &&
        (req->wValue != 0x2100))) {
        return 0;
    }

    if (req->wValue == 0x2100) {
        *buf = (uint8_t *)USBD_HID_Desc;
        *len = sizeof(USBD_HID_Desc);
    }
    else if (req->wValue == 0x2200) {
        *buf = (uint8_t *)hid_report_descriptor;
        *len = sizeof(hid_report_descriptor);
    }

    return 1;
}

const struct usb_interface iface = {
    .num_altsetting = 1,
    .altsetting = &iface_desc,
};

const struct usb_config_descriptor config_desc = {
    .bLength = USB_DT_CONFIGURATION_SIZE,
    .bDescriptorType = USB_DT_CONFIGURATION,
    .wTotalLength = 0,
    .bNumInterfaces = 1,
    .bConfigurationValue = 1,
    .iConfiguration = 0,
    .bmAttributes = 0xC0,
    .bMaxPower = 0x32,
    .interface = &iface,
};

static void usbhidkbd_set_config(usbd_device *dev, uint16_t wValue) {
    (void)wValue;

    usbd_ep_setup(dev, 0x81, USB_ENDPOINT_ATTR_INTERRUPT, 4, NULL);

    usbd_register_control_callback(
            dev,
            USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE,
            USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
            usbhidkbd_control_request);
}

// Packet:
// Byte 0: Keyboard modifier bits (SHIFT, ALT, CTRL etc)
// Byte 1: reserved
// Byte 2-7: Up to six keyboard usage indexes representing the keys that are
//           currently "pressed".
//           Order is not important, a key is either pressed (present in the
//           buffer) or not pressed.
void usbhidkbd_send_key(uint8_t key) {
    uint8_t packet[8] __attribute__((aligned(4))) = {0};

    // Key down
    packet[2] = key;
    while (usbd_ep_write_packet(g_usbd_dev, 0x81, packet, sizeof(packet)) == 0);

    // Key up
    packet[2] = 0x00;
    while (usbd_ep_write_packet(g_usbd_dev, 0x81, packet, sizeof(packet)) == 0);
}

static const char *usb_strings[] = {
    "NorthSec Technologies",
    "HID",
    "Keyboard",
};

void usbhidkbd_init(void) {
    rcc_set_usbclk_source(RCC_HSI48);

    g_usbd_dev = usbd_init(&st_usbfs_v2_usb_driver,
        &dev_desc,
        &config_desc,
        usb_strings,
        3,
        usbd_control_buffer,
        sizeof(usbd_control_buffer));

    usbd_register_set_config_callback(g_usbd_dev, usbhidkbd_set_config);

    nvic_enable_irq(NVIC_USB_IRQ);

    return;
}

