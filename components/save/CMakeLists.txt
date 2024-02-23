# SPDX-FileCopyrightText: 2024 NorthSec
#
# SPDX-License-Identifier: MIT

idf_component_register(SRCS "save.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES nvs_flash
)
