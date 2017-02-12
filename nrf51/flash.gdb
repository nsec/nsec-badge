source flash_config.gdb

set gnutarget elf32-littlearm
monitor tpwr enable
monitor swdp_scan
attach 1
set mem inaccessible-by-default off
set debug arm
if $FORCE_ERASE_ALL
  monitor erase_mass
end
if $LOAD_SOFTDEVICE
  load _build/s130_nrf51_2.0.1_softdevice.elf
end
load _build/nsec16_badge.out
file _build/nsec16_badge.out

define hook-quit
  set confirm off
end

if $QUIT_AFTER_FLASH
  quit
else
  run
end
