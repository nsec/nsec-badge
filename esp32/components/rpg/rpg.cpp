#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "rpg.h"
#include "rpg_control.h"

#include "rpg/Coordinates.h"
#include "rpg/Scene.h"
#include "rpg/characters/MainCharacter.h"
#include "rpg/data/BlockedDataReader.h"

#include "graphics.h"

void run_main_scene(void)
{
    rpg::Scene scene{"main", rpg::GlobalCoordinates::xy(1200, 1200)};

    rpg::data::BlockedDataReader blocked_data_reader{
        "main", rpg::GlobalCoordinates::xy(1200, 1200)};

    rpg::MainCharacter mc{blocked_data_reader};
    scene.add_character(&mc);

    scene.get_viewport().move(rpg::GlobalCoordinates::tile(14, 24));
    mc.move(rpg::GlobalCoordinates::xy(630, 815));

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
