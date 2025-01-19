# SPDX-FileCopyrightText: 2024 NorthSec
#
# SPDX-License-Identifier: MIT

set(CONFIG_NSEC_BUILD_CONFERENCE $ENV{CONFIG_NSEC_BUILD_CONFERENCE} CACHE INTERNAL "")
set(CONFIG_NSEC_BUILD_CTF $ENV{CONFIG_NSEC_BUILD_CTF} CACHE INTERNAL "")
set(CONFIG_NSEC_BUILD_ADDON $ENV{CONFIG_NSEC_BUILD_ADDON} CACHE INTERNAL "")

set(requires_components console driver cmd_nvs cmd_sys)

if(CONFIG_NSEC_BUILD_CTF)
    list(APPEND requires_components modbus wifi ir)
endif()

idf_component_register(SRCS "console.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES ${requires_components}
)
