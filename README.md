Northsec 2019 Badge
===================

Brought to you by the Team badge for NorthSec.
----------------------------------------------

## Hardware overview

The NorthSec 2019 badge has two programmable micro-controllers:

 - The [Nordic Semiconductor](https://www.nordicsemi.com)
[`nRF52832`](https://www.nordicsemi.com/eng/Products/Bluetooth-low-energy2/nRF52832)
(nRF52)
 - The [STMicroelectronics](http://www.st.com/)
[`STM32F070F6P6`](http://www.st.com/content/st_com/en/products/microcontrollers/stm32-32-bit-arm-cortex-mcus/stm32-mainstream-mcus/stm32f0-series/stm32f0x0-value-line/stm32f070f6.html)
(stm32).

The nRF52 is based on the ARM Cortex-M4F core and runs the `armv7-m` instruction
set, it takes care of:

 - The LCD display
 - The Bluetooth Low Energy (BLE)
 - The battery management
 - The [NeoPixel](https://en.wikipedia.org/wiki/Adafruit_Industries#NeoPixel)
RGB LEDs
 - The buttons
 - The external 4 Mb (512 kB) flash storage ([Adesto AT25SF041](https://www.adestotech.com/wp-content/uploads/DS-AT25SF041_044.pdf))

The stm32 is based on the ARM Cortex-M0 core and runs the `armv6-m` instruction
set, it takes care of:

 - The USB port

The badge can be supplied by 1 ICR14500 3.7v Li-ion rechargeable battery and/or
the USB port. The battery will be charged when the USB port is connected.

If you wish to hack your badge or create a new one based on the hardware. All the information is available here: [hardware_2019](./hw/2019/)

You can also access directly the [CircuitMaker project](https://circuitmaker.com/Projects/Details/Eric-Tremblay-2/Northsec2019Badge)

## Compiling
You can either compile the code using CQFD and Docker or you can install the
toolchain and build the project by yourself. Using CQFD and Docker will make
sure you are using the right compiler version and toolchain.

### CQFD ( Preferred method )

Follow the steps to install CQFD and Docker, then simply run:

```
$ cqfd init && cqfd
```

[CQFD](https://github.com/savoirfairelinux/cqfd)

### Getting the toolchain

There is a `Makefile` for each micro-controller, but here are some stuff you'll
need to make it work.

To compile the badge firmware from source, your gcc need to be able generating
`armv6-m` code and link with the nano variant of
[newlib](https://sourceware.org/newlib/).

On OS X or macOS, you can use my formula for Homebrew:

```
$ brew tap marc-etienne/stm32
$ brew install arm-none-eabi-gcc
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
$ sudo apt install python3-pil python3-bs4 python3-usb PyYAML
```

or via [pip](https://pypi.org/project/pip/)

```
$ pip install -r requirements
```

### Dependencies

The nF52 firmware depends on the Nordic SDK v14.2.0. You can download and
extract the SDK by typing `make nordicsdk` in the `nrf52` directory. It will
also be done automatically when building the first time.

The nRF52 also uses the s132 "SoftDevice" v5.0.0 to painlessly enable Bluetooth
Low Energy (BLE).  The SoftDevice is distributed as a blob with the SDK, and is
found at `nrf52/nordicsdk/softdevice/s132/hex/s132_nrf52_5.0.0_softdevice.hex`.

NOTE: The Nordic SDK and the SoftDevice aren't free software. The licence
agreement is available at

    nrf52/nordicsdk/softdevice/s132/hex/s132_nrf52_5.0.0_licence-agreement.txt

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

The stm32 can also be flashed by holding the `DFU` button behind the badge
while pressing the `RESET` button. The stm32 will boot a specific on-chip
bootloader that implements the
[DFU](https://en.wikipedia.org/wiki/USB#Device_Firmware_Upgrade) interface.

Reset the board with the `DFU` button pressed, then release the button.

You should now see a DFU device appearing on your computer:

```
$ lsusb
[...]
Bus 001 Device 057: ID 0483:df11 STMicroelectronics STM Device in DFU Mode
[...]
```

Use a DFU compliant software to flash the STM32:
- [dfu-util](https://www.archlinux.org/packages/community/x86_64/dfu-util/) on
ArchLinux

To make a `bin` file from an ELF, run the following:

```
$ arm-none-eabi-objcopy -O binary builds/nsec19_stm32_debugger.elf builds/nsec19_stm32_debugger.bin
```

Run the following command:

```
$ dfu-util --device 0483:df11 --alt 0 --dfuse-address 0x08000000:leave --download builds/nsec19_stm32_debugger.bin
```

The STM32 should reset automagically, running the newly downloaded firmware. The
address of 0x08000000 is important, this is where the stm32 flash is mapped into
memory.

## Firmware of NorthSec 2019

There were 6 firmware images built for the NorthSec 2019 event.

* `nsec19_stm32_debugger.elf`: The firmware of the stm32 used during the
  NorthSec Conference & CTF.
* `nsec19_nrf52_conf.elf`: The firmware of the nRF52 used during the NorthSec
  Conference.
* `nsec19_nrf52_ctf.elf`: The firmware of the nRF52 used during the NorthSec
  CTF.
* `nsec19_stm32_soldering.elf`: The firmware of the stm32 used during the
  NorthSec CTF (soldering track).
* `nsec19_nrf52_soldering`: The firmware of the nRF52 used during the NorthSec
  CTF (soldering track).
* `nsec19_nrf52_bar_beacon.elf`: The firmware of the nRF52 used during the
  NorthSec CTF. It broadcast messages and a flag via BLE.

## Cookbook

Here are the steps to get your badge up and running from scratch (with the CTF
firmware, adjust the instructions to use other firmwares).

### stm32

The blackmagic firmware that runs on the stm32 lets you debug and flash the
firmware on the nRF52.  It also provides a bridge to communicate with the UART
of the nRF52

Make sure you have cloned the blackmagic submodule:

```
$ git submodule init
$ git submodule update
```

Compile it with:

```
$ make builds/nsec19_stm32_debugger.bin
```

Boot the stm32 in `DFU` mode by pressing the `RESET` button while holding the
`DFU` button.  Both buttons are at the back of the badge.  Make sure you see a
DFU device:

```
$ lsusb
[...]
Bus 001 Device 057: ID 0483:df11 STMicroelectronics STM Device in DFU Mode
[...]
```

Use [dfu-util](http://dfu-util.sourceforge.net/) to write the firmware on the
stm32:

```
$ dfu-util --device 0483:df11 --alt 0 --dfuse-address 0x08000000:leave --download builds/nsec19_stm32_debugger.bin
```

If everything went right, you should now see a USB device that looks like:

```
$ lsusb
[...]
Bus 004 Device 024: ID 1d50:6018 OpenMoko, Inc. Black Magic Debug Probe (Application)
[...]
```
And two ttyACM devices should have been created:

```
$ dmesg
[...]
[1785692.689392] cdc_acm 4-1:1.0: ttyACM0: USB ACM device
[1785692.692334] cdc_acm 4-1:1.2: ttyACM1: USB ACM device
[...]
```

You are now ready to flash the nRF52 chip.

### nRF52

To compile the binary for the nRF52:

```
$ cd nrf52
$ make FLAVOR=ctf
```

We will use `arm-none-eabi-gdb` to flash the firmware on the nRF52.  But first,
we need to do a bit of configuration.  Copy `nrf52/flash_config.gdb.template`
over to `nrf52/flash_config.gdb`.  Adjust the following:

* `target extended-remote <device>`: Use the name of the first ttyACM device
  created by blackmagic (typically /dev/ttyACM0 on Linux).
* `LOAD_SOFTDEVICE`: The SoftDevice needs to be written once on the nrf52, so
  set this to 1 at least the first time you flash your badge.  It won't be
  necessary to write the SoftDevice the subsequent times.  It won't hurt to do
  it multiple times, but it will just take more time unnecessarily.

Write the firmware on the nRF52 with:

```
$ cd nrf52
$ arm-none-eabi-gdb -nx -x flash.gdb
```

If you want to flash another firmware than the CTF firmware, update the paths
in `nrf52/flash.gdb` accordingly.

The badge contains an external flash chip to hold larger assets, such as images
for the games. To populate this flash, an extra step is required.  First, boot
in "flash mode" by pressing the "up" button while powering up the badge.  You
should see `Flash mode` appear on the screen.  Then write the flash with:

```
$ cd nrf52
$ make flash-external-flash FLAVOR=ctf
```

Note that the step above assumes that the second ttyACM device created by
blackmagic (the UART bridge to the nRF52) is `/dev/ttyACM1`.  Adjust the path
in `nrf52/utils/flash_client.py` if this assumption is wrong.

Reboot your badge, you should now have a complete CTF firmware!
