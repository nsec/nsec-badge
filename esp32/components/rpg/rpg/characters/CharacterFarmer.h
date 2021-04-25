#pragma once

#include "rpg/Character.h"
#include "rpg/characters/MovingMixin.h"

#include "graphics.h"

namespace rpg::animation
{

static constexpr int farmer_moving_down[] = {
    LIBRARY_IMAGE_FARMER_DOWN_1,
    LIBRARY_IMAGE_FARMER_DOWN_2,
};

static constexpr int farmer_moving_left[] = {
    LIBRARY_IMAGE_FARMER_LEFT_1,
    LIBRARY_IMAGE_FARMER_LEFT_2,
    LIBRARY_IMAGE_FARMER_LEFT_1,
    LIBRARY_IMAGE_FARMER_LEFT_3,
};

static constexpr int farmer_moving_right[] = {
    LIBRARY_IMAGE_FARMER_RIGHT_1,
    LIBRARY_IMAGE_FARMER_RIGHT_2,
    LIBRARY_IMAGE_FARMER_RIGHT_1,
    LIBRARY_IMAGE_FARMER_RIGHT_3,
};

static constexpr int farmer_moving_up[] = {
    LIBRARY_IMAGE_FARMER_UP_1,
    LIBRARY_IMAGE_FARMER_UP_2,
};

} // namespace rpg::animation

namespace rpg::dialog
{

static const char *farmer_dialog_1[] = {
    "Welcome! All im-\n",
    "plants and ex-\n",
    "ploits that we grow\n",
    "here are 100%\n",
    "organic and ethically\n",
    "sourced. Have a look.",
    "",
};

static const char *farmer_dialog_2[] = {
    "Solar energy and\n",
    "strong winds help\n",
    "to control the\n",
    "supply.",
    "",
};

static const char *farmer_dialog_3[] = {
    "I feel the pulse of\n",
    "private commercial\n",
    "networks.",
    "",
};

static const char *farmer_dialog_4[] = {
    "We regularly receive\n",
    "fresh cod that is\n",
    "caught in the\n",
    "coastal cove.",
    "",
};

static const char *farmer_dialog_5[] = {
    "You want to exchange\n",
    "your logon? You need\n",
    "to find a proxy to\n",
    "do that.",
    "",
};

} // namespace rpg::dialog

namespace rpg
{

class CharacterFarmer : virtual public Character, public MovingMixin
{
  public:
    CharacterFarmer() : Character(SceneObjectIdentity::GENERIC, 22, 29, 11, 28)
    {
        set_animation_variant(Appearance::moving_down,
                              animation::farmer_moving_down, 2);

        set_animation_variant(Appearance::moving_left,
                              animation::farmer_moving_left, 4);

        set_animation_variant(Appearance::moving_right,
                              animation::farmer_moving_right, 4);

        set_animation_variant(Appearance::moving_up,
                              animation::farmer_moving_up, 2);
    }

    virtual const char **get_dialog() override
    {
        if (next_dialog == dialog::farmer_dialog_1) {
            next_dialog = dialog::farmer_dialog_2;
            return dialog::farmer_dialog_1;
        }

        if (next_dialog == dialog::farmer_dialog_2) {
            next_dialog = dialog::farmer_dialog_3;
            return dialog::farmer_dialog_2;
        }

        if (next_dialog == dialog::farmer_dialog_3) {
            next_dialog = dialog::farmer_dialog_4;
            return dialog::farmer_dialog_3;
        }

        if (next_dialog == dialog::farmer_dialog_4) {
            next_dialog = dialog::farmer_dialog_5;
            return dialog::farmer_dialog_4;
        }

        if (next_dialog == dialog::farmer_dialog_5) {
            next_dialog = dialog::farmer_dialog_1;
            return dialog::farmer_dialog_5;
        }

        next_dialog = dialog::farmer_dialog_2;
        return dialog::farmer_dialog_1;
    }

    virtual const char *get_name() const override
    {
        return "Farmer";
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
