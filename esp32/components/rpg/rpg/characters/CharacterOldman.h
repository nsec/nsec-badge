#pragma once

#include "rpg/Character.h"

#include "graphics.h"

namespace rpg::animation
{

static constexpr int oldman_standing[] = {
    LIBRARY_IMAGE_OLDMAN_1,
    LIBRARY_IMAGE_OLDMAN_2,
};

} // namespace rpg::animation

namespace rpg
{

class CharacterOldman : public Character
{
  public:
    CharacterOldman() : Character(20, 33, 10, 31)
    {
        set_animation_variant(Appearance::standing, animation::oldman_standing,
                              2);
    }

    virtual const char *get_name() const override { return "Oldman"; }

    virtual void render(Viewport &viewport) override;
};

} // namespace rpg
