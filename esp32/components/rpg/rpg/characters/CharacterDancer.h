#pragma once

#include "rpg/Character.h"
#include "rpg/characters/MovingMixin.h"

namespace rpg::animation
{

static constexpr int dancer_dancing[] = {
    LIBRARY_IMAGE_DANCER_DANCING_1, LIBRARY_IMAGE_DANCER_DANCING_2,
    LIBRARY_IMAGE_DANCER_DANCING_1, LIBRARY_IMAGE_DANCER_DANCING_2,
    LIBRARY_IMAGE_DANCER_DANCING_1, LIBRARY_IMAGE_DANCER_DANCING_2,
    LIBRARY_IMAGE_DANCER_DANCING_3, LIBRARY_IMAGE_DANCER_DANCING_2,
    LIBRARY_IMAGE_DANCER_DANCING_3, LIBRARY_IMAGE_DANCER_DANCING_2,
    LIBRARY_IMAGE_DANCER_DANCING_3, LIBRARY_IMAGE_DANCER_DANCING_4,
    LIBRARY_IMAGE_DANCER_DANCING_3, LIBRARY_IMAGE_DANCER_DANCING_4,
    LIBRARY_IMAGE_DANCER_DANCING_3, LIBRARY_IMAGE_DANCER_DANCING_2,
    LIBRARY_IMAGE_DANCER_DANCING_1,
};

static constexpr int dancer_moving_down[] = {
    LIBRARY_IMAGE_DANCER_DOWN_1,
    LIBRARY_IMAGE_DANCER_DOWN_2,
};

static constexpr int dancer_moving_left[] = {
    LIBRARY_IMAGE_DANCER_LEFT_1,
    LIBRARY_IMAGE_DANCER_LEFT_2,
    LIBRARY_IMAGE_DANCER_LEFT_3,
    LIBRARY_IMAGE_DANCER_LEFT_2,
};

static constexpr int dancer_moving_right[] = {
    LIBRARY_IMAGE_DANCER_RIGHT_1,
    LIBRARY_IMAGE_DANCER_RIGHT_2,
    LIBRARY_IMAGE_DANCER_RIGHT_3,
    LIBRARY_IMAGE_DANCER_RIGHT_2,
};

static constexpr int dancer_moving_up[] = {
    LIBRARY_IMAGE_DANCER_UP_1,
    LIBRARY_IMAGE_DANCER_UP_2,
};

} // namespace rpg::animation

namespace rpg::dialog
{

static const char *dancer_dialog_1[] = {
    "Where did the\n", "hacker go?\n",   "\n",
    "I don't know,\n", "he ransomware.", "",
};

} // namespace rpg::dialog

namespace rpg
{

class CharacterDancer : virtual public Character, public MovingMixin
{
  public:
    CharacterDancer() : Character(character::Identity::GENERIC, 30, 29, 14, 27)
    {
        set_animation_variant(Appearance::action_a, animation::dancer_dancing,
                              17);

        set_animation_variant(Appearance::moving_down,
                              animation::dancer_moving_down, 2);

        set_animation_variant(Appearance::moving_left,
                              animation::dancer_moving_left, 4);

        set_animation_variant(Appearance::moving_right,
                              animation::dancer_moving_right, 4);

        set_animation_variant(Appearance::moving_up,
                              animation::dancer_moving_up, 2);
    }

    virtual const char **get_dialog() override
    {
        return dialog::dancer_dialog_1;
    }

    virtual const char *get_name() const override
    {
        return "dancer";
    }

    virtual void post_render(Viewport &viewport) override;

    virtual void render(Viewport &viewport) override;

  private:
    enum class Mode {
        standing,
        dancing,
        walking,
    };

    Mode current_mode = Mode::dancing;

    void render_dancing(Viewport &viewport);
    void render_standing(Viewport &viewport);
    void render_walking(Viewport &viewport);
};

} // namespace rpg
