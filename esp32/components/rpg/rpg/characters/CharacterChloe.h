#pragma once

#include "rpg/Character.h"
#include "rpg/characters/MovingMixin.h"

#include "graphics.h"

namespace rpg::animation
{

static constexpr int chloe_moving_down[] = {
    LIBRARY_IMAGE_CHLOE_DOWN_1,
    LIBRARY_IMAGE_CHLOE_DOWN_2,
};

static constexpr int chloe_moving_left[] = {
    LIBRARY_IMAGE_CHLOE_LEFT_1,
    LIBRARY_IMAGE_CHLOE_LEFT_2,
    LIBRARY_IMAGE_CHLOE_LEFT_1,
    LIBRARY_IMAGE_CHLOE_LEFT_3,
};

static constexpr int chloe_moving_right[] = {
    LIBRARY_IMAGE_CHLOE_RIGHT_1,
    LIBRARY_IMAGE_CHLOE_RIGHT_2,
    LIBRARY_IMAGE_CHLOE_RIGHT_1,
    LIBRARY_IMAGE_CHLOE_RIGHT_3,
};

static constexpr int chloe_moving_up[] = {
    LIBRARY_IMAGE_CHLOE_UP_1,
    LIBRARY_IMAGE_CHLOE_UP_2,
};

} // namespace rpg::animation

namespace rpg::dialog
{

static const char *chloe_dialog_1[] = {
    "Have you seen Cloud?\n",
    "I heard he is on\n",
    "someone else's badge.",
    "",
};

static const char *chloe_dialog_2[] = {
    "No matter how dark\n",
    "the night, morning\n",
    "always comes, and\n",
    "our journey begins\n",
    "anew.",
    "",
};

static const char *chloe_dialog_3[] = {
    "They say the User\n",
    "lives outside the\n",
    "badge. No one knows\n",
    "for sure...",
    "",
};

} // namespace rpg::dialog

namespace rpg
{

class CharacterChloe : virtual public Character, public MovingMixin
{
  public:
    CharacterChloe() : Character(SceneObjectIdentity::GENERIC, 22, 29, 11, 28)
    {
        set_animation_variant(Appearance::moving_down,
                              animation::chloe_moving_down, 2);

        set_animation_variant(Appearance::moving_left,
                              animation::chloe_moving_left, 4);

        set_animation_variant(Appearance::moving_right,
                              animation::chloe_moving_right, 4);

        set_animation_variant(Appearance::moving_up, animation::chloe_moving_up,
                              2);
    }

    virtual const char **get_dialog() override
    {
        if (next_dialog == dialog::chloe_dialog_1) {
            next_dialog = dialog::chloe_dialog_2;
            return dialog::chloe_dialog_1;
        }

        if (next_dialog == dialog::chloe_dialog_2) {
            next_dialog = dialog::chloe_dialog_3;
            return dialog::chloe_dialog_2;
        }

        if (next_dialog == dialog::chloe_dialog_3) {
            next_dialog = dialog::chloe_dialog_1;
            return dialog::chloe_dialog_3;
        }

        next_dialog = dialog::chloe_dialog_2;
        return dialog::chloe_dialog_1;
    }

    virtual const char *get_name() const override
    {
        return "Chloe";
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
