#pragma once

#include "rpg/Character.h"
#include "rpg/characters/MovingMixin.h"

#include "graphics.h"

namespace rpg::animation
{

static constexpr int monk_moving_down[] = {
    LIBRARY_IMAGE_MONK_DOWN_1,
    LIBRARY_IMAGE_MONK_DOWN_2,
};

static constexpr int monk_moving_left[] = {
    LIBRARY_IMAGE_MONK_LEFT_1,
    LIBRARY_IMAGE_MONK_LEFT_2,
    LIBRARY_IMAGE_MONK_LEFT_3,
    LIBRARY_IMAGE_MONK_LEFT_2,
};

static constexpr int monk_moving_right[] = {
    LIBRARY_IMAGE_MONK_RIGHT_1,
    LIBRARY_IMAGE_MONK_RIGHT_2,
    LIBRARY_IMAGE_MONK_RIGHT_3,
    LIBRARY_IMAGE_MONK_RIGHT_2,
};

static constexpr int monk_moving_up[] = {
    LIBRARY_IMAGE_MONK_UP_1,
    LIBRARY_IMAGE_MONK_UP_2,
};

} // namespace rpg::animation

namespace rpg::dialog
{

static const char *monk_dialog_1[] = {
    "Ommmm... join..\n",
    "the.. resistance...\n",
    "..ommm....\n",
    "",
};

static const char *monk_dialog_2[] = {
    "Pray for PHP's\n",
    "safe return...",
    "",
};

static const char *monk_dialog_3[] = {
    "Praise the Sun\n",
    "Microsystems!",
    "",
};

} // namespace rpg::dialog

namespace rpg
{

class CharacterMonk : virtual public Character, public MovingMixin
{
  public:
    CharacterMonk() : Character(SceneObjectIdentity::GENERIC, 22, 29, 11, 27)
    {
        set_animation_variant(Appearance::moving_down,
                              animation::monk_moving_down, 2);

        set_animation_variant(Appearance::moving_left,
                              animation::monk_moving_left, 4);

        set_animation_variant(Appearance::moving_right,
                              animation::monk_moving_right, 4);

        set_animation_variant(Appearance::moving_up, animation::monk_moving_up,
                              2);
    }

    virtual const char **get_dialog() override
    {
        if (next_dialog == dialog::monk_dialog_1) {
            next_dialog = dialog::monk_dialog_2;
            return dialog::monk_dialog_1;
        }

        if (next_dialog == dialog::monk_dialog_2) {
            next_dialog = dialog::monk_dialog_3;
            return dialog::monk_dialog_2;
        }

        if (next_dialog == dialog::monk_dialog_3) {
            next_dialog = dialog::monk_dialog_1;
            return dialog::monk_dialog_3;
        }

        next_dialog = dialog::monk_dialog_2;
        return dialog::monk_dialog_1;
    }

    virtual const char *get_name() const override
    {
        return "Brother Tux";
    }

    virtual void post_render(Viewport &viewport) override;
    virtual void render(Viewport &viewport) override;

  private:
    enum class Mode {
        meditating,
        standing,
        walking,
    };

    Mode current_mode = Mode::standing;
    const char **next_dialog;
};

} // namespace rpg