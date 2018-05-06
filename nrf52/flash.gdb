source flash_config.gdb

set gnutarget elf32-littlearm
if $USE_EXTERNAL_DEBUGGER
  monitor tpwr enable
else
  set tdesc filename gdb-tdesc-cortex-m4f.xml
end
monitor swdp_scan
attach 1
set mem inaccessible-by-default off
set debug arm
if $FORCE_ERASE_ALL
  monitor erase_mass
end
if $LOAD_SOFTDEVICE
  load nordicsdk/softdevice/s132/hex/s132_nrf52_5.0.0_softdevice.hex
end
load builds/nsec18_nrf52_ctf.out
file builds/nsec18_nrf52_ctf.out

define hook-quit
  set confirm off
end

if $QUIT_AFTER_FLASH
  quit
else
  run
end
