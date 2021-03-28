#pragma once

#include "rpg/Character.h"
#include "rpg/characters/MovingMixin.h"

#include "graphics.h"

namespace rpg::animation
{

static constexpr int retailer_moving_left[] = {
    LIBRARY_IMAGE_RETAILER_LEFT_1,
    LIBRARY_IMAGE_RETAILER_LEFT_2,
    LIBRARY_IMAGE_RETAILER_LEFT_1,
    LIBRARY_IMAGE_RETAILER_LEFT_3,
};

static constexpr int retailer_moving_right[] = {
    LIBRARY_IMAGE_RETAILER_RIGHT_1,
    LIBRARY_IMAGE_RETAILER_RIGHT_2,
    LIBRARY_IMAGE_RETAILER_RIGHT_1,
    LIBRARY_IMAGE_RETAILER_RIGHT_3,
};

} // namespace rpg::animation

namespace rpg
{

class CharacterRetailer : virtual public Character, public MovingMixin
{
  public:
    CharacterRetailer() : Character(22, 29, 11, 27)
    {
        set_animation_variant(Appearance::moving_left,
                              animation::retailer_moving_left, 4);

        set_animation_variant(Appearance::moving_right,
                              animation::retailer_moving_right, 4);
    }

    virtual const char *get_name() const override { return "retailer"; }

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
