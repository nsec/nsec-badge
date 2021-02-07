idf_component_register(
    SRCS
        "main.c"
    INCLUDE_DIRS
        "."
)

spiffs_create_partition_image(storage ${SPIFFS_DIR} FLASH_IN_PROJECT)
