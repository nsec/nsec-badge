# SPDX-FileCopyrightText: 2024 NorthSec
#
# SPDX-License-Identifier: MIT

idf_component_register(SRCS "qkd.cpp"
            INCLUDE_DIRS "." "../" 
            REQUIRES console driver ssd1306)