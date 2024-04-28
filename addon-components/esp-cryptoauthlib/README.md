# ESP-CRYPTOAUTHLIB

This is a port of Microchip's [cryptoauthlib](https://github.com/MicrochipTech/cryptoauthlib) for ESP-IDF. It contains necessary build support to use cryptoauthlib with ESP-IDF as well as `esp_cryptoauthlib_utility` for configuring and provisiong ATECC608A chip connected to an ESP module. Currently the utility is supported for ESP32, ESP32S3 and ESP32C3. The cryptoauthlib folder which is a subset of Microchip's [cryptoauthlib](https://github.com/MicrochipTech/cryptoauthlib) is created with help of script [generate_component.sh](https://github.com/espressif/esp-cryptoauthlib/blob/master/generate_component.sh).

## Requirements

* [ESP-IDF](https://github.com/espressif/esp-idf) version should be `release/v4.3` or newer.
* Environment variable `IDF_PATH` should be set

## How to use esp-cryptoauthlib with ESP-IDF
---
There are two ways to use `esp-cryptoauthlib` in your project

1) Directly add `esp-cryptoauthlib` as a component in your project with following three commands.

    (First change directory (cd) to your project directory)
```
    mkdir components
    cd components
    git clone https://github.com/espressif/esp-cryptoauthlib.git
```
2) Add `esp-cryptoauthlib` as an extra component in your project.

* Download `esp-cryptoauthlib` with:
```
    git clone https://github.com/espressif/esp-cryptoauthlib.git
```
* Include  `esp-cryptoauthlib` in `ESP-IDF` with setting `EXTRA_COMPONENT_DIRS` in CMakeLists.txt/Makefile of your project.For reference see [Optional Project Variables](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html#optional-project-variables)

## How to configure and provision ATECC608
The python utilty `esp_cryptoauth_utility` helps to configure, generate resources as well as provision ATECC608A chip connected to an ESP module.
For detailed instructions on how to use the utility please refer utility [README.md](https://github.com/espressif/esp-cryptoauthlib/blob/master/esp_cryptoauth_utility/README.md)
