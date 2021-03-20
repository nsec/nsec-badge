#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "rpg.h"
#include "rpg_control.h"

#include "rpg/Coordinates.h"
#include "rpg/Scene.h"
#include "rpg/characters/CharacterArcher.h"
#include "rpg/characters/CharacterAristocrate.h"
#include "rpg/characters/CharacterCharles.h"
#include "rpg/characters/CharacterDancer.h"
#include "rpg/characters/CharacterFisherman.h"
#include "rpg/characters/MainCharacter.h"
#include "rpg/data/BlockedDataReader.h"

#include "graphics.h"

void run_main_scene(void)
{
    rpg::Scene scene{"main", rpg::GlobalCoordinates::xy(1200, 1200)};

    rpg::data::BlockedDataReader blocked_data_reader{
        "main", rpg::GlobalCoordinates::xy(1200, 1200)};

    rpg::MainCharacter mc{};
    mc.set_blocked_data_reader(blocked_data_reader);
    scene.add_character(&mc);

    rpg::CharacterArcher character_archer{};
    scene.add_character(&character_archer);

    rpg::CharacterAristocrate character_aristocrate{};
    character_aristocrate.set_blocked_data_reader(blocked_data_reader);
    scene.add_character(&character_aristocrate);

    rpg::CharacterFisherman character_fisherman{};
    scene.add_character(&character_fisherman);

    rpg::CharacterCharles character_charles{};
    scene.add_character(&character_charles);

    rpg::CharacterDancer character_dancer{};
    character_dancer.set_blocked_data_reader(blocked_data_reader);
    scene.add_character(&character_dancer);

    scene.get_viewport().move(rpg::GlobalCoordinates::tile(22, 30));

    mc.move(rpg::GlobalCoordinates::xy(640, 815));
    character_archer.move(rpg::GlobalCoordinates::xy(293, 1034));
    character_aristocrate.move(rpg::GlobalCoordinates::xy(760, 680));
    character_fisherman.move(rpg::GlobalCoordinates::xy(970, 1100));
    character_charles.move(rpg::GlobalCoordinates::xy(105, 745));
    character_dancer.move(rpg::GlobalCoordinates::xy(740, 477));

    // Just return back to the main scene on any exit action, as temporary
    // implementation.
    while (true) {
        rpg::ControlExitAction action = rpg_control_take(scene);

        scene.get_viewport().mark_for_full_refresh();
        scene.render();

        // Scene fade out effect.
        graphics_fadeout_display_buffer(40);
        graphics_update_display();
        for (int i = 0; i <= 13; ++i) {
            graphics_fadeout_display_buffer(20);
            graphics_update_display();
        }

        switch (action) {
        case rpg::ControlExitAction::konami_code:
            graphics_draw_jpeg("/spiffs/nsec.jpeg", 0, 0);
            graphics_update_display();
            vTaskDelay(200);
            break;

        default:
            break;
        }
    }
}
