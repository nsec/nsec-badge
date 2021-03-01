idf_component_register(
    SRCS
        "main.cpp"
    INCLUDE_DIRS
        "."
)

spiffs_create_partition_image(storage ${SPIFFS_DIR} FLASH_IN_PROJECT)

# Convert files in the graphics library before the spiffs partition is built.
# This dependency cannot be defined in the graphics component because the
# storage target is defined in the main component that is built last.
add_dependencies(spiffs_storage_bin graphics_library)
