#ifndef USB_CDC_ACM_H
#define USB_CDC_ACM_H

typedef void (*on_data_in_cb_t)(uint8_t *data, uint16_t size);

int usbcdcacm_init(void);
int usbcdcacm_deinit(void);
void usbcdcacm_send_data(uint8_t *data, uint16_t size);
void usbcdcacm_set_on_data_in_cb(on_data_in_cb_t on_data_in);

#endif
