#pragma once

#include "rpg/Character.h"

#include "graphics.h"

namespace rpg::animation
{

static constexpr int archer_standing[] = {
    LIBRARY_IMAGE_ARCHER_1, LIBRARY_IMAGE_ARCHER_1, LIBRARY_IMAGE_ARCHER_1,
    LIBRARY_IMAGE_ARCHER_1, LIBRARY_IMAGE_ARCHER_1, LIBRARY_IMAGE_ARCHER_1,
    LIBRARY_IMAGE_ARCHER_1, LIBRARY_IMAGE_ARCHER_1, LIBRARY_IMAGE_ARCHER_1,
    LIBRARY_IMAGE_ARCHER_1, LIBRARY_IMAGE_ARCHER_1, LIBRARY_IMAGE_ARCHER_1,
    LIBRARY_IMAGE_ARCHER_1, LIBRARY_IMAGE_ARCHER_1, LIBRARY_IMAGE_ARCHER_1,
    LIBRARY_IMAGE_ARCHER_1, LIBRARY_IMAGE_ARCHER_1, LIBRARY_IMAGE_ARCHER_1,
    LIBRARY_IMAGE_ARCHER_1, LIBRARY_IMAGE_ARCHER_1, LIBRARY_IMAGE_ARCHER_1,
    LIBRARY_IMAGE_ARCHER_1, LIBRARY_IMAGE_ARCHER_1, LIBRARY_IMAGE_ARCHER_1,
    LIBRARY_IMAGE_ARCHER_1, LIBRARY_IMAGE_ARCHER_1, LIBRARY_IMAGE_ARCHER_1,
    LIBRARY_IMAGE_ARCHER_1, LIBRARY_IMAGE_ARCHER_1, LIBRARY_IMAGE_ARCHER_1,
    LIBRARY_IMAGE_ARCHER_1, LIBRARY_IMAGE_ARCHER_1, LIBRARY_IMAGE_ARCHER_1,
    LIBRARY_IMAGE_ARCHER_1, LIBRARY_IMAGE_ARCHER_1, LIBRARY_IMAGE_ARCHER_2,
    LIBRARY_IMAGE_ARCHER_3, LIBRARY_IMAGE_ARCHER_3, LIBRARY_IMAGE_ARCHER_3,
    LIBRARY_IMAGE_ARCHER_4, LIBRARY_IMAGE_ARCHER_5, LIBRARY_IMAGE_ARCHER_5,
    LIBRARY_IMAGE_ARCHER_5, LIBRARY_IMAGE_ARCHER_5, LIBRARY_IMAGE_ARCHER_5,
    LIBRARY_IMAGE_ARCHER_5, LIBRARY_IMAGE_ARCHER_5, LIBRARY_IMAGE_ARCHER_5,
    LIBRARY_IMAGE_ARCHER_6, LIBRARY_IMAGE_ARCHER_6, LIBRARY_IMAGE_ARCHER_6,
    LIBRARY_IMAGE_ARCHER_6, LIBRARY_IMAGE_ARCHER_6, LIBRARY_IMAGE_ARCHER_6,
    LIBRARY_IMAGE_ARCHER_6, LIBRARY_IMAGE_ARCHER_6, LIBRARY_IMAGE_ARCHER_6,
    LIBRARY_IMAGE_ARCHER_6, LIBRARY_IMAGE_ARCHER_6, LIBRARY_IMAGE_ARCHER_6,
};

} // namespace rpg::animation

namespace rpg
{

class CharacterArcher : public Character
{
  public:
    CharacterArcher() : Character("archer", 33, 36, 17, 34)
    {
        set_animation_variant(Appearance::standing, animation::archer_standing,
                              60);
    }

    virtual void render(Viewport &viewport) override;
};

} // namespace rpg