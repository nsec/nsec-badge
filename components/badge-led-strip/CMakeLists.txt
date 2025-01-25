# SPDX-FileCopyrightText: 2024 NorthSec
#
# SPDX-License-Identifier: MIT

set(CONFIG_NSEC_BUILD_CONFERENCE $ENV{CONFIG_NSEC_BUILD_CONFERENCE} CACHE INTERNAL "")
set(CONFIG_NSEC_BUILD_CTF $ENV{CONFIG_NSEC_BUILD_CTF} CACHE INTERNAL "")

if(CONFIG_NSEC_BUILD_CONFERENCE)
        idf_component_register(SRCS "strip_animator.cpp"
                        INCLUDE_DIRS "." "../" "../../.pio/libdeps/conference/fmt/include" "../../.pio/libdeps/conference/SmartLeds/src"
                        REQUIRES driver
        )
endif()
