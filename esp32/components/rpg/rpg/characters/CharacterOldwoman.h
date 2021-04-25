#pragma once

#include "rpg/Character.h"
#include "rpg/characters/MovingMixin.h"

#include "graphics.h"

namespace rpg::animation
{

static constexpr int oldwoman_moving_down[] = {
    LIBRARY_IMAGE_OLDWOMAN_DOWN_1,
    LIBRARY_IMAGE_OLDWOMAN_DOWN_2,
};

static constexpr int oldwoman_moving_left[] = {
    LIBRARY_IMAGE_OLDWOMAN_LEFT_1,
    LIBRARY_IMAGE_OLDWOMAN_LEFT_2,
    LIBRARY_IMAGE_OLDWOMAN_LEFT_1,
    LIBRARY_IMAGE_OLDWOMAN_LEFT_3,
};

static constexpr int oldwoman_moving_right[] = {
    LIBRARY_IMAGE_OLDWOMAN_RIGHT_1,
    LIBRARY_IMAGE_OLDWOMAN_RIGHT_2,
    LIBRARY_IMAGE_OLDWOMAN_RIGHT_1,
    LIBRARY_IMAGE_OLDWOMAN_RIGHT_3,
};

static constexpr int oldwoman_moving_up[] = {
    LIBRARY_IMAGE_OLDWOMAN_UP_1,
    LIBRARY_IMAGE_OLDWOMAN_UP_2,
};

} // namespace rpg::animation

namespace rpg::dialog
{

static const char *oldwoman_dialog_1[] = {
    "Back in my day, we\n",
    "used to punch\n",
    "programs, and when\n",
    "they did not give\n",
    "the correct result,\n",
    "we punched them\n",
    "some more.",
    "",
};

static const char *oldwoman_dialog_2[] = {
    "Nobody needs more\n",
    "than 32Mb of RAM!",
    "",
};

static const char *oldwoman_dialog_3[] = {
    "Just overflow the\n",
    "shellcode on the\n",
    "stack and jump to\n",
    "it.",
    "",
};

} // namespace rpg::dialog

namespace rpg
{

class CharacterOldwoman : virtual public Character, public MovingMixin
{
  public:
    CharacterOldwoman()
        : Character(SceneObjectIdentity::GENERIC, 22, 29, 11, 29)
    {
        set_animation_variant(Appearance::moving_down,
                              animation::oldwoman_moving_down, 2);

        set_animation_variant(Appearance::moving_left,
                              animation::oldwoman_moving_left, 4);

        set_animation_variant(Appearance::moving_right,
                              animation::oldwoman_moving_right, 4);

        set_animation_variant(Appearance::moving_up,
                              animation::oldwoman_moving_up, 2);
    }

    virtual const char **get_dialog() override
    {
        if (next_dialog == dialog::oldwoman_dialog_1) {
            next_dialog = dialog::oldwoman_dialog_2;
            return dialog::oldwoman_dialog_1;
        }

        if (next_dialog == dialog::oldwoman_dialog_2) {
            next_dialog = dialog::oldwoman_dialog_3;
            return dialog::oldwoman_dialog_2;
        }

        if (next_dialog == dialog::oldwoman_dialog_3) {
            next_dialog = dialog::oldwoman_dialog_1;
            return dialog::oldwoman_dialog_3;
        }

        next_dialog = dialog::oldwoman_dialog_2;
        return dialog::oldwoman_dialog_1;
    }

    virtual const char *get_name() const override
    {
        return "Gran-gran";
    }

    virtual unsigned int get_animation_step() const override
    {
        return Character::get_animation_step() / 3;
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
