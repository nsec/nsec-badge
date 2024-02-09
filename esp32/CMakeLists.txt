cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

if(CONFIG_NSEC_BUILD_CTF_ADDON)
    project(nsec-ctf-addon)
else()
    project(nsec-badge)
endif()
