# SPDX-FileCopyrightText: 2025 NorthSec
#
# SPDX-License-Identifier: MIT

set(CONFIG_NSEC_BUILD_CONFERENCE $ENV{CONFIG_NSEC_BUILD_CONFERENCE} CACHE INTERNAL "")
set(CONFIG_NSEC_BUILD_CTF $ENV{CONFIG_NSEC_BUILD_CTF} CACHE INTERNAL "")

if(CONFIG_NSEC_BUILD_CTF)
        idf_component_register(SRCS "save.cpp"
                        INCLUDE_DIRS "."
                        REQUIRES nvs_flash
        )
endif()
