# SPDX-FileCopyrightText: 2024 NorthSec
#
# SPDX-License-Identifier: MIT

idf_component_register(SRCS "dbg-led.cpp"
        INCLUDE_DIRS "."
        REQUIRES driver badge
)
