# SPDX-FileCopyrightText: 2024 NorthSec
#
# SPDX-License-Identifier: MIT

if(CONFIG_NSEC_BUILD_ADDON OR CONFIG_NSEC_BUILD_CTF)
        idf_component_register(SRCS "save.cpp"
                        INCLUDE_DIRS "."
                        REQUIRES nvs_flash
        )
endif()
