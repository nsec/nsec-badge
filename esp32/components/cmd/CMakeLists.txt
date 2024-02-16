set(requires_components console driver fatfs cmd_nvs off_the_addon)

if(CONFIG_NSEC_BUILD_ADDON)
    list(APPEND requires_components challenges_storage)
endif()

idf_component_register(SRCS "console.c"
                       INCLUDE_DIRS "."
                       REQUIRES ${requires_components}
)
