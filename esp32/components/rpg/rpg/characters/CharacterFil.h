#pragma once

#include "rpg/Character.h"
#include "rpg/characters/MovingMixin.h"

#include "graphics.h"

namespace rpg::animation
{

static constexpr int fil_moving_down[] = {
    LIBRARY_IMAGE_FIL_DOWN_1,
    LIBRARY_IMAGE_FIL_DOWN_2,
};

static constexpr int fil_moving_left[] = {
    LIBRARY_IMAGE_FIL_LEFT_1,
    LIBRARY_IMAGE_FIL_LEFT_2,
    LIBRARY_IMAGE_FIL_LEFT_3,
    LIBRARY_IMAGE_FIL_LEFT_2,
};

static constexpr int fil_moving_right[] = {
    LIBRARY_IMAGE_FIL_RIGHT_1,
    LIBRARY_IMAGE_FIL_RIGHT_2,
    LIBRARY_IMAGE_FIL_RIGHT_3,
    LIBRARY_IMAGE_FIL_RIGHT_2,
};

static constexpr int fil_moving_up[] = {
    LIBRARY_IMAGE_FIL_UP_1,
    LIBRARY_IMAGE_FIL_UP_2,
};

} // namespace rpg::animation

namespace rpg::dialog
{

static const char *fil_dialog_1[] = {
    "I don't often\n", "exit vi.\n",           "\n", "But when I do,\n",
    "it's with \n",    "sudo shutdown -r now", "",
};

static const char *fil_dialog_2[] = {
    "Q: How do you\n",
    "   generate a\n",
    "   random string?\n",
    "\n",
    "A: Put a Windows\n",
    "   user in front\n",
    "   of vi, and tell\n",
    "   them to exit.",
    "",
};

static const char *fil_dialog_3[] = {
    "I am not a quitter!\n",
    "I use vim. I don't\n",
    "know how to quit.",
    "",
};

static const char *fil_dialog_4[] = {
    "vi > emacs > nano",
    "",
};

} // namespace rpg::dialog

namespace rpg
{

class CharacterFil : virtual public Character, public MovingMixin
{
  public:
    CharacterFil() : Character(SceneObjectIdentity::GENERIC, 22, 29, 11, 26)
    {
        set_animation_variant(Appearance::moving_down,
                              animation::fil_moving_down, 2);

        set_animation_variant(Appearance::moving_left,
                              animation::fil_moving_left, 4);

        set_animation_variant(Appearance::moving_right,
                              animation::fil_moving_right, 4);

        set_animation_variant(Appearance::moving_up, animation::fil_moving_up,
                              2);
    }

    virtual const char **get_dialog() override
    {
        if (next_dialog == dialog::fil_dialog_1) {
            next_dialog = dialog::fil_dialog_2;
            return dialog::fil_dialog_1;
        }

        if (next_dialog == dialog::fil_dialog_2) {
            next_dialog = dialog::fil_dialog_3;
            return dialog::fil_dialog_2;
        }

        if (next_dialog == dialog::fil_dialog_3) {
            next_dialog = dialog::fil_dialog_4;
            return dialog::fil_dialog_3;
        }

        if (next_dialog == dialog::fil_dialog_4) {
            next_dialog = dialog::fil_dialog_1;
            return dialog::fil_dialog_4;
        }

        next_dialog = dialog::fil_dialog_2;
        return dialog::fil_dialog_1;
    }

    virtual const char *get_name() const override
    {
        return "Vim Diesel";
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
