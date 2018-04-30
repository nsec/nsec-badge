# Bluetooth Low Energy - Getting Started

## 1. Creating the BLE device

A BLE device has to be created and configured. The BLE stack requires the soft device, so it must be 
initialized before the BLE device is created. The ``create_ble_device`` function takes the name of the soft device in
 parameter.


```C
#include "ble/ble_device.h"
#include "ble/gap_configuration.h"
#include "softdevice.h"


int main(){
    softdevice_init();
    create_ble_device("My BLE device");
    configure_advertising();
}
```
## 2. Adding a vendor service

After the BLE device is created, a vendor service (fancy Bluetooth spec name for a custom service) can be added to the
 ble device. 


```C
int main(){
    softdevice_init();
    create_ble_device("My BLE device");
    configure_advertising();
    VendorService my_service;
    create_vendor_service(&my_service);
    add_vendor_service(&service1);
}
```

## 3. Adding characteristics to the vendor service

```C
    VendorService my_service;
    create_vendor_service(&my_service);
    add_vendor_service(&my_service);
    ServiceCharacteristic characteristic0;
    uint16_t length_of_characteristic_value = 1;
    add_characteristic_to_vendor_service(&my_service, &characteristic0, length_of_characteristic_value, AUTO_READ,
                                         AUTO_WRITE);
```

### 3.1 Setting and getting the value of a bluetooth characteristic

Change the size of the buffer to the size of the characteristic value.

```C
#define SIZE_OF_CHARACTERISTIC_VALUE 1
uint8_t value_buffer[SIZE_OF_CHARACTERISTIC_VALUE];
value_buffer[0] = 0xab;
set_characteristic_value(&characteristic0, value_buffer);

get_characteristic_value(&characteristic0, value_buffer);

```

### 3.2 Characteristic permissions

The last two parameters of ``add_characteristic_to_vendor_service`` are the read permission and the write permission 
for the characteristic.

Read permissions are:
* AUTO\_READ: The reading of a characteristic by a BLE client is entirely done by the soft device. No action is 
required by the user code. No handler is called.
* REQUEST\_READ: The reading of a characteristic by a BLE client invoke the ``on_read_request`` handler of the 
characteristic, which can allow or deny the read request.
* DENY\_READ: The reading of a characteristic by a BLE client will be denied. No action is required by the user code. 
No handler is called.

Write permissions are :
* AUTO\_WRITE: The writing of a characteristic by a BLE client is entirely done by the soft device. No action is 
required by the user code. The ``on_write_command`` handler of the characteristic is invoked to inform the user code 
of the write event.
* REQUEST\_WRITE: The writing of a characteristic by a BLE client invoke the ``on_write_request`` handler of the 
characteristic, which can allow or deny the write request.
* DENY\_WRITE: The writing of a characteristic by a BLE client will be denied. No action is required by the user code. 
No handler is called.

### 3.3 Characteristic event handlers

TODO


## 4. Starting the advertising of the BLE device.

A default BLE service has to be included in the advertising packets, so one service has to be set as the default 
service with ``set_default_advertised_service`` before the advertising can be started.


```C
    	 set_default_advertised_service(&my_service);
    	 start_advertising();
```
A bluetooth device named "My BLE device" should now be visible.
