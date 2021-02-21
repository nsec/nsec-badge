idf_component_register(
    SRCS
        "rpg/Character.cpp"
        "rpg/MainCharacter.cpp"
        "rpg/Scene.cpp"
        "rpg/Viewport.cpp"
        "rpg/data/SceneDataReader.cpp"
        "rpg.cpp"
    INCLUDE_DIRS
        "."
    REQUIRES
        graphics
)

add_custom_command(
    OUTPUT
        ${SPIFFS_DIR}/rpg/main.scene
    COMMAND
        python ${COMPONENT_DIR}/../graphics/tools/compile-tilemap.py ${GRAPHICS_LIBRARY_DIR}/images-registry.txt ${GRAPHICS_LIBRARY_DIR}/rpg.main.scene.svg ${SPIFFS_DIR}/rpg/main.scene
    DEPENDS
        ${GRAPHICS_LIBRARY_DIR}/images-registry.txt
        ${GRAPHICS_LIBRARY_DIR}/rpg.main.scene.svg
)

add_custom_target(rpg_main_scene DEPENDS ${SPIFFS_DIR}/rpg/main.scene)
add_dependencies(graphics_library rpg_main_scene)
