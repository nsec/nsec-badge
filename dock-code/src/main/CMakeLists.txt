# SPDX-FileCopyrightText: 2025 NorthSec
#
# SPDX-License-Identifier: MIT

idf_component_register(SRCS "main.cpp"
            INCLUDE_DIRS "." "../"
            REQUIRES console driver ssd1306)
