#pragma once

#include "rpg/Character.h"
#include "rpg/characters/MovingMixin.h"

#include "graphics.h"

namespace rpg::animation
{

static constexpr int yue_moving_down[] = {
    LIBRARY_IMAGE_YUE_DOWN_1,
    LIBRARY_IMAGE_YUE_DOWN_2,
};

static constexpr int yue_moving_left[] = {
    LIBRARY_IMAGE_YUE_LEFT_1,
    LIBRARY_IMAGE_YUE_LEFT_2,
    LIBRARY_IMAGE_YUE_LEFT_1,
    LIBRARY_IMAGE_YUE_LEFT_3,
};

static constexpr int yue_moving_right[] = {
    LIBRARY_IMAGE_YUE_RIGHT_1,
    LIBRARY_IMAGE_YUE_RIGHT_2,
    LIBRARY_IMAGE_YUE_RIGHT_1,
    LIBRARY_IMAGE_YUE_RIGHT_3,
};

static constexpr int yue_moving_up[] = {
    LIBRARY_IMAGE_YUE_UP_1,
    LIBRARY_IMAGE_YUE_UP_2,
};

} // namespace rpg::animation

namespace rpg::dialog
{

static const char *yue_dialog_1[] = {
    "I love cats! They\n",
    "are such adorable\n",
    "fluff balls.",
    "",
};

static const char *yue_dialog_2[] = {
    "Did you know: cats\n",
    "can jump up to 6\n",
    "times their length.",
    "",
};

static const char *yue_dialog_3[] = {
    "Cats use their\n",
    "whiskers to feel\n",
    "the world around\n",
    "them to determine\n",
    "which small spaces\n",
    "they can fit into.",
    "",
};

static const char *yue_dialog_4[] = {
    "Catnip produces an\n",
    "effect similar to\n",
    "LSD or marijuana\n",
    "in cats.",
    "",
};

static const char *yue_dialog_5[] = {
    "Cats often attack\n",
    "your ankles when\n",
    "they are bored.",
    "",
};

static const char *yue_dialog_6[] = {
    "A cat's purr\n",
    "vibrates at a\n",
    "frequency of\n",
    "25 to 150 hertz.",
    "",
};

static const char *yue_dialog_7[] = {
    "Cats love to sleep\n",
    "in laundry baskets,\n",
    "because they're\n",
    "basically hiding\n",
    "places with deep\n",
    "holes.",
    "",
};

static const char *yue_dialog_8[] = {
    "A cat could beat\n",
    "superstar runner\n",
    "Usain Bolt in the\n",
    "200 meter dash.",
    "",
};

static const char *yue_dialog_9[] = {
    "Cats find it\n",
    "threatening when\n",
    "you make direct\n",
    "eye contact with\n",
    "them.",
    "",
};

static const char *yue_dialog_10[] = {
    "Cats mark you as\n", "their territory\n", "when they rub their\n",
    "faces and bodies\n", "against you.",      "",
};

static const char *yue_dialog_11[] = {
    "A cat with a\n",
    "question-mark-shaped\n",
    "tail is asking:\n",
    "Want to play?",
    "",
};

static const char *yue_dialog_12[] = {
    "Cats' rough tongues\n",
    "can lick a bone\n",
    "clean of any shred\n",
    "of meat.",
    "",
};

} // namespace rpg::dialog

namespace rpg
{

class CharacterYue : virtual public Character, public MovingMixin
{
  public:
    CharacterYue() : Character(SceneObjectIdentity::GENERIC, 22, 29, 11, 28)
    {
        set_animation_variant(Appearance::moving_down,
                              animation::yue_moving_down, 2);

        set_animation_variant(Appearance::moving_left,
                              animation::yue_moving_left, 4);

        set_animation_variant(Appearance::moving_right,
                              animation::yue_moving_right, 4);

        set_animation_variant(Appearance::moving_up, animation::yue_moving_up,
                              2);
    }

    virtual const char **get_dialog() override
    {
        if (next_dialog == dialog::yue_dialog_1) {
            next_dialog = dialog::yue_dialog_2;
            return dialog::yue_dialog_1;
        }

        if (next_dialog == dialog::yue_dialog_2) {
            next_dialog = dialog::yue_dialog_3;
            return dialog::yue_dialog_2;
        }

        if (next_dialog == dialog::yue_dialog_3) {
            next_dialog = dialog::yue_dialog_4;
            return dialog::yue_dialog_3;
        }

        if (next_dialog == dialog::yue_dialog_4) {
            next_dialog = dialog::yue_dialog_5;
            return dialog::yue_dialog_4;
        }

        if (next_dialog == dialog::yue_dialog_5) {
            next_dialog = dialog::yue_dialog_6;
            return dialog::yue_dialog_5;
        }

        if (next_dialog == dialog::yue_dialog_6) {
            next_dialog = dialog::yue_dialog_7;
            return dialog::yue_dialog_6;
        }

        if (next_dialog == dialog::yue_dialog_7) {
            next_dialog = dialog::yue_dialog_8;
            return dialog::yue_dialog_7;
        }

        if (next_dialog == dialog::yue_dialog_8) {
            next_dialog = dialog::yue_dialog_9;
            return dialog::yue_dialog_8;
        }

        if (next_dialog == dialog::yue_dialog_9) {
            next_dialog = dialog::yue_dialog_10;
            return dialog::yue_dialog_9;
        }

        if (next_dialog == dialog::yue_dialog_10) {
            next_dialog = dialog::yue_dialog_11;
            return dialog::yue_dialog_10;
        }

        if (next_dialog == dialog::yue_dialog_11) {
            next_dialog = dialog::yue_dialog_12;
            return dialog::yue_dialog_11;
        }

        if (next_dialog == dialog::yue_dialog_12) {
            next_dialog = dialog::yue_dialog_1;
            return dialog::yue_dialog_12;
        }

        next_dialog = dialog::yue_dialog_2;
        return dialog::yue_dialog_1;
    }

    virtual const char *get_name() const override
    {
        return "Yue";
    }

    virtual void post_render(Viewport &viewport) override;
    virtual void render(Viewport &viewport) override;

  private:
    enum class Mode {
        standing,
        walking,
    };

    Mode current_mode = Mode::standing;
    const char **next_dialog;
};

} // namespace rpg
