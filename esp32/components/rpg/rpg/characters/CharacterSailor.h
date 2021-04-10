#pragma once

#include "rpg/Character.h"
#include "rpg/characters/MovingMixin.h"

#include "graphics.h"

namespace rpg::animation
{

static constexpr int sailor_moving_down[] = {
    LIBRARY_IMAGE_SAILOR_DOWN_1,
    LIBRARY_IMAGE_SAILOR_DOWN_2,
};

static constexpr int sailor_moving_left[] = {
    LIBRARY_IMAGE_SAILOR_LEFT_1,
    LIBRARY_IMAGE_SAILOR_LEFT_2,
    LIBRARY_IMAGE_SAILOR_LEFT_1,
    LIBRARY_IMAGE_SAILOR_LEFT_3,
};

static constexpr int sailor_moving_right[] = {
    LIBRARY_IMAGE_SAILOR_RIGHT_1,
    LIBRARY_IMAGE_SAILOR_RIGHT_2,
    LIBRARY_IMAGE_SAILOR_RIGHT_1,
    LIBRARY_IMAGE_SAILOR_RIGHT_3,
};

static constexpr int sailor_moving_up[] = {
    LIBRARY_IMAGE_SAILOR_UP_1,
    LIBRARY_IMAGE_SAILOR_UP_2,
};

} // namespace rpg::animation

namespace rpg::dialog
{

static const char *sailor_dialog_1[] = {
    "Data lake straight\n",
    "ahead, filled with\n",
    "PII fishes.",
    "",
};

} // namespace rpg::dialog

namespace rpg
{

class CharacterSailor : virtual public Character, public MovingMixin
{
  public:
    CharacterSailor() : Character(character::Identity::GENERIC, 22, 29, 11, 27)
    {
        set_animation_variant(Appearance::moving_down,
                              animation::sailor_moving_down, 2);

        set_animation_variant(Appearance::moving_left,
                              animation::sailor_moving_left, 4);

        set_animation_variant(Appearance::moving_right,
                              animation::sailor_moving_right, 4);

        set_animation_variant(Appearance::moving_up,
                              animation::sailor_moving_up, 2);
    }

    virtual const char **get_dialog() override
    {
        return dialog::sailor_dialog_1;
    }

    virtual const char *get_name() const override
    {
        return "sailor";
    }

    virtual void post_render(Viewport &viewport) override;
    virtual void render(Viewport &viewport) override;

  private:
    enum class Mode {
        standing,
        walking,
    };

    Mode current_mode = Mode::standing;
};

} // namespace rpg
