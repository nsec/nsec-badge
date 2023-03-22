idf_component_register(SRCS "console.c"
                       REQUIRES console driver fatfs cmd_nvs cmd_system
                       INCLUDE_DIRS ".")
