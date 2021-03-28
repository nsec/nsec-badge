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

namespace rpg
{

class CharacterFil : virtual public Character, public MovingMixin
{
  public:
    CharacterFil() : Character("Fil", 22, 29, 11, 26)
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
