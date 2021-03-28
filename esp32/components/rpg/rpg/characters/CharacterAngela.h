#pragma once

#include "rpg/Character.h"
#include "rpg/characters/MovingMixin.h"

#include "graphics.h"

namespace rpg::animation
{

static constexpr int angela_moving_down[] = {
    LIBRARY_IMAGE_ANGELA_DOWN_1,
    LIBRARY_IMAGE_ANGELA_DOWN_2,
};

static constexpr int angela_moving_left[] = {
    LIBRARY_IMAGE_ANGELA_LEFT_1,
    LIBRARY_IMAGE_ANGELA_LEFT_2,
    LIBRARY_IMAGE_ANGELA_LEFT_1,
    LIBRARY_IMAGE_ANGELA_LEFT_3,
};

static constexpr int angela_moving_right[] = {
    LIBRARY_IMAGE_ANGELA_RIGHT_1,
    LIBRARY_IMAGE_ANGELA_RIGHT_2,
    LIBRARY_IMAGE_ANGELA_RIGHT_1,
    LIBRARY_IMAGE_ANGELA_RIGHT_3,
};

static constexpr int angela_moving_up[] = {
    LIBRARY_IMAGE_ANGELA_UP_1,
    LIBRARY_IMAGE_ANGELA_UP_2,
};

} // namespace rpg::animation

namespace rpg
{

class CharacterAngela : virtual public Character, public MovingMixin
{
  public:
    CharacterAngela() : Character("Angela", 22, 29, 11, 28)
    {
        set_animation_variant(Appearance::moving_down,
                              animation::angela_moving_down, 2);

        set_animation_variant(Appearance::moving_left,
                              animation::angela_moving_left, 4);

        set_animation_variant(Appearance::moving_right,
                              animation::angela_moving_right, 4);

        set_animation_variant(Appearance::moving_up,
                              animation::angela_moving_up, 2);
    }

    virtual void post_render(Viewport &viewport) override;
    virtual void render(Viewport &viewport) override;

  private:
    enum class Mode {
        standing,
        walking,
    };

    Mode current_mode = Mode::standing;

    void render_standing(Viewport &viewport);
    void render_walking(Viewport &viewport);
};

} // namespace rpg
