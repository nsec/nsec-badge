# SPDX-FileCopyrightText: 2024 NorthSec
#
# SPDX-License-Identifier: MIT

set(CONFIG_NSEC_BUILD_CONFERENCE $ENV{CONFIG_NSEC_BUILD_CONFERENCE} CACHE INTERNAL "")
set(CONFIG_NSEC_BUILD_CTF $ENV{CONFIG_NSEC_BUILD_CTF} CACHE INTERNAL "")
set(CONFIG_NSEC_BUILD_ADDON $ENV{CONFIG_NSEC_BUILD_ADDON} CACHE INTERNAL "")

if(CONFIG_NSEC_BUILD_ADDON OR CONFIG_NSEC_BUILD_CTF)
    idf_component_register(
        SRCS
            "ota_init.cpp"
            "ota_actions.cpp"
        INCLUDE_DIRS "."
        REQUIRES console spi_flash driver main esp_rom
    )
endif()
