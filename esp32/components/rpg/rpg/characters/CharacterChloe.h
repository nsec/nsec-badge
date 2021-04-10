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

namespace rpg
{

class CharacterChloe : virtual public Character, public MovingMixin
{
  public:
    CharacterChloe() : Character(character::Identity::GENERIC, 22, 29, 11, 28)
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
};

} // namespace rpg
