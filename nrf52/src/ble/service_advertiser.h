//  Copyright (c) 2018
//  Nicolas Aubry <nicolas.aubry@hotmail.com>
//
//  License: MIT (see LICENSE for details)

#include <ble.h>
#include <stdbool.h>

struct Advertiser;
struct VendorService;

void init_adv_module(void);
void enable_fast_advertising_mode(uint32_t timeout);
void enable_slow_advertising_mode(uint32_t timeout);
void set_device_name(const char* name);
void set_vendor_service_in_advertising_packet(struct VendorService* service, bool more_available);
void set_vendor_service_in_scan_response(struct VendorService* service, bool more_available);
struct Advertiser *get_service_advertiser(void);
