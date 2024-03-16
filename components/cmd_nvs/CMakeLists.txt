# SPDX-FileCopyrightText: 2024 NorthSec
#
# SPDX-License-Identifier: MIT

if(CONFIG_NSEC_BUILD_ADDON OR CONFIG_NSEC_BUILD_CTF)
        idf_component_register(SRCS "cmd_nvs.c"
                        INCLUDE_DIRS .
                        REQUIRES console nvs_flash
        )
endif()
