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

namespace rpg::dialog
{

static const char *oldman_dialog_1[] = {
    "Thou art most\n",
    "welcome to\n",
    "North Sectoria.\n",
    "",
};

static const char *oldman_dialog_2[] = {
    "One does not simply\n",
    "exit vim! I was\n",
    "trapped there for\n",
    "20 years, be\n",
    "careful out there.",
    "",
};

} // namespace rgp::dialog

namespace rpg
{

class CharacterOldman : public Character
{
  public:
    CharacterOldman() : Character(character::Identity::GENERIC, 20, 33, 10, 31)
    {
        set_animation_variant(Appearance::standing, animation::oldman_standing,
                              2);
    }

    virtual const char **get_dialog() override
    {
        if (first_dialog) {
            first_dialog = false;
            return dialog::oldman_dialog_1;
        }

        switch (get_animation_step() % 2) {
        case 0:
            return dialog::oldman_dialog_1;

        case 1:
        default:
            return dialog::oldman_dialog_2;
        }
    }

    virtual const char *get_name() const override { return "Oldman"; }

    virtual void render(Viewport &viewport) override;

  private:
    bool first_dialog = true;
};

} // namespace rpg
