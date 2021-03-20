#pragma once

#include "rpg/Character.h"

#include "graphics.h"

namespace rpg::animation
{

static constexpr int charles_standing[] = {
    LIBRARY_IMAGE_CHARLES_1,
    LIBRARY_IMAGE_CHARLES_2,
};

} // namespace rpg::animation

namespace rpg
{

class CharacterCharles : public Character
{
  public:
    CharacterCharles() : Character("Charles", 15, 34, 8, 33)
    {
        set_animation_variant(Appearance::standing, animation::charles_standing,
                              2);
    }

    virtual void render(Viewport &viewport) override;
};

} // namespace rpg
