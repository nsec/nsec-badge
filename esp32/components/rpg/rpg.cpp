#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "rpg.h"
#include "rpg_control.h"

#include "rpg/Coordinates.h"
#include "rpg/Scene.h"
#include "rpg/characters/CharacterAngela.h"
#include "rpg/characters/CharacterAristocrate.h"
#include "rpg/characters/CharacterCharles.h"
#include "rpg/characters/CharacterChloe.h"
#include "rpg/characters/CharacterDancer.h"
#include "rpg/characters/CharacterDog.h"
#include "rpg/characters/CharacterDuck.h"
#include "rpg/characters/CharacterFarmer.h"
#include "rpg/characters/CharacterFil.h"
#include "rpg/characters/CharacterFisherman.h"
#include "rpg/characters/CharacterMerchant.h"
#include "rpg/characters/CharacterMonk.h"
#include "rpg/characters/CharacterOldman.h"
#include "rpg/characters/CharacterOldwoman.h"
#include "rpg/characters/CharacterPunk.h"
#include "rpg/characters/CharacterRed.h"
#include "rpg/characters/CharacterRetailer.h"
#include "rpg/characters/CharacterSailor.h"
#include "rpg/characters/CharacterSoldier.h"
#include "rpg/characters/CharacterVijay.h"
#include "rpg/characters/CharacterYue.h"
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

    rpg::CharacterAngela character_angela{};
    character_angela.set_blocked_data_reader(blocked_data_reader);
    scene.add_character(&character_angela);

    rpg::CharacterAristocrate character_aristocrate{};
    character_aristocrate.set_blocked_data_reader(blocked_data_reader);
    scene.add_character(&character_aristocrate);

    rpg::CharacterCharles character_charles{};
    scene.add_character(&character_charles);

    rpg::CharacterChloe character_chloe{};
    character_chloe.set_blocked_data_reader(blocked_data_reader);
    scene.add_character(&character_chloe);

    rpg::CharacterDancer character_dancer{};
    character_dancer.set_blocked_data_reader(blocked_data_reader);
    scene.add_character(&character_dancer);

    rpg::CharacterDog character_dog{};
    character_dog.set_blocked_data_reader(blocked_data_reader);
    scene.add_character(&character_dog);

    rpg::CharacterDuck character_duck{};
    character_duck.set_max_distance(40, 1);
    scene.add_character(&character_duck);

    rpg::CharacterFarmer character_farmer{};
    character_farmer.set_blocked_data_reader(blocked_data_reader);
    character_farmer.set_max_distance(180, 100);
    scene.add_character(&character_farmer);

    rpg::CharacterFil character_fil{};
    character_fil.set_blocked_data_reader(blocked_data_reader);
    scene.add_character(&character_fil);

    rpg::CharacterFisherman character_fisherman{};
    scene.add_character(&character_fisherman);

    rpg::CharacterMerchant character_merchant{};
    character_merchant.set_blocked_data_reader(blocked_data_reader);
    scene.add_character(&character_merchant);

    rpg::CharacterMonk character_monk{};
    character_monk.set_blocked_data_reader(blocked_data_reader);
    scene.add_character(&character_monk);

    rpg::CharacterOldman character_oldman{};
    scene.add_character(&character_oldman);

    rpg::CharacterOldwoman character_oldwoman{};
    character_oldwoman.set_blocked_data_reader(blocked_data_reader);
    scene.add_character(&character_oldwoman);

    rpg::CharacterPunk character_punk{};
    character_punk.set_blocked_data_reader(blocked_data_reader);
    scene.add_character(&character_punk);

    rpg::CharacterRed character_red{};
    character_red.set_blocked_data_reader(blocked_data_reader);
    character_red.set_max_distance(150, 240);
    scene.add_character(&character_red);

    rpg::CharacterRetailer character_retailer{};
    character_retailer.set_blocked_data_reader(blocked_data_reader);
    character_retailer.set_max_distance(40, 1);
    scene.add_character(&character_retailer);

    rpg::CharacterSailor character_sailor{};
    character_sailor.set_blocked_data_reader(blocked_data_reader);
    character_sailor.set_max_distance(18, 72);
    scene.add_character(&character_sailor);

    rpg::CharacterSoldier character_solider1{}, character_solider2{};
    character_solider1.set_blocked_data_reader(blocked_data_reader);
    character_solider2.set_blocked_data_reader(blocked_data_reader);
    character_solider1.set_max_distance(90, 48);
    character_solider2.set_max_distance(30, 100);
    scene.add_character(&character_solider1);
    scene.add_character(&character_solider2);

    rpg::CharacterVijay character_vijay{};
    character_vijay.set_blocked_data_reader(blocked_data_reader);
    scene.add_character(&character_vijay);

    rpg::CharacterYue character_yue{};
    character_yue.set_blocked_data_reader(blocked_data_reader);
    character_yue.set_max_distance(120, 120);
    scene.add_character(&character_yue);

    scene.get_viewport().move(rpg::GlobalCoordinates::xy(935, 611));

    mc.move(rpg::GlobalCoordinates::xy(1084, 712));
    character_angela.move_initial(rpg::GlobalCoordinates::xy(120, 210));
    character_aristocrate.move_initial(rpg::GlobalCoordinates::xy(760, 680));
    character_charles.move_initial(rpg::GlobalCoordinates::xy(105, 745));
    character_chloe.move_initial(rpg::GlobalCoordinates::xy(450, 313));
    character_dancer.move_initial(rpg::GlobalCoordinates::xy(740, 477));
    character_dog.move_initial(rpg::GlobalCoordinates::xy(262, 474));
    character_duck.move_initial(rpg::GlobalCoordinates::xy(589, 542));
    character_farmer.move_initial(rpg::GlobalCoordinates::xy(938, 140));
    character_fil.move_initial(rpg::GlobalCoordinates::xy(460, 720));
    character_fisherman.move_initial(rpg::GlobalCoordinates::xy(970, 1100));
    character_merchant.move_initial(rpg::GlobalCoordinates::xy(570, 700));
    character_monk.move_initial(rpg::GlobalCoordinates::xy(265, 1057));
    character_oldman.move_initial(rpg::GlobalCoordinates::xy(975, 753));
    character_oldwoman.move_initial(rpg::GlobalCoordinates::xy(250, 111));
    character_punk.move_initial(rpg::GlobalCoordinates::xy(24, 768));
    character_red.move_initial(rpg::GlobalCoordinates::xy(648, 805));
    character_retailer.move_initial(rpg::GlobalCoordinates::xy(550, 615));
    character_sailor.move_initial(rpg::GlobalCoordinates::xy(1017, 826));
    character_solider1.move_initial(rpg::GlobalCoordinates::xy(590, 1080));
    character_solider2.move_initial(rpg::GlobalCoordinates::xy(705, 1135));
    character_vijay.move_initial(rpg::GlobalCoordinates::xy(616, 1010));
    character_yue.move_initial(rpg::GlobalCoordinates::xy(660, 125));

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
