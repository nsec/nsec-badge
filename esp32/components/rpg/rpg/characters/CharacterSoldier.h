#pragma once

#include "rpg/Character.h"
#include "rpg/characters/MovingMixin.h"

#include "graphics.h"

namespace rpg::animation
{

static constexpr int soldier_moving_down[] = {
    LIBRARY_IMAGE_SOLDIER_DOWN_1,
    LIBRARY_IMAGE_SOLDIER_DOWN_2,
};

static constexpr int soldier_moving_left[] = {
    LIBRARY_IMAGE_SOLDIER_LEFT_1,
    LIBRARY_IMAGE_SOLDIER_LEFT_2,
    LIBRARY_IMAGE_SOLDIER_LEFT_1,
    LIBRARY_IMAGE_SOLDIER_LEFT_3,
};

static constexpr int soldier_moving_right[] = {
    LIBRARY_IMAGE_SOLDIER_RIGHT_1,
    LIBRARY_IMAGE_SOLDIER_RIGHT_2,
    LIBRARY_IMAGE_SOLDIER_RIGHT_1,
    LIBRARY_IMAGE_SOLDIER_RIGHT_3,
};

static constexpr int soldier_moving_up[] = {
    LIBRARY_IMAGE_SOLDIER_UP_1,
    LIBRARY_IMAGE_SOLDIER_UP_2,
};

} // namespace rpg::animation

namespace rpg::dialog
{

static const char *soldier_dialog_1[] = {
    "Stop right there!",
    "",
};

static const char *soldier_dialog_2[] = {
    "Did you hear?\n",
    "What is that?",
    "",
};

static const char *soldier_dialog_3[] = {
    "What's the password?",
    "",
};

static const char *soldier_dialog_4[] = {
    "You can't do that!",
    "",
};

} // namespace rpg::dialog

namespace rpg
{

class CharacterSoldier : virtual public Character, public MovingMixin
{
  public:
    CharacterSoldier() : Character(SceneObjectIdentity::GENERIC, 22, 29, 11, 26)
    {
        set_animation_variant(Appearance::moving_down,
                              animation::soldier_moving_down, 2);

        set_animation_variant(Appearance::moving_left,
                              animation::soldier_moving_left, 4);

        set_animation_variant(Appearance::moving_right,
                              animation::soldier_moving_right, 4);

        set_animation_variant(Appearance::moving_up,
                              animation::soldier_moving_up, 2);
    }

    virtual const char **get_dialog() override
    {
        if (next_dialog == dialog::soldier_dialog_1) {
            next_dialog = dialog::soldier_dialog_2;
            return dialog::soldier_dialog_1;
        }

        if (next_dialog == dialog::soldier_dialog_2) {
            next_dialog = dialog::soldier_dialog_3;
            return dialog::soldier_dialog_2;
        }

        if (next_dialog == dialog::soldier_dialog_3) {
            next_dialog = dialog::soldier_dialog_4;
            return dialog::soldier_dialog_3;
        }

        if (next_dialog == dialog::soldier_dialog_4) {
            next_dialog = dialog::soldier_dialog_1;
            return dialog::soldier_dialog_4;
        }

        next_dialog = dialog::soldier_dialog_2;
        return dialog::soldier_dialog_1;
    }

    virtual const char *get_name() const override
    {
        return "Guard";
    }

    virtual void post_render(Viewport &viewport) override;
    virtual void render(Viewport &viewport) override;

  private:
    enum class Mode {
        standing,
        walking,
    };

    Mode current_mode = Mode::standing;
    const char **next_dialog;

    void render_standing(Viewport &viewport);
    void render_walking(Viewport &viewport);
};

} // namespace rpg
