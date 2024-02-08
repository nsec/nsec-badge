idf_component_register(
    SRCS
        "ota_init.cpp"
        "ota_write.cpp"
    INCLUDE_DIRS "."
    REQUIRES console spi_flash driver main esp_rom
)
