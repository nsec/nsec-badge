if(CONFIG_NSEC_BUILD_CTF)
idf_component_register(SRCS "wifi.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES driver save modbus
)
endif()