set gnutarget elf32-littlearm
define hook-quit
    set confirm off
end

target extended-remote /dev/ttyACM0
monitor tpwr enable
monitor swdp_scan
attach 1
set mem inaccessible-by-default off
set debug arm

load stm32_nsec17.elf
file stm32_nsec17.elf
run


