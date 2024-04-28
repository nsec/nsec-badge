# SPDX-FileCopyrightText: 2024 NorthSec
#
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(ENV{CONFIG_NSEC_BUILD_CTF} ${CONFIG_NSEC_BUILD_CTF})
set(ENV{CONFIG_NSEC_BUILD_ADDON} ${CONFIG_NSEC_BUILD_ADDON})
set(ENV{CONFIG_NSEC_BUILD_CONFERENCE} ${CONFIG_NSEC_BUILD_CONFERENCE})

# Use an sdkconfig.defaults per environment and common sdkconfig.defaults
set(SDKCONFIG_DEFAULTS "${SDKCONFIG}.defaults;sdkconfig.defaults")
message(STATUS "-- Using defaults: ${SDKCONFIG_DEFAULTS}")

# Add CTF specific components to the build
if(CONFIG_NSEC_BUILD_CTF)
list(APPEND EXTRA_COMPONENT_DIRS "./ctf-components/")
endif()

# These project names are used for firmware detection, don't change without
# refactor
if(CONFIG_NSEC_BUILD_CTF)
    message(STATUS "-- Building for CONFIG_NSEC_BUILD_CTF")
    project(nsec-ctf)
elseif(CONFIG_NSEC_BUILD_ADDON)
    message(STATUS "-- Building for CONFIG_NSEC_BUILD_ADDON")
    project(nsec-ctf-addon)
elseif(CONFIG_NSEC_BUILD_CONFERENCE)
    message(STATUS "-- Building for CONFIG_NSEC_BUILD_CONFERENCE")
    project(nsec-badge)
else()
    message(FATAL_ERROR "Invalid firmware configuration.")
endif()
