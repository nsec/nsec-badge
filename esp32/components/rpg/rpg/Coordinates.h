#pragma once

#include "graphics.h"

namespace rpg
{

/**
 * Base class for all coordinates systems.
 */
class Coordinates
{
  public:
    void change_xy_by(int dx, int dy)
    {
        value_x += dx;
        value_y += dy;

        value_tile_x = convert_to_tile(value_x);
        value_tile_y = convert_to_tile(value_y);
    }

    int tile_x() const
    {
        return value_tile_x;
    }

    int tile_y() const
    {
        return value_tile_y;
    }

    int x() const
    {
        return value_x;
    }

    int y() const
    {
        return value_y;
    }

    /**
     * Check that coordinates lay within the specified box using tile units.
     */
    bool within_tile(int min_x, int min_y, int max_x, int max_y)
    {
        if (value_tile_x < min_x)
            return false;

        if (value_tile_y < min_y)
            return false;

        if (value_tile_x > max_x)
            return false;

        if (value_tile_y > max_y)
            return false;

        return true;
    }

    /**
     * Check that coordinates lay within the specified box using pixels.
     */
    bool within_xy(int min_x, int min_y, int max_x, int max_y)
    {
        if (value_x < min_x)
            return false;

        if (value_y < min_y)
            return false;

        if (value_x > max_x)
            return false;

        if (value_y > max_y)
            return false;

        return true;
    }

  protected:
    Coordinates(int x, int y, int tile_x, int tile_y)
        : value_tile_x{tile_x}, value_tile_y{tile_y}, value_x{x}, value_y{y}
    {
    }

    static int convert_to_tile(int xy)
    {
        return xy / DISPLAY_TILE_WIDTH -
               ((xy % DISPLAY_TILE_WIDTH) < 0 ? 1 : 0);
    }

    static int convert_to_xy(int tile)
    {
        return tile * DISPLAY_TILE_WIDTH;
    }

  private:
    int value_tile_x;
    int value_tile_y;
    int value_x;
    int value_y;
};

/**
 * Global coordinates system of a scene.
 */
class GlobalCoordinates : public Coordinates
{
  public:
    using Coordinates::Coordinates;

    static GlobalCoordinates tile(int tile_x, int tile_y)
    {
        return GlobalCoordinates(convert_to_xy(tile_x), convert_to_xy(tile_y),
                                 tile_x, tile_y);
    }

    static GlobalCoordinates xy(int x, int y)
    {
        return GlobalCoordinates(x, y, convert_to_tile(x), convert_to_tile(y));
    }
};

/**
 * Internal coordinates system of the viewport.
 */
class LocalCoordinates : public Coordinates
{
  public:
    using Coordinates::Coordinates;

    static LocalCoordinates tile(int tile_x, int tile_y)
    {
        return LocalCoordinates(convert_to_xy(tile_x), convert_to_xy(tile_y),
                                tile_x, tile_y);
    }

    static LocalCoordinates xy(int x, int y)
    {
        return LocalCoordinates(x, y, convert_to_tile(x), convert_to_tile(y));
    }
};

/**
 * Coordinates system of the screen.
 */
class ScreenCoordinates : public Coordinates
{
  public:
    using Coordinates::Coordinates;

    static ScreenCoordinates tile(int tile_x, int tile_y)
    {
        return ScreenCoordinates(convert_to_xy(tile_x), convert_to_xy(tile_y),
                                 tile_x, tile_y);
    }

    static ScreenCoordinates xy(int x, int y)
    {
        return ScreenCoordinates(x, y, convert_to_tile(x), convert_to_tile(y));
    }
};

} // namespace rpg
