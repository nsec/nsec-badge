#pragma once

#include "rpg/Character.h"
#include "rpg/characters/MovingMixin.h"

#include "graphics.h"

namespace rpg::animation
{

static constexpr int yue_moving_down[] = {
    LIBRARY_IMAGE_YUE_DOWN_1,
    LIBRARY_IMAGE_YUE_DOWN_2,
};

static constexpr int yue_moving_left[] = {
    LIBRARY_IMAGE_YUE_LEFT_1,
    LIBRARY_IMAGE_YUE_LEFT_2,
    LIBRARY_IMAGE_YUE_LEFT_1,
    LIBRARY_IMAGE_YUE_LEFT_3,
};

static constexpr int yue_moving_right[] = {
    LIBRARY_IMAGE_YUE_RIGHT_1,
    LIBRARY_IMAGE_YUE_RIGHT_2,
    LIBRARY_IMAGE_YUE_RIGHT_1,
    LIBRARY_IMAGE_YUE_RIGHT_3,
};

static constexpr int yue_moving_up[] = {
    LIBRARY_IMAGE_YUE_UP_1,
    LIBRARY_IMAGE_YUE_UP_2,
};

} // namespace rpg::animation

namespace rpg
{

class CharacterYue : virtual public Character, public MovingMixin
{
  public:
    CharacterYue() : Character(character::Identity::GENERIC, 22, 29, 11, 28)
    {
        set_animation_variant(Appearance::moving_down,
                              animation::yue_moving_down, 2);

        set_animation_variant(Appearance::moving_left,
                              animation::yue_moving_left, 4);

        set_animation_variant(Appearance::moving_right,
                              animation::yue_moving_right, 4);

        set_animation_variant(Appearance::moving_up, animation::yue_moving_up,
                              2);
    }

    virtual const char *get_name() const override { return "Yue"; }

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
