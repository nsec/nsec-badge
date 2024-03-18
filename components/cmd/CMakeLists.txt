# SPDX-FileCopyrightText: 2024 NorthSec
#
# SPDX-License-Identifier: MIT

set(requires_components console driver fatfs cmd_nvs off_the_addon)

if(CONFIG_NSEC_BUILD_CTF)
    list(APPEND requires_components modbus wifi)
endif()

if(CONFIG_NSEC_BUILD_ADDON OR CONFIG_NSEC_BUILD_CTF)
idf_component_register(SRCS "console.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES ${requires_components}
)
endif()
