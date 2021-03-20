#pragma once

#include "rpg/Coordinates.h"
#include "rpg/SceneObject.h"

#include <string>

namespace rpg
{

class Viewport;

class Character : public SceneObject
{
  public:
    Character() : Character{"", 0, 0, 0, 0}
    {
    }

    Character(const char *name) : Character{name, 0, 0, 0, 0}
    {
    }

    Character(const char *name, int width, int height)
        : Character{name, width, height, 0, 0}
    {
    }

    Character(const char *name, int width, int height, int ground_base_x,
              int ground_base_y)
        : name{name}, width{width}, height{height},
          ground_base_x{ground_base_x}, ground_base_y{ground_base_y}
    {
        animation_step = 0;
        rendered_scene_x = 0;
        rendered_scene_y = 0;
        scene_x = 0;
        scene_y = 0;
    }

    unsigned int get_animation_step() const
    {
        return animation_step;
    }

    const std::string get_name() const
    {
        return name;
    }

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

    virtual void move(GlobalCoordinates coordinates)
    {
        scene_x = coordinates.x();
        scene_y = coordinates.y();
    }

    virtual void render(Viewport &viewport) = 0;

  protected:
    /**
     * Check that the character is currently visible on screen.
     */
    bool is_visible(Viewport &viewport);

  private:
    const std::string name;
    const int width;
    const int height;
    const int ground_base_x;
    const int ground_base_y;

    unsigned int animation_step;
    int rendered_scene_x;
    int rendered_scene_y;
    int scene_x;
    int scene_y;
};

} // namespace rpg
