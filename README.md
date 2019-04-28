Northsec 2019 Badge
===================

Brought to you by the Team badge for NorthSec.
----------------------------------------------

[Badge website](http://sputnak.ga/)

## Hardware overview

The NorthSec 2019 badge has two programmable micro-controllers:

 - The [Nordic Semiconductor](https://www.nordicsemi.com) [`nRF52832`](https://www.nordicsemi.com/eng/Products/Bluetooth-low-energy2/nRF52832) (nRF52) and
 - The [STMicroelectronics](http://www.st.com/) [`STM32F070F6P6`](http://www.st.com/content/st_com/en/products/microcontrollers/stm32-32-bit-arm-cortex-mcus/stm32-mainstream-mcus/stm32f0-series/stm32f0x0-value-line/stm32f070f6.html) (stm32).

The nRF52 is based on the ARM Cortex-M4F core and runs the `armv7-m` instruction set, it takes care of:

 - The OLED display
 - The Bluetooth Low Energy
 - The battery management
 - The [NeoPixel](https://en.wikipedia.org/wiki/Adafruit_Industries#NeoPixel) RGB LEDs
 - The buttons

The stm32 is based on the ARM Cortex-M0 core and runs the `armv6-m` instruction set, it takes care of:

 - The USB port

The badge can be supplied by 1 ICR14500 3.7v Li-ion rechargeable battery and/or the USB port. The
battery can be charged from the USB port but the power switch must be turned ON for the duration
of the charge.

The [schematic](http://xn--rr8b.ga/northsec_2019_schematics.pdf) are available.

If you wish to build your own badge:

* [pcb]()
* [bill of material]()

## Compiling
You can either compile the code using CQFD and Docker or you can install the toolchain and build the project
by yourself. Using CQFD and Docker will make sure you are using the right compiler version and toolchain.

### CQFD ( Preferred method )

Follow the steps to install CQFD and Docker, then simply run:

```
cqfd init && cqfd
```

[CQFD](https://github.com/savoirfairelinux/cqfd)

### Getting the toolchain

There is a `Makefile` for each micro-controller, but here are some stuff you'll
need to make it work.

To compile the badge firmware from source, your gcc need to be able
generating `armv6-m` code and link with the nano variant of
[newlib](https://sourceware.org/newlib/).

On OS X or macOS, you can use my formula for Homebrew:

```
brew tap marc-etienne/stm32
brew install arm-none-eabi-gcc
```

On Arch Linux, you will need the package
[arm-none-eabi-gcc](https://www.archlinux.org/packages/community/x86_64/arm-none-eabi-gcc/),
[arm-none-eabi-newlib](https://www.archlinux.org/packages/community/any/arm-none-eabi-newlib/)
and [arm-none-eabi-gdb](https://www.archlinux.org/packages/community/x86_64/arm-none-eabi-gdb/).

On Debian Jessie, install `gcc-arm-none-eabi` and `libnewlib-arm-none-eabi`.

On Ubuntu, check the the [GNU ARM Embedded Toolchain PPA](https://launchpad.net/~team-gcc-arm-embedded/+archive/ubuntu/ppa).

Pre-built packages are also available for Windows, Linux and OS X here:
https://launchpad.net/gcc-arm-embedded.

You will also need some python libraries, on Ubuntu / Debian:

```
sudo apt install python3-pil python3-pycparser
```

### Dependencies

The nF52 firmware depends on the Nordic SDK v14.2.0. You can download and extract
the SDK by typing `make nordicsdk` in the `nrf52` directory. I will also be done
automatically when building the first time.

The nRF52 also uses the s132 "SoftDevice" v5.0.0 to painlessly enable Bluetooth
Low Energy (BLE), which comes with the SDK. Use
`make builds/s132_nrf52_5.0.0_softdevice.elf` to create the ELF file
from Nordic's binary distribution.

NOTE: The Nordic SDK and the SoftDevice isn't free software. The licence
agreements (`nordicsdk_licence_agreement.txt` and
`s132_nrf52_5.0.0_licence_agreement.txt`) will be downloaded as well.

The stm32 requires the [opencm3](http://libopencm3.org/) library. The library
is setup as a git submodule. Typing `git submodule init` and `git submodule
update` in the stm32 directory will fetch the latest version of the library.

## Programming

### Via Serial Wire Debug (SWD)

Each micro-controller can be programmed using 6-pin Tag-Connect cable, the
[TC2030-CTX-NL](http://www.tag-connect.com/TC2030-CTX-NL), and a probe to talk
to the chip via SWD. We used the
[Blacksphere](https://web.archive.org/web/20160331030547/http://www.blacksphere.co.nz/main/index.php)
[Blackmagic](https://web.archive.org/web/20160331030217/http://www.blacksphere.co.nz/main/index.php/blackmagic)
probe. There is a [version 2](http://1bitsquared.com/collections/frontpage/products/black-magic-probe)
out there now.

The following `gdb` commands will load the firmware into the nRF52:

```
set gnutarget elf32-littlearm
define hook-quit
    set confirm off
end

target extended-remote /dev/sttyXXX # Replace with your Black magic serial port
monitor tpwr enable
monitor swdp_scan
attach 1
set mem inaccessible-by-default off
set debug arm
load builds/s132_nrf52_5.0.0_softdevice.hex
load builds/nsec19_nrf52_conf.elf
quit
```

The stm32 can be flashed in a similar fashion, although you will want to
`load builds/nsec19_stm32_debugger.elf` instead.

You can also use the (more expensive) STLink, but we do not have the hardware
to test instructions for it.

### Via USB

The stm32 can also be flashed by holding the `PROGRAM` button behind the badge
while pressing the `RESET` button. The stm32 will boot a specific on-chip bootloader
that implements the [DFU](https://en.wikipedia.org/wiki/USB#Device_Firmware_Upgrade)
interface.

Reset the board with the `PROGRAM` button pressed, then release the button.

You should now see a DFU device appearing on your computer:
    % lsusb
    ...
    Bus 001 Device 057: ID 0483:df11 STMicroelectronics STM Device in DFU Mode
    ...

Use a DFU compliant software to flash the STM32:
- [dfu-util](https://www.archlinux.org/packages/community/x86_64/dfu-util/) on ArchLinux

To make a `bin` file from an ELF, run the following:

    % arm-none-eabi-objcopy -O binary builds/nsec19_stm32_debugger.elf builds/nsec19_stm32_debugger.bin

Run the following command:

    % dfu-util --device 0483:df11 --alt 0 --dfuse-address 0x08000000:leave --download builds/nsec19_stm32_debugger.bin

The STM32 should reset automagically, running the newly downloaded firmware. The
address of 0x08000000 is important, this is where the stm32 flash is mapped into
memory.

## Firmware of NorthSec 2019

There were 6 firmware images built for the NorthSec 2019 event.

### `nsec19_stm32_debugger.elf`

The firmware of the stm32 used during the NorthSec conference. It has the
BlackMagic gdb stub exposed via the USB to reprogram and debug the nRF52 chip.

### `nsec19_stm32_crossdebug.elf`

Same as the stm32 conference firmware, except the debugger uses the external
pins to allow programming and debugging the stm32 micro-controller of another
badge.

### `nsec19_nrf52_{bar_beacon,conf,ctf,soldering}.elf`

The firmware of the nRF52 used during the NorthSec conference or CTF.

## Cookbook

Here are the steps to get you started. Lets say you've downloaded the source into the
`nsec-badge` folder.

### stm32

#### BlackMagic

The blackmagic firmware that runs on the stm32 let you debug and flash firmware
on the nRF52.

To compile the blackmagic firmware for the stm32:

    % git submodule init
    % git submodule update
    % make builds/nsec19_stm32_debugger.bin

#### Flashing the firmware

To flash the firmware on the stm32, boot the uController into DFU mode by
pressing and holding the `PROGRAM` button, press `RESET`, then releasing the
`PROGRAM` button.

Make sure you see a DFU device:

    % lsusb
    Bus 001 Device 057: ID 0483:df11 STMicroelectronics STM Device in DFU Mode

Use [dfu-util](http://dfu-util.sourceforge.net/) to flash the firmware:

    % dfu-util --reset --device 0483:df11 --alt 0 --dfuse-address 0x08000000 --download builds/nsec19_stm32_debugger.bin

### nRF52

To compile the binary for the nRF52:

    % cd nrf52
    % make

To flash the binary on the nRF52, you'll need to either:

* have the blackmagic debugger firmware flashed onto the stm32
* use a BlackMagic device

Use the blackmagic exposed ACM device to flash the firmware using GDB.
