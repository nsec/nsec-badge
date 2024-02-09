if(CONFIG_NSEC_BUILD_CTF_ADDON)
    idf_component_register(
        SRCS
            "challenges_storage.cpp"
            "flash_operations.cpp"
            "ota_operations.cpp"
            "spi_operations.cpp"
        INCLUDE_DIRS "." "../save"
        REQUIRES console spi_flash driver main
    )
endif()
