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
    "One does not simply\n", "exit vim! I was\n",  "trapped there for\n",
    "20 years, be\n",        "careful out there.", "",
};

} // namespace rpg::dialog

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
        if (next_dialog == dialog::oldman_dialog_1) {
            next_dialog = dialog::oldman_dialog_2;
            return dialog::oldman_dialog_1;
        }

        if (next_dialog == dialog::oldman_dialog_2) {
            next_dialog = dialog::oldman_dialog_1;
            return dialog::oldman_dialog_2;
        }

        next_dialog = dialog::oldman_dialog_2;
        return dialog::oldman_dialog_1;
    }

    virtual const char *get_name() const override
    {
        return "Oldman";
    }

    virtual void render(Viewport &viewport) override;

  private:
    const char **next_dialog;
};

} // namespace rpg
