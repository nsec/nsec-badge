if(CONFIG_NSEC_BUILD_ADDON)
    idf_component_register(
        SRCS
            "crypto_atecc_cmd.cpp"
            "crypto_atecc_init.cpp"
            "crypto_operations.cpp"
        INCLUDE_DIRS "."
        REQUIRES save console esp-cryptoauthlib
    )
endif()
