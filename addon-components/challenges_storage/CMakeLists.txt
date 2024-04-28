if(CONFIG_NSEC_BUILD_ADDON)
    idf_component_register(
        SRCS
            "challenges_storage.cpp"
            "flash_operations.cpp"
            "spi_operations.cpp"
        INCLUDE_DIRS "."
        REQUIRES crypto_atecc console save spi_flash driver main esp_rom
    )
endif()
