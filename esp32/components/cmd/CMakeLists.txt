set(requires_components console driver fatfs cmd_nvs)

if(${CONFIG_NSEC_BUILD_CTF_ADDON})
    list(APPEND requires_components challenges_storage)
endif()

idf_component_register(SRCS "console.c"
                    INCLUDE_DIRS "."
                    REQUIRES ${requires_components}
)

