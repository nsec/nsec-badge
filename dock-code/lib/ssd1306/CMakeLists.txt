# SPDX-FileCopyrightText: 2024 NorthSec
#
# SPDX-License-Identifier: MIT

set(component_srcs "ssd1306.c" "ssd1306_spi.c" "badge_ssd1306_helper.cpp")

# get IDF version for comparison
set(idf_version "${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}")

#if(idf_version VERSION_GREATER_EQUAL "5.2")
#	if(CONFIG_LEGACY_DRIVER)
#		list(APPEND component_srcs "ssd1306_i2c_legacy.c")
#	else()
#		list(APPEND component_srcs "ssd1306_i2c_new.c")
#	endif()
#else()
list(APPEND component_srcs "ssd1306_i2c_legacy.c")
#endif()

idf_component_register(SRCS "${component_srcs}" REQUIRES driver console driver INCLUDE_DIRS "." "../" )
