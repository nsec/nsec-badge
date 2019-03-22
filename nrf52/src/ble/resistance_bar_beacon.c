//  Copyright (c) 2019
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)


#include "beacon.h"

static const char resistance_propaganda[] = "J01N TH3 R3S1ST4NC3";
static const char beacon_name[] = "B4R";

void init_resistance_bar_beacon(){
    set_beacon_name(beacon_name);
    set_broadcast_data((uint8_t*)resistance_propaganda, (uint16_t)strlen(resistance_propaganda));
    set_advertising_interval_ms(500);
}

struct Advertiser* get_resistance_bar_beacon(){
    return get_beacon();
}
