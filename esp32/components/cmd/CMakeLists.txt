idf_component_register(SRCS "console.c"
                       INCLUDE_DIRS "."
                       REQUIRES console driver fatfs cmd_nvs cmd_neopixel neopixel challenges_led badge_mesh cmd_math
)
