#pragma once

#include "rpg/Character.h"
#include "rpg/characters/MovingMixin.h"

#include "graphics.h"

namespace rpg::animation
{

static constexpr int retailer_moving_left[] = {
    LIBRARY_IMAGE_RETAILER_LEFT_1,
    LIBRARY_IMAGE_RETAILER_LEFT_2,
    LIBRARY_IMAGE_RETAILER_LEFT_1,
    LIBRARY_IMAGE_RETAILER_LEFT_3,
};

static constexpr int retailer_moving_right[] = {
    LIBRARY_IMAGE_RETAILER_RIGHT_1,
    LIBRARY_IMAGE_RETAILER_RIGHT_2,
    LIBRARY_IMAGE_RETAILER_RIGHT_1,
    LIBRARY_IMAGE_RETAILER_RIGHT_3,
};

} // namespace rpg::animation

namespace rpg::dialog
{

static const char *retailer_dialog_1[] = {
    "Stay a while and\n",
    "listen what people\n",
    "are saying.",
    "",
};

static const char *retailer_dialog_2[] = {
    "We only accept Goat\n",
    "Pieces here, but\n",
    "it's not a stable\n",
    "currency.",
    "",
};

static const char *retailer_dialog_3[] = {
    "Many people\n",        "incorrectly call\n", "it GIF, but the\n",
    "correct way to say\n", "it is GIF.",         "",
};

} // namespace rpg::dialog

namespace rpg
{

class CharacterRetailer : virtual public Character, public MovingMixin
{
  public:
    CharacterRetailer()
        : Character(SceneObjectIdentity::GENERIC, 22, 29, 11, 27)
    {
        set_animation_variant(Appearance::moving_left,
                              animation::retailer_moving_left, 4);

        set_animation_variant(Appearance::moving_right,
                              animation::retailer_moving_right, 4);
    }

    virtual const char **get_dialog() override
    {
        if (next_dialog == dialog::retailer_dialog_1) {
            next_dialog = dialog::retailer_dialog_2;
            return dialog::retailer_dialog_1;
        }

        if (next_dialog == dialog::retailer_dialog_2) {
            next_dialog = dialog::retailer_dialog_3;
            return dialog::retailer_dialog_2;
        }

        if (next_dialog == dialog::retailer_dialog_3) {
            next_dialog = dialog::retailer_dialog_1;
            return dialog::retailer_dialog_3;
        }

        next_dialog = dialog::retailer_dialog_2;
        return dialog::retailer_dialog_1;
    }

    virtual const char *get_name() const override
    {
        return "Sveta";
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
