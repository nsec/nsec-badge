idf_component_register(
    SRCS
        "images_registry.c"
        "palette.c"
        "graphics.c"
    INCLUDE_DIRS
        "."
    PRIV_REQUIRES
        spiffs
        st7789
)

add_custom_command(
    OUTPUT
        palette.txt
    COMMAND
        python ${COMPONENT_DIR}/tools/generate-palette.py ${GRAPHICS_LIBRARY_DIR} ${GRAPHICS_LIBRARY_DIR}/images-registry.txt palette.txt
    DEPENDS
        ${GRAPHICS_LIBRARY_DIR}/images-registry.txt
)

add_custom_command(
    OUTPUT
        ${COMPONENT_DIR}/images_registry.c
        ${COMPONENT_DIR}/images_registry.h
    COMMAND
        python ${COMPONENT_DIR}/tools/compile-images-registry.py ${GRAPHICS_LIBRARY_DIR}/images-registry.txt jpeg-registry.json ${COMPONENT_DIR}/images_registry.h ${COMPONENT_DIR}/images_registry.c
    DEPENDS
        ${GRAPHICS_LIBRARY_DIR}/images-registry.txt
        jpeg-registry.json
)

add_custom_command(
    OUTPUT
        ${COMPONENT_DIR}/palette.c
        ${COMPONENT_DIR}/palette.h
    COMMAND
        python ${COMPONENT_DIR}/tools/compile-palette.py palette.txt ${COMPONENT_DIR}/palette.h ${COMPONENT_DIR}/palette.c
    DEPENDS
        palette.txt
)

add_custom_target(
    graphics_library ALL
    COMMAND
        python ${COMPONENT_DIR}/tools/convert-all.py ${GRAPHICS_LIBRARY_DIR}/images-registry.txt palette.txt ${GRAPHICS_LIBRARY_DIR} ${SPIFFS_DIR}/library/ jpeg-registry.json
    BYPRODUCTS
        jpeg-registry.json
        ${SPIFFS_DIR}/library/jpeg
        ${SPIFFS_DIR}/library/maps
    DEPENDS
        palette.txt
)

add_custom_target(graphics_images_registry DEPENDS ${COMPONENT_DIR}/images_registry.c graphics_library)
add_custom_target(graphics_palette DEPENDS ${COMPONENT_DIR}/palette.c)
add_dependencies(${COMPONENT_LIB} graphics_images_registry graphics_palette)
