# Parse sdkconfig file
if(EXISTS "${PROJECT_DIR}/sdkconfig")
    file(STRINGS "${PROJECT_DIR}/sdkconfig" sdkconfig_contents)
    string(REGEX MATCH "CONFIG_NSEC_BUILD_CTF_ADDON=y" config_ctf_addon_match ${sdkconfig_contents})
else()
    if(EXISTS "${PROJECT_DIR}/sdkconfig.defaults")
        file(STRINGS "${PROJECT_DIR}/sdkconfig.defaults" sdkconfig_contents)
        string(REGEX MATCH "CONFIG_NSEC_BUILD_CTF_ADDON=y" config_ctf_addon_match ${sdkconfig_contents})
    endif()
endif()

if("${config_ctf_addon_match}" STREQUAL "CONFIG_NSEC_BUILD_CTF_ADDON=y")
    set(CONFIG_NSEC_BUILD_CTF_ADDON 1 CACHE INTERNAL "")
else()
    set(CONFIG_NSEC_BUILD_CTF_ADDON 0 CACHE INTERNAL "")
endif()


idf_component_register(SRCS "main.cpp"
                       INCLUDE_DIRS "."
)
