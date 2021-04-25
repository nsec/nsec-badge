#pragma once

#include "rpg/Character.h"
#include "rpg/characters/MovingMixin.h"

#include "graphics.h"

namespace rpg::animation
{

static constexpr int merchant_moving_down[] = {
    LIBRARY_IMAGE_MERCHANT_DOWN_1,
    LIBRARY_IMAGE_MERCHANT_DOWN_2,
};

static constexpr int merchant_moving_left[] = {
    LIBRARY_IMAGE_MERCHANT_LEFT_1,
    LIBRARY_IMAGE_MERCHANT_LEFT_2,
    LIBRARY_IMAGE_MERCHANT_LEFT_1,
    LIBRARY_IMAGE_MERCHANT_LEFT_3,
};

static constexpr int merchant_moving_right[] = {
    LIBRARY_IMAGE_MERCHANT_RIGHT_1,
    LIBRARY_IMAGE_MERCHANT_RIGHT_2,
    LIBRARY_IMAGE_MERCHANT_RIGHT_1,
    LIBRARY_IMAGE_MERCHANT_RIGHT_3,
};

static constexpr int merchant_moving_up[] = {
    LIBRARY_IMAGE_MERCHANT_UP_1,
    LIBRARY_IMAGE_MERCHANT_UP_2,
};

} // namespace rpg::animation

namespace rpg::dialog
{

static const char *merchant_dialog_1[] = {
    "Hmm.",
    "",
};

static const char *merchant_dialog_2[] = {
    "Hmm??",
    "",
};

static const char *merchant_dialog_3[] = {
    "Hhmmm...",
    "",
};

static const char *merchant_dialog_4[] = {
    "\n\n",
    "* Hmm intensifies *",
    "",
};

} // namespace rpg::dialog

namespace rpg
{

class CharacterMerchant : virtual public Character, public MovingMixin
{
  public:
    CharacterMerchant()
        : Character(SceneObjectIdentity::GENERIC, 22, 29, 11, 27)
    {
        set_animation_variant(Appearance::moving_down,
                              animation::merchant_moving_down, 2);

        set_animation_variant(Appearance::moving_left,
                              animation::merchant_moving_left, 4);

        set_animation_variant(Appearance::moving_right,
                              animation::merchant_moving_right, 4);

        set_animation_variant(Appearance::moving_up,
                              animation::merchant_moving_up, 2);
    }

    virtual const char **get_dialog() override
    {
        if (next_dialog == dialog::merchant_dialog_1) {
            next_dialog = dialog::merchant_dialog_2;
            return dialog::merchant_dialog_1;
        }

        if (next_dialog == dialog::merchant_dialog_2) {
            next_dialog = dialog::merchant_dialog_3;
            return dialog::merchant_dialog_2;
        }

        if (next_dialog == dialog::merchant_dialog_3) {
            next_dialog = dialog::merchant_dialog_4;
            return dialog::merchant_dialog_3;
        }

        if (next_dialog == dialog::merchant_dialog_4) {
            next_dialog = dialog::merchant_dialog_1;
            return dialog::merchant_dialog_4;
        }

        next_dialog = dialog::merchant_dialog_2;
        return dialog::merchant_dialog_1;
    }

    virtual const char *get_name() const override
    {
        return "Faisal";
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
