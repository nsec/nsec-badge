#pragma once

#include "rpg/Character.h"
#include "rpg/characters/MovingMixin.h"

#include "graphics.h"

namespace rpg::animation
{

static constexpr int red_moving_down[] = {
    LIBRARY_IMAGE_RED_DOWN_1,
    LIBRARY_IMAGE_RED_DOWN_2,
};

static constexpr int red_moving_left[] = {
    LIBRARY_IMAGE_RED_LEFT_1,
    LIBRARY_IMAGE_RED_LEFT_2,
    LIBRARY_IMAGE_RED_LEFT_1,
    LIBRARY_IMAGE_RED_LEFT_3,
};

static constexpr int red_moving_right[] = {
    LIBRARY_IMAGE_RED_RIGHT_1,
    LIBRARY_IMAGE_RED_RIGHT_2,
    LIBRARY_IMAGE_RED_RIGHT_1,
    LIBRARY_IMAGE_RED_RIGHT_3,
};

static constexpr int red_moving_up[] = {
    LIBRARY_IMAGE_RED_UP_1,
    LIBRARY_IMAGE_RED_UP_2,
};

} // namespace rpg::animation

namespace rpg
{

class CharacterRed : virtual public Character, public MovingMixin
{
  public:
    CharacterRed() : Character(character::Identity::GENERIC, 22, 29, 11, 28)
    {
        set_animation_variant(Appearance::moving_down,
                              animation::red_moving_down, 2);

        set_animation_variant(Appearance::moving_left,
                              animation::red_moving_left, 4);

        set_animation_variant(Appearance::moving_right,
                              animation::red_moving_right, 4);

        set_animation_variant(Appearance::moving_up, animation::red_moving_up,
                              2);
    }

    virtual const char *get_name() const override
    {
        return "Rouge";
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
