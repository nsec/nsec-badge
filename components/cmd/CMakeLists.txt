# SPDX-FileCopyrightText: 2024 NorthSec
#
# SPDX-License-Identifier: MIT

set(CONFIG_NSEC_BUILD_CONFERENCE $ENV{CONFIG_NSEC_BUILD_CONFERENCE} CACHE INTERNAL "")
set(CONFIG_NSEC_BUILD_CTF $ENV{CONFIG_NSEC_BUILD_CTF} CACHE INTERNAL "")
set(CONFIG_NSEC_BUILD_ADDON $ENV{CONFIG_NSEC_BUILD_ADDON} CACHE INTERNAL "")

if(CONFIG_NSEC_BUILD_ADDON OR CONFIG_NSEC_BUILD_CTF)

set(requires_components console driver fatfs cmd_nvs off_the_addon)

if(CONFIG_NSEC_BUILD_CTF)
    list(APPEND requires_components modbus wifi)
endif()

idf_component_register(SRCS "console.c"
                    INCLUDE_DIRS "."
                    REQUIRES ${requires_components}
)
endif()
