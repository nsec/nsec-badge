#pragma once

#include "rpg/Character.h"
#include "rpg/characters/MovingMixin.h"

#include "graphics.h"

namespace rpg::animation
{

static constexpr int soldier_moving_down[] = {
    LIBRARY_IMAGE_SOLDIER_DOWN_1,
    LIBRARY_IMAGE_SOLDIER_DOWN_2,
};

static constexpr int soldier_moving_left[] = {
    LIBRARY_IMAGE_SOLDIER_LEFT_1,
    LIBRARY_IMAGE_SOLDIER_LEFT_2,
    LIBRARY_IMAGE_SOLDIER_LEFT_1,
    LIBRARY_IMAGE_SOLDIER_LEFT_3,
};

static constexpr int soldier_moving_right[] = {
    LIBRARY_IMAGE_SOLDIER_RIGHT_1,
    LIBRARY_IMAGE_SOLDIER_RIGHT_2,
    LIBRARY_IMAGE_SOLDIER_RIGHT_1,
    LIBRARY_IMAGE_SOLDIER_RIGHT_3,
};

static constexpr int soldier_moving_up[] = {
    LIBRARY_IMAGE_SOLDIER_UP_1,
    LIBRARY_IMAGE_SOLDIER_UP_2,
};

} // namespace rpg::animation

namespace rpg
{

class CharacterSoldier : virtual public Character, public MovingMixin
{
  public:
    CharacterSoldier() : Character(22, 29, 11, 26)
    {
        set_animation_variant(Appearance::moving_down,
                              animation::soldier_moving_down, 2);

        set_animation_variant(Appearance::moving_left,
                              animation::soldier_moving_left, 4);

        set_animation_variant(Appearance::moving_right,
                              animation::soldier_moving_right, 4);

        set_animation_variant(Appearance::moving_up,
                              animation::soldier_moving_up, 2);
    }

    virtual const char *get_name() const override { return "Soldier"; }

    virtual void post_render(Viewport &viewport) override;
    virtual void render(Viewport &viewport) override;

  private:
    enum class Mode {
        standing,
        walking,
    };

    Mode current_mode = Mode::standing;

    void render_standing(Viewport &viewport);
    void render_walking(Viewport &viewport);
};

} // namespace rpg
