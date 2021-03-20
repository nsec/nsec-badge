#pragma once

#include "rpg/Character.h"

#include "graphics.h"

namespace rpg::animation
{

static constexpr int fisherman_standing[] = {
    LIBRARY_IMAGE_FISHERMAN_1, LIBRARY_IMAGE_FISHERMAN_1,
    LIBRARY_IMAGE_FISHERMAN_1, LIBRARY_IMAGE_FISHERMAN_1,
    LIBRARY_IMAGE_FISHERMAN_2, LIBRARY_IMAGE_FISHERMAN_3,
    LIBRARY_IMAGE_FISHERMAN_1, LIBRARY_IMAGE_FISHERMAN_4,
};

} // namespace rpg::animation

namespace rpg
{

class CharacterFisherman : public Character
{
  public:
    CharacterFisherman() : Character("fisherman", 56, 27, 9, 24)
    {
        set_animation_variant(Appearance::standing,
                              animation::fisherman_standing, 8);
    }

    virtual void render(Viewport &viewport) override;
};

} // namespace rpg
