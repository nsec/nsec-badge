# SPDX-FileCopyrightText: 2024 NorthSec
#
# SPDX-License-Identifier: MIT

### Commenting out while cleaning up base conf build 
# set(CONFIG_NSEC_BUILD_CONFERENCE $ENV{CONFIG_NSEC_BUILD_CONFERENCE} CACHE INTERNAL "")
# set(CONFIG_NSEC_BUILD_CTF $ENV{CONFIG_NSEC_BUILD_CTF} CACHE INTERNAL "")

#if(CONFIG_NSEC_BUILD_CONFERENCE)
#        idf_component_register(SRCS "globals.cpp"
#                INCLUDE_DIRS "." "../" "../../.pio/libdeps/conference/fmt/include"  "../../.pio/libdeps/conference/SmartLeds/src"
#                REQUIRES driver nvs_flash
#        )
#endif()