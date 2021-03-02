#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "rpg.h"
#include "rpg_control.h"

#include "rpg/MainCharacter.h"
#include "rpg/Scene.h"
#include "rpg/data/BlockedDataReader.h"

#include "graphics.h"

void run_main_scene(void)
{
    rpg::Scene scene{"main", 1200, 1200};

    rpg::data::BlockedDataReader blocked_data_reader{"main", 1200, 1200};
    rpg::MainCharacter mc{blocked_data_reader};
    scene.add_character(&mc);

    scene.get_viewport().move_to_tile(13, 21);

    // Just return back to the main scene on any exit action, as temporary
    // implementation.
    while (true) {
        rpg_control_take(scene);

        scene.get_viewport().mark_for_full_refresh();
        scene.render();

        // Scene fade out effect.
        for (int i = 0; i <= 13; ++i) {
            graphics_fadeout_display_buffer(20);
            graphics_update_display();
        }

        vTaskDelay(200);
    }
}
