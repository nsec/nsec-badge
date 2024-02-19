cmake_minimum_required(VERSION 3.16)

# Parse sdkconfig files because CMake doesn't have the variables yet
if(EXISTS "${CMAKE_SOURCE_DIR}/sdkconfig")
    file(STRINGS "${CMAKE_SOURCE_DIR}/sdkconfig" sdkconfig_contents)
else()
    if(EXISTS "${CMAKE_SOURCE_DIR}/sdkconfig.defaults")
        file(STRINGS "${CMAKE_SOURCE_DIR}/sdkconfig.defaults" sdkconfig_contents)
    else()
        # no sdkconfig files found in project, that is weird but let's support
        # it
        set(sdkconfig_contents "")
    endif()
endif()

string(REGEX MATCH "CONFIG_NSEC_BUILD_CONFERENCE=y" config_conf_match ${sdkconfig_contents})
string(REGEX MATCH "CONFIG_NSEC_BUILD_CTF=y" config_ctf_match ${sdkconfig_contents})
string(REGEX MATCH "CONFIG_NSEC_BUILD_ADDON=y" config_addon_match ${sdkconfig_contents})

if("${config_conf_match}" STREQUAL "CONFIG_NSEC_BUILD_CONFERENCE=y")
    set(CONFIG_NSEC_BUILD_CONFERENCE 1 CACHE INTERNAL "")
else()
    set(CONFIG_NSEC_BUILD_CONFERENCE 0 CACHE INTERNAL "")
endif()

if("${config_ctf_match}" STREQUAL "CONFIG_NSEC_BUILD_CTF=y")
    set(CONFIG_NSEC_BUILD_CTF 1 CACHE INTERNAL "")
else()
    set(CONFIG_NSEC_BUILD_CTF 0 CACHE INTERNAL "")
endif()

if("${config_addon_match}" STREQUAL "CONFIG_NSEC_BUILD_ADDON=y")
    set(CONFIG_NSEC_BUILD_ADDON 1 CACHE INTERNAL "")
else()
    set(CONFIG_NSEC_BUILD_ADDON 0 CACHE INTERNAL "")
endif()

set(ENV{NSEC_BUILD_CONFERENCE} ${CONFIG_NSEC_BUILD_CONFERENCE})
set(ENV{NSEC_BUILD_CTF} ${CONFIG_NSEC_BUILD_CTF})
set(ENV{NSEC_BUILD_ADDON} ${CONFIG_NSEC_BUILD_ADDON})

set(EXTRA_COMPONENT_DIRS "./extra-components/")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# These project names are used for firmware detection, don't change without
# refactor
if(CONFIG_NSEC_BUILD_CTF)
    message(STATUS "Building for CONFIG_NSEC_BUILD_CTF")
    project(nsec-ctf)
elseif(CONFIG_NSEC_BUILD_ADDON)
    message(STATUS "Building for CONFIG_NSEC_BUILD_ADDON")
    project(nsec-ctf-addon)
else() #default CONFIG_NSEC_BUILD_CONFERENCE
    message(STATUS "Building for CONFIG_NSEC_BUILD_CONFERENCE")
    project(nsec-badge)
endif()
