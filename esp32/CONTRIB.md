# Contribute to the 2024 badge!

# Build
```
idf.py menuconfig
# In NORTHSEC 2024 menu option, choose the firmware to build
# By default it will build the conference one as nsec-badge.bin
idf.py build
```

You can circle the 3 options to have the 3 firmware files in `build/` ready:
- nsec-badge.bin (conference)
- nsec-ctf.bin
- nsec-ctf-addon.bin

You can see which one is loaded when you boot the ESP32S3 with INFO logging
enabled:
```
I (232) cpu_start: Application information:
I (235) cpu_start: Project name:     nsec-badge
I (240) cpu_start: App version:      nsec21-161-g85d5823-dirty
```

# Provisioning
During test you can use `idf.py flash` to load any last built firmware into
factory.

In production, for the initial conference firmware select "NorthSec Conference"
in `idf.py menuconfig` and then do `idf.py flash`. This will also create the
needed partitions for the CTF and Addon.

At the CTF event admin table, `parttool.py write_partition
--partition-name=ota_0 --input build\nsec-ctf.bin` to load the firmware into
`OTA` slot `0`. To make the badge boot into it after, use `otatool.py
switch_ota_partition --slot 0`. Inside the badge console there's a
`firmware_select` command that should be faster.

The CTF Addon is self-provisioning when the badge is booted with the CTF Addon
connected properly.

# Code
We have the following macro defined for code built exclusively for that
firmware:
- *CONFIG_NSEC_BUILD_CONFERENCE*
- *CONFIG_NSEC_BUILD_CTF*
- *CONFIG_NSEC_BUILD_ADDON*

In your C or C++ files, you can do it like that:
```
#if CONFIG_NSEC_BUILD_CTF
    #include "my_ctf_only_lib.h"
    my_ctf_only_function();
#endif
```

In your `CMakeLists.txt` files, you can choose to only build a component like
that:
```
if(CONFIG_NSEC_BUILD_CTF)
    idf_component_register(
        SRCS
            "myfile.cpp"
            "myfile2.cpp"
        INCLUDE_DIRS "." "../save"
        REQUIRES console
    )
endif()
```

If you do that, you'll need to conditionaly load your component like that:
```
set(requires_components console)

if(CONFIG_NSEC_BUILD_CTF)
    list(APPEND requires_components ctfonlycomponent)
endif()

idf_component_register(SRCS "console.c"
                    INCLUDE_DIRS "."
                    REQUIRES ${requires_components}
)
```

You can also use that last trick to load only some sources:
```
set(SOURCES
    "file1.cpp"
    "file2.cpp"
)

if(CONFIG_NSEC_BUILD_CTF)
    list(APPEND SOURCES "file2.cpp")
endif()

idf_component_register(
    SRCS ${SOURCES}
)
```

# Testing
## CMakeLists
If you change things with CMakeLists.txt files, you can test with:
```
cd build
cmake .. -G Ninja
```
This is the step that `idf.py build` does when running CMake.
If you are running for the first time and don't have a `sdkconfig` file, run
`cmake .. -G Ninja` *twice*.

# Common issues
## Wrong build target
If you see `Building ESP-IDF components for target esp32` when building and get
weird include errors,
this means that you are on the wrong target. Sometimes even with the
`sdkconfig.defaults` files stating it, it can be buggy.
Simply run `idf.py set-target esp32s3` to rectify.
