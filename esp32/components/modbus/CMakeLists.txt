idf_component_register(SRCS "modbus.cpp"
                INCLUDE_DIRS "."
                REQUIRES mb_example_common protocol_examples_common esp_wifi nvs_flash)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")

