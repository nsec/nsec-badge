idf_component_register(
    SRCS
        "challenges_led.cpp"
        "challenge_led0.cpp"
        "challenge_led1.cpp"
        "challenge_led2.cpp"
        "challenge_led3.cpp"
    INCLUDE_DIRS .
    REQUIRES console FastLED-idf neopixel
)
