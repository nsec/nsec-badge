#pragma once

#include "rpg/Character.h"
#include "rpg/characters/MovingMixin.h"

#include "graphics.h"

namespace rpg::dialog
{

static const char *duck_dialog_1[] = {
    "... quack.",
    "",
};

} // namespace rgp::dialog

namespace rpg
{

class CharacterDuck : virtual public Character, public MovingMixin
{
  public:
    CharacterDuck() : Character(character::Identity::duck, 23, 15, 12, 13)
    {
        set_moving_direction(direction::right);
    }

    static bool identify(Character *character_p)
    {
        return character_p->get_identity() == character::Identity::duck;
    }

    virtual const char **get_dialog() override
    {
        return dialog::duck_dialog_1;
    }

    virtual const char *get_name() const override {
        return "Duck";
    }

    virtual unsigned int get_animation_step() const override
    {
        return Character::get_animation_step() / 2;
    }

    virtual void post_render(Viewport &viewport) override;
    virtual void render(Viewport &viewport) override;
};

} // namespace rpg
