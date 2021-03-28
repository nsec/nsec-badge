#pragma once

#include "rpg/Character.h"
#include "rpg/characters/MovingMixin.h"

#include "graphics.h"

namespace rpg
{

class CharacterDuck : virtual public Character, public MovingMixin
{
  public:
    CharacterDuck() : Character("duck", 23, 15, 12, 13)
    {
        set_moving_direction(direction::right);
    }

    virtual unsigned int get_animation_step() const override
    {
        return Character::get_animation_step() / 2;
    }

    virtual void post_render(Viewport &viewport) override;
    virtual void render(Viewport &viewport) override;
};

} // namespace rpg
