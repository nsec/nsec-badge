#pragma once

#include "rpg/Character.h"
#include "rpg/characters/MovingMixin.h"

#include "graphics.h"

namespace rpg::animation
{

static constexpr int vijay_moving_down[] = {
    LIBRARY_IMAGE_VIJAY_DOWN_1,
    LIBRARY_IMAGE_VIJAY_DOWN_2,
};

static constexpr int vijay_moving_left[] = {
    LIBRARY_IMAGE_VIJAY_LEFT_1,
    LIBRARY_IMAGE_VIJAY_LEFT_2,
    LIBRARY_IMAGE_VIJAY_LEFT_1,
    LIBRARY_IMAGE_VIJAY_LEFT_3,
};

static constexpr int vijay_moving_right[] = {
    LIBRARY_IMAGE_VIJAY_RIGHT_1,
    LIBRARY_IMAGE_VIJAY_RIGHT_2,
    LIBRARY_IMAGE_VIJAY_RIGHT_1,
    LIBRARY_IMAGE_VIJAY_RIGHT_3,
};

static constexpr int vijay_moving_up[] = {
    LIBRARY_IMAGE_VIJAY_UP_1,
    LIBRARY_IMAGE_VIJAY_UP_2,
};

} // namespace rpg::animation

namespace rpg::dialog
{

static const char *vijay_dialog_1[] = {
    "They say I cannot\n",
    "enter the city.",
    "",
};

static const char *vijay_dialog_2[] = {
    "I did not come all\n",
    "the way here to\n",
    "stand in front\n",
    "of the firewall!",
    "",
};

static const char *vijay_dialog_3[] = {
    "That wall is not\n",
    "even on fire!",
    "",
};

static const char *vijay_dialog_4[] = {
    "Vijay has warez\n",
    "if you have coin.",
    "",
};

static const char *vijay_dialog_5[] = {
    "I'm NOT in!",
    "",
};

static const char *vijay_dialog_6[] = {
    "Every access control\n",
    "must come with a\n",
    "backdoor for times\n",
    "like this.",
    "",
};

} // namespace rpg::dialog

namespace rpg
{

class CharacterVijay : virtual public Character, public MovingMixin
{
  public:
    CharacterVijay() : Character(SceneObjectIdentity::GENERIC, 22, 29, 11, 28)
    {
        set_animation_variant(Appearance::moving_down,
                              animation::vijay_moving_down, 2);

        set_animation_variant(Appearance::moving_left,
                              animation::vijay_moving_left, 4);

        set_animation_variant(Appearance::moving_right,
                              animation::vijay_moving_right, 4);

        set_animation_variant(Appearance::moving_up, animation::vijay_moving_up,
                              2);
    }

    virtual const char **get_dialog() override
    {
        if (next_dialog == dialog::vijay_dialog_1) {
            next_dialog = dialog::vijay_dialog_2;
            return dialog::vijay_dialog_1;
        }

        if (next_dialog == dialog::vijay_dialog_2) {
            next_dialog = dialog::vijay_dialog_3;
            return dialog::vijay_dialog_2;
        }

        if (next_dialog == dialog::vijay_dialog_3) {
            next_dialog = dialog::vijay_dialog_4;
            return dialog::vijay_dialog_3;
        }

        if (next_dialog == dialog::vijay_dialog_4) {
            next_dialog = dialog::vijay_dialog_5;
            return dialog::vijay_dialog_4;
        }

        if (next_dialog == dialog::vijay_dialog_5) {
            next_dialog = dialog::vijay_dialog_6;
            return dialog::vijay_dialog_5;
        }

        if (next_dialog == dialog::vijay_dialog_6) {
            next_dialog = dialog::vijay_dialog_1;
            return dialog::vijay_dialog_6;
        }

        next_dialog = dialog::vijay_dialog_2;
        return dialog::vijay_dialog_1;
    }

    virtual const char *get_name() const override
    {
        return "Vijay";
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
