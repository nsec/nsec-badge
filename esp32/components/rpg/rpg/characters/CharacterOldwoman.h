#pragma once

#include "rpg/Character.h"
#include "rpg/characters/MovingMixin.h"

#include "graphics.h"

namespace rpg::animation
{

static constexpr int oldwoman_moving_down[] = {
    LIBRARY_IMAGE_OLDWOMAN_DOWN_1,
    LIBRARY_IMAGE_OLDWOMAN_DOWN_2,
};

static constexpr int oldwoman_moving_left[] = {
    LIBRARY_IMAGE_OLDWOMAN_LEFT_1,
    LIBRARY_IMAGE_OLDWOMAN_LEFT_2,
    LIBRARY_IMAGE_OLDWOMAN_LEFT_1,
    LIBRARY_IMAGE_OLDWOMAN_LEFT_3,
};

static constexpr int oldwoman_moving_right[] = {
    LIBRARY_IMAGE_OLDWOMAN_RIGHT_1,
    LIBRARY_IMAGE_OLDWOMAN_RIGHT_2,
    LIBRARY_IMAGE_OLDWOMAN_RIGHT_1,
    LIBRARY_IMAGE_OLDWOMAN_RIGHT_3,
};

static constexpr int oldwoman_moving_up[] = {
    LIBRARY_IMAGE_OLDWOMAN_UP_1,
    LIBRARY_IMAGE_OLDWOMAN_UP_2,
};

} // namespace rpg::animation

namespace rpg
{

class CharacterOldwoman : virtual public Character, public MovingMixin
{
  public:
    CharacterOldwoman()
        : Character(character::Identity::GENERIC, 22, 29, 11, 29)
    {
        set_animation_variant(Appearance::moving_down,
                              animation::oldwoman_moving_down, 2);

        set_animation_variant(Appearance::moving_left,
                              animation::oldwoman_moving_left, 4);

        set_animation_variant(Appearance::moving_right,
                              animation::oldwoman_moving_right, 4);

        set_animation_variant(Appearance::moving_up,
                              animation::oldwoman_moving_up, 2);
    }

    virtual const char *get_name() const override { return "oldwoman"; }

    virtual unsigned int get_animation_step() const override
    {
        return Character::get_animation_step() / 3;
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
