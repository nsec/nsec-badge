# SPDX-FileCopyrightText: 2025 NorthSec
#
# SPDX-License-Identifier: MIT

set(CONFIG_NSEC_BUILD_CONFERENCE $ENV{CONFIG_NSEC_BUILD_CONFERENCE} CACHE INTERNAL "")
set(CONFIG_NSEC_BUILD_CTF $ENV{CONFIG_NSEC_BUILD_CTF} CACHE INTERNAL "")

if(CONFIG_NSEC_BUILD_CONFERENCE)
        idf_component_register(SRCS "config_store.cpp" "badge_store.cpp" "utils.cpp"
                INCLUDE_DIRS "." "../" "../../.pio/libdeps/conference/fmt/include"
                REQUIRES driver nvs_flash
        )
endif()
