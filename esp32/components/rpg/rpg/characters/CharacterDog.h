#pragma once

#include "rpg/Character.h"
#include "rpg/characters/MovingMixin.h"

#include "graphics.h"

namespace rpg::animation
{

static constexpr int dog_moving_down[] = {
    LIBRARY_IMAGE_DOG_DOWN_1,
    LIBRARY_IMAGE_DOG_DOWN_2,
};

static constexpr int dog_moving_left[] = {
    LIBRARY_IMAGE_DOG_LEFT_1,
    LIBRARY_IMAGE_DOG_LEFT_2,
};

static constexpr int dog_moving_right[] = {
    LIBRARY_IMAGE_DOG_RIGHT_1,
    LIBRARY_IMAGE_DOG_RIGHT_2,
};

static constexpr int dog_moving_up[] = {
    LIBRARY_IMAGE_DOG_UP_1,
    LIBRARY_IMAGE_DOG_UP_2,
};

} // namespace rpg::animation

namespace rpg
{

class CharacterDog : virtual public Character, public MovingMixin
{
  public:
    CharacterDog() : Character(27, 27, 13, 25)
    {
        set_animation_variant(Appearance::moving_down,
                              animation::dog_moving_down, 2);

        set_animation_variant(Appearance::moving_left,
                              animation::dog_moving_left, 2);

        set_animation_variant(Appearance::moving_right,
                              animation::dog_moving_right, 2);

        set_animation_variant(Appearance::moving_up, animation::dog_moving_up,
                              2);
    }

    virtual const char *get_name() const override { return "dog"; }

    virtual unsigned int get_animation_step() const override
    {
        return Character::get_animation_step() * 3;
    }

    virtual void post_render(Viewport &viewport) override;
    virtual void render(Viewport &viewport) override;
};

} // namespace rpg
