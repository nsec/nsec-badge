#pragma once

#include "rpg/Character.h"

#include "graphics.h"

namespace rpg::animation
{

static constexpr int charles_standing[] = {
    LIBRARY_IMAGE_CHARLES_1,
    LIBRARY_IMAGE_CHARLES_2,
};

} // namespace rpg::animation

namespace rpg::dialog
{

static const char *charles_dialog_1[] = {
    "I like to wear\n",
    "trousers. They're\n",
    "comfy and easy\n"
    "to wear.",
    "",
};

static const char *charles_dialog_2[] = {
    "I used to be an\n",
    "adventurer like you,\n",
    "then I took an SQL\n",
    "injection in the\n",
    "knee.\n",
    "",
};

static const char *charles_dialog_3[] = {
    "What is a man?\n\n",
    "A miserable little\n",
    "pile of secrets.",
    "",
};

} // namespace rpg::dialog

namespace rpg
{

class CharacterCharles : public Character
{
  public:
    CharacterCharles() : Character(SceneObjectIdentity::GENERIC, 15, 34, 8, 33)
    {
        set_animation_variant(Appearance::standing, animation::charles_standing,
                              2);
    }

    virtual const char **get_dialog() override
    {
        if (next_dialog == dialog::charles_dialog_1) {
            next_dialog = dialog::charles_dialog_2;
            return dialog::charles_dialog_1;
        }

        if (next_dialog == dialog::charles_dialog_2) {
            next_dialog = dialog::charles_dialog_3;
            return dialog::charles_dialog_2;
        }

        if (next_dialog == dialog::charles_dialog_3) {
            next_dialog = dialog::charles_dialog_1;
            return dialog::charles_dialog_3;
        }

        next_dialog = dialog::charles_dialog_2;
        return dialog::charles_dialog_1;
    }

    virtual const char *get_name() const override
    {
        return "Charles";
    }

    virtual void render(Viewport &viewport) override;

  private:
    const char **next_dialog;
};

} // namespace rpg
