Northsec 2016 Badge
===================

## Hardware overview

The Northsec 2016 badge has two programmable micro-controllers:

 - The [Nordic Semiconductor](https://www.nordicsemi.com) [`nRF51822`](https://www.nordicsemi.com/eng/Products/Bluetooth-low-energy2/nRF51822) (nRF51) and
 - The [STMicroelectronics](http://www.st.com/) [`stm32f072cb`](http://www.st.com/content/st_com/en/products/microcontrollers/stm32-32-bit-arm-cortex-mcus/stm32f0-series/stm32f0x2/stm32f072cb.html) (stm32).

Both are based on the ARM Cortex-M0 core and runs the `armv6-m` instruction set.

The nRF51 takes care of:

 - The OLED display
 - The Bluetooth Low Energy
 - The battery management
 - The red and the green LEDs

The stm32 takes care of:

 - The touch buttons
 - The USB port

## Programming

### Via Serial Wire Debug (SWD)

Each micro-controller can be programmed using 6-pin Tag-Connect cable, the
[TC2030-CTX-NL](http://www.tag-connect.com/TC2030-CTX-NL), and a probe to talk
to the chip via SWD. We used the
[Blacksphere](https://web.archive.org/web/20160331030547/http://www.blacksphere.co.nz/main/index.php)
[Blackmagic](https://web.archive.org/web/20160331030217/http://www.blacksphere.co.nz/main/index.php/blackmagic)
probe. There is a [version 2](http://1bitsquared.com/collections/frontpage/products/black-magic-probe)
out there now. We haven't test it, but it should also work.

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
`load stm32f072cb.elf` only.

You can also use the (more expensive) STLink, but we do not have the hardware
to test instructions for it.

### Via USB

The stm32 can also be flashed by holding the "program" button behind the badge
while pressing the "reset" button. The stm32 will boot a specific bootloader
that  implements the DFU interface.

Reset the board with the "PROGRAM" button pushed in, then release the button.

You should now see a DFU device appearing on your computer:
    % lsusb
    ...
    Bus 001 Device 057: ID 0483:df11 STMicroelectronics STM Device in DFU Mode
    ...

Use a DFU compliant software to flash the STM32:
- [dfu-util](https://www.archlinux.org/packages/community/x86_64/dfu-util/) on ArchLinux

Run the following command:
    % dfu-util --reset --device 0483:df11 --alt 0 --dfuse-address 0x08000000 --download nsec17_stm32.bin

The STM32 should reset automagically, running the downloaded firmware.

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

The nF51 firmware depends on the Nordic SDK v6.1. You can download and extract
the SDK by typing `make nordicsdk` in the nrf51 directory. We know it is an old
version of the SDK but that's what we used anyway.

The nRF51 also uses the s110 "SoftDevice" v7.0 to painlessly enable Bluetooth
Low Energy (BLE). To create the a ELF file of the SoftDevice, use `make
_build/s110_nrf51822_7.0.0_softdevice.elf` to download and create the ELF file
from Nordic's binary distribution. Again, this is not the latest version, but
that's the one that was on the badges. Also note: the SoftDevice isn't free
software. The licence agreement (`s110_nrf51822_7.0.0_licence_agreement.pdf`)
will be downloaded as well.

The stm32 requires the STM32CUBE library. You can download and extract the library
by typing `make cube` in the stm32 directory. The license forbid the usage of the
the linkerscript using another toolchain than the one from
`Atollic TrueSTUDIO(R) product`. You have been warned that you might get sued if
you use this linker script!

