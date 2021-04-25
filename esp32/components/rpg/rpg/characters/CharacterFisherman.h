#pragma once

#include "rpg/Character.h"

#include "graphics.h"

namespace rpg::animation
{

static constexpr int fisherman_standing[] = {
    LIBRARY_IMAGE_FISHERMAN_1, LIBRARY_IMAGE_FISHERMAN_1,
    LIBRARY_IMAGE_FISHERMAN_1, LIBRARY_IMAGE_FISHERMAN_1,
    LIBRARY_IMAGE_FISHERMAN_2, LIBRARY_IMAGE_FISHERMAN_3,
    LIBRARY_IMAGE_FISHERMAN_1, LIBRARY_IMAGE_FISHERMAN_4,
};

} // namespace rpg::animation

namespace rpg::dialog
{

static const char *fisherman_dialog_1[] = {
    "I heard you were\n", "coming.\n", "\n", "Want to go phishing?\n", "",
};

static const char *fisherman_dialog_2[] = {
    "Do you think there\n",
    "is more fish on\n",
    "the oposite shore?\n",
    "",
};

static const char *fisherman_dialog_3[] = {
    "I got the Babel\n",
    "Fish.",
    "",
};

} // namespace rpg::dialog

namespace rpg
{

class CharacterFisherman : public Character
{
  public:
    CharacterFisherman()
        : Character(SceneObjectIdentity::GENERIC, 56, 27, 9, 24)
    {
        set_animation_variant(Appearance::standing,
                              animation::fisherman_standing, 8);
    }

    virtual const char **get_dialog() override
    {
        if (next_dialog == dialog::fisherman_dialog_1) {
            next_dialog = dialog::fisherman_dialog_2;
            return dialog::fisherman_dialog_1;
        }

        if (next_dialog == dialog::fisherman_dialog_2) {
            next_dialog = dialog::fisherman_dialog_3;
            return dialog::fisherman_dialog_2;
        }

        if (next_dialog == dialog::fisherman_dialog_3) {
            next_dialog = dialog::fisherman_dialog_1;
            return dialog::fisherman_dialog_3;
        }

        next_dialog = dialog::fisherman_dialog_2;
        return dialog::fisherman_dialog_1;
    }

    virtual const char *get_name() const override
    {
        return "Fisher";
    }

    virtual void render(Viewport &viewport) override;

  private:
    const char **next_dialog;
};

} // namespace rpg
