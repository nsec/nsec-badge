#pragma once

#include "rpg/Character.h"
#include "rpg/characters/MovingMixin.h"

#include "graphics.h"

namespace rpg::animation
{

static constexpr int merchant_moving_down[] = {
    LIBRARY_IMAGE_MERCHANT_DOWN_1,
    LIBRARY_IMAGE_MERCHANT_DOWN_2,
};

static constexpr int merchant_moving_left[] = {
    LIBRARY_IMAGE_MERCHANT_LEFT_1,
    LIBRARY_IMAGE_MERCHANT_LEFT_2,
    LIBRARY_IMAGE_MERCHANT_LEFT_1,
    LIBRARY_IMAGE_MERCHANT_LEFT_3,
};

static constexpr int merchant_moving_right[] = {
    LIBRARY_IMAGE_MERCHANT_RIGHT_1,
    LIBRARY_IMAGE_MERCHANT_RIGHT_2,
    LIBRARY_IMAGE_MERCHANT_RIGHT_1,
    LIBRARY_IMAGE_MERCHANT_RIGHT_3,
};

static constexpr int merchant_moving_up[] = {
    LIBRARY_IMAGE_MERCHANT_UP_1,
    LIBRARY_IMAGE_MERCHANT_UP_2,
};

} // namespace rpg::animation

namespace rpg
{

class CharacterMerchant : virtual public Character, public MovingMixin
{
  public:
    CharacterMerchant()
        : Character(character::Identity::GENERIC, 22, 29, 11, 27)
    {
        set_animation_variant(Appearance::moving_down,
                              animation::merchant_moving_down, 2);

        set_animation_variant(Appearance::moving_left,
                              animation::merchant_moving_left, 4);

        set_animation_variant(Appearance::moving_right,
                              animation::merchant_moving_right, 4);

        set_animation_variant(Appearance::moving_up,
                              animation::merchant_moving_up, 2);
    }

    virtual const char *get_name() const override
    {
        return "Merchant";
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
