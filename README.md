Northsec 2017 Badge
===================

## Hardware overview

The Northsec 2017 badge has two programmable micro-controllers:

 - The [Nordic Semiconductor](https://www.nordicsemi.com) [`nRF51822`](https://www.nordicsemi.com/eng/Products/Bluetooth-low-energy2/nRF51822) (nRF51) and
 - The [STMicroelectronics](http://www.st.com/) [`stm32f072cb`](http://www.st.com/content/st_com/en/products/microcontrollers/stm32-32-bit-arm-cortex-mcus/stm32f0-series/stm32f0x2/stm32f072cb.html) (stm32).

Both are based on the ARM Cortex-M0 core and runs the `armv6-m` instruction set.

The nRF51 takes care of:

 - The OLED display
 - The Bluetooth Low Energy
 - The battery management
 - The red and green LEDs

The stm32 takes care of:

 - The touch buttons
 - The USB port

The badge can be supplied by 2 AAA batteries and/or the USB port. It is
perfectly safe to have both at the same time. The badge should work with
any DC voltage between 1.8V and 5.5V.

The [schematic](http://xn--rr8b.ga/northsec_2017_schematics.pdf) are available.

If you wish to build your own badge:

* [pcb]()
* [bill of material]()

## Compiling

There is a `Makefile` for each micro-controller, but here are some stuff you'll
need to make it work.

### Getting the toolchain

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

### Dependencies

The nF51 firmware depends on the Nordic SDK v12.1.0. You can download and extract
the SDK by typing `make nordicsdk` in the nrf51 directory.

The nRF51 also uses the s130 "SoftDevice" v2.0.1 to painlessly enable Bluetooth
Low Energy (BLE), which comes with the SDK. Use
`make _build/s130_nrf51822_2.0.1_softdevice.elf` to create the ELF file
from Nordic's binary distribution. Also note: the SoftDevice isn't free
software. The licence agreement (`s110_nrf51822_7.0.0_licence_agreement.pdf`)
will be downloaded as well.

The stm32 requires the [opencm3](http://libopencm3.org/) library. The library is setup as a git submodule.
Typing `git submodule init` and `git submodule update` in the stm32 directory
will fetch the latest version of the library.

## Programming

### Via Serial Wire Debug (SWD)

Each micro-controller can be programmed using 6-pin Tag-Connect cable, the
[TC2030-CTX-NL](http://www.tag-connect.com/TC2030-CTX-NL), and a probe to talk
to the chip via SWD. We used the
[Blacksphere](https://web.archive.org/web/20160331030547/http://www.blacksphere.co.nz/main/index.php)
[Blackmagic](https://web.archive.org/web/20160331030217/http://www.blacksphere.co.nz/main/index.php/blackmagic)
probe. There is a [version 2](http://1bitsquared.com/collections/frontpage/products/black-magic-probe)
out there now.

The following `gdb` commands will load the firmware into the nRF51:

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
load _build/s110_nrf51822_7.0.0_softdevice.elf
load _build/nsec16_badge_s110.elf
quit
```

The stm32 can be flashed in a similar fashion, although you will want to
`load stm32f072cb.elf` instead.

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
    % arm-none-eabi-objcopy -O binary input.elf output.bin

Run the following command:
    % dfu-util --reset --device 0483:df11 --alt 0 --dfuse-address 0x08000000 --download nsec17_stm32.bin

The STM32 should reset automagically, running the newly downloaded firmware. The
addres of 0x08000000 is important, this is where the stm32 flash is mapped into
memory.

## Firmware of NorthSec 2017

There were 7 firmware images built for the NorthSec 2017 event.

- `nsec17_conf_stm32.elf`
  The firmware of the stm32 used during the NorthSec conference. It has the
  BlackMagic gdb stub exposed via the USB to reprogram and debug the nrf51 chip.

- `nsec17_conf_nrf51.elf`
  The firmware of the nrf51 used during the NorthSec conference. It exposes a
  BLE service to change you avatar image and name.

- `nsec17_ctf_stm32.elf`
  The firmware of the stm32 used during the NorthSec CTF competition. It does
  *not* have the BlackMagic gdb stub. It exposes a serial device with
  challenges for the CTF. Read protection is enable when this firmware is
  started.

- `nsec17_ctf_nrf51.elf`
  The firmware of the nrf51 used during the NorthSec CTF competition. It is
  exactly the same as the conference firmware except it displays `CTF` on the
  status bar at the top of the display.

- `nsec17_ctf_nrf51_namechange.elf`
  One badge was running the firmware with the actual flag on the admin table
  during the competition. The flag is shown when the name on it is change via
  BLE.

- `nsec17_ctf_nrf51_rao.elf`
  Rao's badge. The ELF file was distributed during the CTF. One badge was
  running the firmware with the actual flag on the admin table during the
  competition.

- `nsec17_stm32_crossdebug.elf`
  Same as the stm32 conference firmware, except the debugger uses the external
  pins to allow programming and debugging the stm32 microcontroller of another
  badge.

## Cookbook

Here are the steps to get you started. Lets say you've downloaded the source into the
`nsec_badge` folder.

### stm32

#### BlackMagic

The blackmagic firmware that runs on the stm32 let you debug and
flash firmware on the nrf51.

To compile the blackmagic firmware for the stm32:

    % cd stm32
    % git submodule init
    % git submodule update
    % make -C blackmagic PROBE_HOST=nsec17

#### CTF

The CTF firmware will expose 4 challenges over an USB/ACM device.

To compile the CTF firmware for the stm32:

    % make -C libopencm3 TARGETS=stm32/f0
    % make -C src BINARY=nsec17_stm32_ctf

#### Flashing the firmware

To flash the firmware on the stm32, boot the uController into DFU mode
by pressing and holding the `PROGRAM` button, press `RESET`, then
releasing the `PROGRAM` button.

Make sure you see a DFU device:

    % lsusb
    Bus 001 Device 057: ID 0483:df11 STMicroelectronics STM Device in DFU Mode

Use [dfu-util](http://dfu-util.sourceforge.net/) to flash the firmware:

    % dfu-util --reset --device 0483:df11 --alt 0 --dfuse-address 0x08000000 --download nsec17_stm32.bin

### nrf51

To compile the binary for the nrf51:

    % cd nrf51
    % make nordicsdk
    % make

To flash the binary on the nrf51, you'll need to either:

* have the blackmagic firmware flashed onto the stm32
* use a BlackMagic device

Use the blackmagic exposed ACM device to flash the firmware using GDB.

