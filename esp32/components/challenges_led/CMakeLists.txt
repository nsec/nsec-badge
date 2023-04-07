idf_component_register(
    SRCS
        "challenges_led.cpp"
        "challenge_led1.cpp"
    INCLUDE_DIRS .
    REQUIRES console FastLED-idf neopixel
)
