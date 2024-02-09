# Parse sdkconfig files because CMake doesn't have the variables yet

if(EXISTS "${PROJECT_DIR}/sdkconfig")
    file(STRINGS "${PROJECT_DIR}/sdkconfig" sdkconfig_contents)
else()
    if(EXISTS "${PROJECT_DIR}/sdkconfig.defaults")
        file(STRINGS "${PROJECT_DIR}/sdkconfig.defaults" sdkconfig_contents)
    else()
        # no sdkconfig files found in project, that is weird but let's support it
        set(sdkconfig_contents "")
    endif()
endif()

string(REGEX MATCH "CONFIG_NSEC_BUILD_CONFERENCE=y" config_conf_match ${sdkconfig_contents})
string(REGEX MATCH "CONFIG_NSEC_BUILD_CTF=y" config_ctf_match ${sdkconfig_contents})
string(REGEX MATCH "CONFIG_NSEC_BUILD_ADDON=y" config_addon_match ${sdkconfig_contents})

if("${config_conf_match}" STREQUAL "CONFIG_NSEC_BUILD_CONFERENCE=y")
    message(STATUS "Building for CONFIG_NSEC_BUILD_CONFERENCE")
    set(CONFIG_NSEC_BUILD_CONFERENCE 1 CACHE INTERNAL "")
else()
    set(CONFIG_NSEC_BUILD_CONFERENCE 0 CACHE INTERNAL "")
endif()

if("${config_ctf_match}" STREQUAL "CONFIG_NSEC_BUILD_CTF=y")
    message(STATUS "Building for CONFIG_NSEC_BUILD_CTF")
    set(CONFIG_NSEC_BUILD_CTF 1 CACHE INTERNAL "")
else()
    set(CONFIG_NSEC_BUILD_CTF 0 CACHE INTERNAL "")
endif()

if("${config_addon_match}" STREQUAL "CONFIG_NSEC_BUILD_ADDON=y")
    message(STATUS "Building for CONFIG_NSEC_BUILD_ADDON")
    set(CONFIG_NSEC_BUILD_ADDON 1 CACHE INTERNAL "")
else()
    set(CONFIG_NSEC_BUILD_ADDON 0 CACHE INTERNAL "")
endif()


idf_component_register(SRCS "main.cpp"
                       INCLUDE_DIRS "."
)
