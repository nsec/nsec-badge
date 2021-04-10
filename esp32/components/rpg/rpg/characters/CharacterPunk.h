#pragma once

#include "rpg/Character.h"
#include "rpg/characters/MovingMixin.h"

#include "graphics.h"

namespace rpg::animation
{

static constexpr int punk_moving_down[] = {
    LIBRARY_IMAGE_PUNK_DOWN_1,
    LIBRARY_IMAGE_PUNK_DOWN_2,
};

static constexpr int punk_moving_left[] = {
    LIBRARY_IMAGE_PUNK_LEFT_1,
    LIBRARY_IMAGE_PUNK_LEFT_2,
    LIBRARY_IMAGE_PUNK_LEFT_3,
    LIBRARY_IMAGE_PUNK_LEFT_2,
};

static constexpr int punk_moving_right[] = {
    LIBRARY_IMAGE_PUNK_RIGHT_1,
    LIBRARY_IMAGE_PUNK_RIGHT_2,
    LIBRARY_IMAGE_PUNK_RIGHT_3,
    LIBRARY_IMAGE_PUNK_RIGHT_2,
};

static constexpr int punk_moving_up[] = {
    LIBRARY_IMAGE_PUNK_UP_1,
    LIBRARY_IMAGE_PUNK_UP_2,
};

} // namespace rpg::animation

namespace rpg::dialog
{

static const char *punk_dialog_1[] = {
    "Are you really\n",
    "CYBER!? PROVE IT\n",
    "\n",
    "\n",
    "\n",
    "\n",
    "\n",
    "FLAG-KLJV490uhkEJF28\n",
    "",
};

} // namespace rgp::dialog

namespace rpg
{

class CharacterPunk : virtual public Character, public MovingMixin
{
  public:
    CharacterPunk() : Character(character::Identity::GENERIC, 22, 30, 11, 28)
    {
        set_animation_variant(Appearance::moving_down,
                              animation::punk_moving_down, 2);

        set_animation_variant(Appearance::moving_left,
                              animation::punk_moving_left, 4);

        set_animation_variant(Appearance::moving_right,
                              animation::punk_moving_right, 4);

        set_animation_variant(Appearance::moving_up, animation::punk_moving_up,
                              2);
    }

    virtual const char **get_dialog() override
    {
        return dialog::punk_dialog_1;
    }

    virtual const char *get_name() const override { return "Punk"; }

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
