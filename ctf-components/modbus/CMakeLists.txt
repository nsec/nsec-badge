# SPDX-FileCopyrightText: 2024 NorthSec
#
# SPDX-License-Identifier: MIT

set(CONFIG_NSEC_BUILD_CONFERENCE $ENV{CONFIG_NSEC_BUILD_CONFERENCE} CACHE INTERNAL "")
set(CONFIG_NSEC_BUILD_CTF $ENV{CONFIG_NSEC_BUILD_CTF} CACHE INTERNAL "")
set(CONFIG_NSEC_BUILD_ADDON $ENV{CONFIG_NSEC_BUILD_ADDON} CACHE INTERNAL "")

idf_component_register(SRCS "modbus.cpp"
            INCLUDE_DIRS "."
            REQUIRES mb_example_common protocol_examples_common esp_wifi nvs_flash)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
