# SPDX-FileCopyrightText: 2024 NorthSec
#
# SPDX-License-Identifier: MIT

set(CONFIG_NSEC_BUILD_CONFERENCE $ENV{CONFIG_NSEC_BUILD_CONFERENCE} CACHE INTERNAL "")
set(CONFIG_NSEC_BUILD_CTF $ENV{CONFIG_NSEC_BUILD_CTF} CACHE INTERNAL "")
set(CONFIG_NSEC_BUILD_ADDON $ENV{CONFIG_NSEC_BUILD_ADDON} CACHE INTERNAL "")

idf_component_register(SRCS "codenames.cpp"
            INCLUDE_DIRS "." "../" "../../.pio/libdeps/conference/fmt/include" "../../.pio/libdeps/conference/SmartLeds/src" "../../components/utils"
            REQUIRES console driver save ssd1306 mbedtls driver utils nvs_flash)