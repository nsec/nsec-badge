cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Parse sdkconfig file
file(STRINGS "sdkconfig" sdkconfig_contents)
string(REGEX MATCH "CONFIG_NSEC_BUILD_CTF_ADDON=y" is_ctf_addon ${sdkconfig_contents})

if(is_ctf_addon)
    project(nsec-ctf-addon)
else()
    project(nsec-badge)
endif()

