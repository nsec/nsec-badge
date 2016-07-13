//
//  status_bar.h
//  nsec16
//
//  Created by Marc-Etienne M.Léveillé on 2016-05-14.
//
//

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

void nsec_status_bar_init();
void nsec_status_set_name(char * name);
void nsec_status_set_badge_class(char * class);
void nsec_status_set_ble_status(status_bluetooth_status status);
void nsec_status_set_battery_status(status_battery_state state);
void nsec_status_bar_ui_redraw();

#endif /* status_bar_h */
