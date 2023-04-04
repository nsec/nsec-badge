idf_component_register(SRCS "neopixel.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES FastLED-idf WS2812FX-idf spi_flash save
)
