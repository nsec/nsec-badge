# SPDX-FileCopyrightText: 2025 NorthSec
#
# SPDX-License-Identifier: MIT

idf_component_register(SRCS "cmd_sys.cpp"
                INCLUDE_DIRS .
                REQUIRES badge console nvs_flash app_update
)
