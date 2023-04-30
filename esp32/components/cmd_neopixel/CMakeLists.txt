idf_component_register(SRCS "cmd_neopixel.cpp"
                       INCLUDE_DIRS .
                       REQUIRES console spi_flash driver save neopixel
)
