#pragma once

#include "rpg/Character.h"

namespace rpg::animation
{

static constexpr int mc_moving_down[] = {
    LIBRARY_IMAGE_MC_01, LIBRARY_IMAGE_MC_02, LIBRARY_IMAGE_MC_03,
    LIBRARY_IMAGE_MC_05, LIBRARY_IMAGE_MC_06, LIBRARY_IMAGE_MC_07,
};

static constexpr int mc_moving_left[] = {
    LIBRARY_IMAGE_MC_17, LIBRARY_IMAGE_MC_18, LIBRARY_IMAGE_MC_19,
    LIBRARY_IMAGE_MC_21, LIBRARY_IMAGE_MC_22, LIBRARY_IMAGE_MC_23,
};

static constexpr int mc_moving_right[] = {
    LIBRARY_IMAGE_MC_25, LIBRARY_IMAGE_MC_26, LIBRARY_IMAGE_MC_27,
    LIBRARY_IMAGE_MC_29, LIBRARY_IMAGE_MC_30, LIBRARY_IMAGE_MC_31,
};

static constexpr int mc_moving_up[] = {
    LIBRARY_IMAGE_MC_08, LIBRARY_IMAGE_MC_09, LIBRARY_IMAGE_MC_10,
    LIBRARY_IMAGE_MC_12, LIBRARY_IMAGE_MC_13, LIBRARY_IMAGE_MC_14,
};

static constexpr int mc_standing[] = {LIBRARY_IMAGE_MC_00, LIBRARY_IMAGE_MC_04};

} // namespace rpg::animation

namespace rpg
{

class MainCharacter : public Character
{
  public:
    MainCharacter() : Character{24, 32, 13, 31}
    {
        set_animation_variant(Appearance::moving_down,
                              animation::mc_moving_down, 6);

        set_animation_variant(Appearance::moving_left,
                              animation::mc_moving_left, 6);

        set_animation_variant(Appearance::moving_right,
                              animation::mc_moving_right, 6);

        set_animation_variant(Appearance::moving_up, animation::mc_moving_up,
                              6);

        set_animation_variant(Appearance::standing, animation::mc_standing, 2);
    }

    virtual const char *get_name() const override { return "main-character"; }

    virtual bool move(GlobalCoordinates coordinates) override;
    virtual void render(Viewport &viewport) override;

  private:
    unsigned long long move_time = 0;
};

} // namespace rpg
