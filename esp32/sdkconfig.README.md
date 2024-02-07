# sdkconfig snafu

According to the official docs, sdkconfig can get messy:
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/kconfig.html#using-sdkconfig-defaults

Solution is to use a `sdkconfig.defaults` file instead and add `sdkconfig` to .gitignore

## How to update sdkconfig.defaults

- make a backup copy of `sdkconfig` and use later if you're lost
- delete `sdkconfig` or start from fresh branch/clone
- run `idf.py menuconfig` to get the current defaults
- change the options you want, exit menuconfig and save
- run `idf.py save-defconfig`
- validate that `sdkconfig.defaults` differs from previous version with your changes
- commit `sdkconfig.defaults`
- do NOT commit `sdkconfig`

