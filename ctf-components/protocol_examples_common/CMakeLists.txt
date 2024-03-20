# SPDX-FileCopyrightText: 2024 NorthSec
#
# SPDX-License-Identifier: MIT

set(CONFIG_NSEC_BUILD_CONFERENCE $ENV{CONFIG_NSEC_BUILD_CONFERENCE} CACHE INTERNAL "")
set(CONFIG_NSEC_BUILD_CTF $ENV{CONFIG_NSEC_BUILD_CTF} CACHE INTERNAL "")
set(CONFIG_NSEC_BUILD_ADDON $ENV{CONFIG_NSEC_BUILD_ADDON} CACHE INTERNAL "")

set(srcs "stdin_out.c"
         "addr_from_stdin.c"
         "connect.c"
         "wifi_connect.c")

if(CONFIG_EXAMPLE_PROVIDE_WIFI_CONSOLE_CMD)
    list(APPEND srcs "console_cmd.c")
endif()

if(CONFIG_EXAMPLE_CONNECT_ETHERNET)
    list(APPEND srcs "eth_connect.c")
endif()


idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES esp_netif driver esp_wifi vfs)

if(CONFIG_EXAMPLE_PROVIDE_WIFI_CONSOLE_CMD)
    idf_component_optional_requires(PRIVATE console)
endif()

if(CONFIG_EXAMPLE_CONNECT_ETHERNET)
    idf_component_optional_requires(PRIVATE esp_eth)
endif()
