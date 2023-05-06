set(SOURCES
    "badge/mesh/config.cpp"
    "badge/mesh/host.cpp"
    "badge/mesh/main.cpp"
    "badge/mesh/models.cpp"
    "badge/mesh/network.cpp"
    "badge/mesh/ops.cpp"
    "badge/mesh/ops/ping.cpp"
    "badge/mesh/ops/census.cpp"
    "badge/mesh/ops/info.cpp"
    "badge/mesh/ops/neopixel.cpp"
    "badge/mesh/ops/realtimeled.cpp"
    "cmd_badge_mesh.cpp"
)

idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS "."
    REQUIRES driver bt mbedtls console nvs_flash neopixel
)
