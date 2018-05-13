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
load builds/nsec18_stm32_debugger.bin
file builds/nsec18_stm32_debugger.bin

define hook-quit
  set confirm off
end

if $QUIT_AFTER_FLASH
  quit
else
  run
end
