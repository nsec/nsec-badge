#pragma once

#include "rpg/Character.h"
#include "rpg/characters/MovingMixin.h"

#include "graphics.h"

namespace rpg::animation
{

static constexpr int red_moving_down[] = {
    LIBRARY_IMAGE_RED_DOWN_1,
    LIBRARY_IMAGE_RED_DOWN_2,
};

static constexpr int red_moving_left[] = {
    LIBRARY_IMAGE_RED_LEFT_1,
    LIBRARY_IMAGE_RED_LEFT_2,
    LIBRARY_IMAGE_RED_LEFT_1,
    LIBRARY_IMAGE_RED_LEFT_3,
};

static constexpr int red_moving_right[] = {
    LIBRARY_IMAGE_RED_RIGHT_1,
    LIBRARY_IMAGE_RED_RIGHT_2,
    LIBRARY_IMAGE_RED_RIGHT_1,
    LIBRARY_IMAGE_RED_RIGHT_3,
};

static constexpr int red_moving_up[] = {
    LIBRARY_IMAGE_RED_UP_1,
    LIBRARY_IMAGE_RED_UP_2,
};

} // namespace rpg::animation

namespace rpg::dialog
{

static const char *red_dialog_1[] = {
    "I am this city's\n",
    "antivirus.",
    "",
};

static const char *red_dialog_2[] = {
    "Repeat after me:\n",
    "EICAR-STANDARD-ANTI\n",
    "VIRUS-TEST-FILE",
    "",
};

static const char *red_dialog_3[] = {
    "I hear a lot that\n",
    "they \"bring their\n",
    "own land.\" How will\n",
    "it fit through the\n",
    "gate?!",
    "",
};

static const char *red_dialog_4[] = {
    "I am watching that\n",
    "guy over there. I\n",
    "think he has a fork\n",
    "bomb.",
    "",
};

static const char *red_dialog_5[] = {
    "Go sit in a\n",
    "detonation box.",
    "",
};

} // namespace rpg::dialog

namespace rpg
{

class CharacterRed : virtual public Character, public MovingMixin
{
  public:
    CharacterRed() : Character(SceneObjectIdentity::GENERIC, 22, 29, 11, 28)
    {
        set_animation_variant(Appearance::moving_down,
                              animation::red_moving_down, 2);

        set_animation_variant(Appearance::moving_left,
                              animation::red_moving_left, 4);

        set_animation_variant(Appearance::moving_right,
                              animation::red_moving_right, 4);

        set_animation_variant(Appearance::moving_up, animation::red_moving_up,
                              2);
    }

    virtual const char **get_dialog() override
    {
        if (next_dialog == dialog::red_dialog_1) {
            next_dialog = dialog::red_dialog_2;
            return dialog::red_dialog_1;
        }

        if (next_dialog == dialog::red_dialog_2) {
            next_dialog = dialog::red_dialog_3;
            return dialog::red_dialog_2;
        }

        if (next_dialog == dialog::red_dialog_3) {
            next_dialog = dialog::red_dialog_4;
            return dialog::red_dialog_3;
        }

        if (next_dialog == dialog::red_dialog_4) {
            next_dialog = dialog::red_dialog_5;
            return dialog::red_dialog_4;
        }

        if (next_dialog == dialog::red_dialog_5) {
            next_dialog = dialog::red_dialog_1;
            return dialog::red_dialog_5;
        }

        next_dialog = dialog::red_dialog_2;
        return dialog::red_dialog_1;
    }

    virtual const char *get_name() const override
    {
        return "Rouge";
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
};

} // namespace rpg
