if(CONFIG_NSEC_BUILD_ADDON)
    idf_component_register(
        SRCS
            "challenges_storage.cpp"
            "flash_operations.cpp"
            "spi_operations.cpp"
        INCLUDE_DIRS "." "../save"
        REQUIRES console spi_flash driver main
    )
endif()
