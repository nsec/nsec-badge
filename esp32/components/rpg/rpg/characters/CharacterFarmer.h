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

namespace rpg
{

class CharacterFarmer : virtual public Character, public MovingMixin
{
  public:
    CharacterFarmer() : Character(character::Identity::GENERIC, 22, 29, 11, 28)
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

    virtual const char *get_name() const override { return "farmer"; }

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
