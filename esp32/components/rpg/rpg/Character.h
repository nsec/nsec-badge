#pragma once

#include "rpg/Coordinates.h"
#include "rpg/SceneObject.h"
#include "rpg/TimerDevice.h"
#include "rpg/data/BlockedDataReader.h"

namespace rpg
{

class Viewport;

class Character : public SceneObject
{
  protected:
    enum class Appearance {
        moving_down = 0,
        moving_left = 1,
        moving_right = 2,
        moving_up = 3,
        standing = 4,
        action_a = 5,
        action_b = 6,
        action_c = 7,
        LENGTH = 8,
    };

  public:
    Character() : Character{character::Identity::GENERIC, 0, 0, 0, 0}
    {
    }

    Character(character::Identity identity, int16_t width, int16_t height)
        : Character{identity, width, height, 0, 0}
    {
    }

    Character(character::Identity identity, int16_t width, int16_t height, int16_t ground_base_x, int16_t ground_base_y)
        : width{width}, height{height},
          ground_base_x{ground_base_x}, ground_base_y{ground_base_y},
          move_distance{GlobalCoordinates::xy(0, 0)}, animation_variants{}, identity{identity}
    {
    }

    virtual unsigned int get_animation_step() const
    {
        return animation_step;
    }

    virtual const char *get_name() const = 0;

    virtual GlobalCoordinates get_coordinates() const override
    {
        return GlobalCoordinates::xy(scene_x, scene_y);
    }

    int get_ground_base_x() const
    {
        return ground_base_x;
    }

    int get_ground_base_y() const
    {
        return ground_base_y;
    }

    virtual int get_height() const override
    {
        return height;
    }

    character::Identity get_identity() const
    {
        return identity;
    }

    GlobalCoordinates get_rendered_coordinates() const
    {
        return GlobalCoordinates::xy(rendered_scene_x, rendered_scene_y);
    }

    virtual int get_sinkline() const override
    {
        // Use the ground position of the character as the default sinkline.
        return get_ground_base_y();
    }

    virtual int get_width() const override
    {
        return width;
    }

    void increment_animation_step()
    {
        ++animation_step;
    }

    /**
     * Move the character to a new position in the scene.
     *
     * If the blocked_data_reader pointer is set, the destination will be
     * checked before making the move and if the position is blocked, the
     * character will stay in place. If blocked_data_reader is not set,
     * every move is possible.
     *
     * The return value is true if the character has been moved, and false if
     * it stayed in place.
     *
     * Additionally, the distance between the old and new locations is recorded
     * into the move_distance property.
     */
    virtual bool move(GlobalCoordinates coordinates);

    /**
     * Move the character to the initial position.
     *
     * This method must be called right after the character initialization,
     * before any other calls to move().
     */
    void move_initial(GlobalCoordinates coordinates);

    virtual void post_render(Viewport &viewport) override
    {
        // No-op.
    }

    virtual void render(Viewport &viewport) = 0;

    void set_blocked_data_reader(data::BlockedDataReader &data_reader)
    {
        blocked_data_reader = &data_reader;
    }

  protected:
    TimerDevice timer{};

    /**
     * Check that the character is currently visible on screen.
     */
    bool is_visible(Viewport &viewport);

    GlobalCoordinates get_initial_coordinates() const
    {
        return GlobalCoordinates::xy(initial_scene_x, initial_scene_y);
    }

    GlobalCoordinates get_move_distance()
    {
        return move_distance;
    }

    void render_animation_variant(Viewport &viewport,
                                  const Appearance appearance,
                                  const int slow_down = 1);

    void reset_move_distance()
    {
        move_distance = GlobalCoordinates::xy(0, 0);
    }

    void set_animation_variant(const Appearance appearance, const int *sprites,
                               const int count);

  private:
    const int16_t width;
    const int16_t height;
    const int16_t ground_base_x;
    const int16_t ground_base_y;

    unsigned int animation_step{0};
    int16_t initial_scene_x{0};
    int16_t initial_scene_y{0};
    int16_t rendered_scene_x{0};
    int16_t rendered_scene_y{0};
    int16_t scene_x{0};
    int16_t scene_y{0};

    GlobalCoordinates move_distance;

    data::BlockedDataReader *blocked_data_reader = 0;

    struct AnimationVariant {
        const int *sprites;
        int count;
    };

    AnimationVariant animation_variants[static_cast<int>(Appearance::LENGTH)];

    character::Identity identity;
};

} // namespace rpg
