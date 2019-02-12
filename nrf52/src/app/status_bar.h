//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#ifndef status_bar_h
#define status_bar_h

typedef enum {
    STATUS_BATTERY_CHARGING,
    STATUS_BATTERY_0_PERCENT,
    STATUS_BATTERY_25_PERCENT,
    STATUS_BATTERY_50_PERCENT,
    STATUS_BATTERY_75_PERCENT,
    STATUS_BATTERY_100_PERCENT,
} status_battery_state;

typedef enum {
    STATUS_BLUETOOTH_OFF,
    STATUS_BLUETOOTH_ON,
} status_bluetooth_status;

void nsec_status_bar_init(void);
void nsec_status_set_name(char * name);
void nsec_status_set_ble_status(status_bluetooth_status status);
void nsec_status_set_battery_status(status_battery_state state);
void nsec_status_bar_ui_redraw(void);

#endif /* status_bar_h */
