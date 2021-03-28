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
    void change_tile_by(int16_t dx, int16_t dy)
    {
        value_tile_x += dx;
        value_tile_y += dy;

        value_x += convert_to_xy(dx);
        value_y += convert_to_xy(dy);
    }

    void change_xy_by(int16_t dx, int16_t dy)
    {
        value_x += dx;
        value_y += dy;

        value_tile_x = convert_to_tile(value_x);
        value_tile_y = convert_to_tile(value_y);
    }

    int16_t tile_x() const
    {
        return value_tile_x;
    }

    int16_t tile_y() const
    {
        return value_tile_y;
    }

    int16_t x() const
    {
        return value_x;
    }

    int16_t y() const
    {
        return value_y;
    }

    /**
     * Check that coordinates lay within the specified box using tile units.
     */
    bool within_tile(int16_t min_x, int16_t min_y, int16_t max_x, int16_t max_y)
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
    bool within_xy(int16_t min_x, int16_t min_y, int16_t max_x, int16_t max_y)
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
    Coordinates(int16_t x, int16_t y, int16_t tile_x, int16_t tile_y)
        : value_tile_x{tile_x}, value_tile_y{tile_y}, value_x{x}, value_y{y}
    {
    }

    static int16_t convert_to_tile(int16_t xy)
    {
        return xy / DISPLAY_TILE_WIDTH -
               ((xy % DISPLAY_TILE_WIDTH) < 0 ? 1 : 0);
    }

    static int16_t convert_to_xy(int16_t tile)
    {
        return tile * DISPLAY_TILE_WIDTH;
    }

  private:
    int16_t value_tile_x;
    int16_t value_tile_y;
    int16_t value_x;
    int16_t value_y;
};

/**
 * Global coordinates system of a scene.
 */
class GlobalCoordinates : public Coordinates
{
  public:
    using Coordinates::Coordinates;

    static GlobalCoordinates tile(int16_t tile_x, int16_t tile_y)
    {
        return GlobalCoordinates(convert_to_xy(tile_x), convert_to_xy(tile_y),
                                 tile_x, tile_y);
    }

    static GlobalCoordinates xy(int16_t x, int16_t y)
    {
        return GlobalCoordinates(x, y, convert_to_tile(x), convert_to_tile(y));
    }

    bool within(GlobalCoordinates min, GlobalCoordinates max)
    {
        return within_xy(min.x(), min.y(), max.x(), max.y());
    }
};

/**
 * Internal coordinates system of the viewport.
 */
class LocalCoordinates : public Coordinates
{
  public:
    using Coordinates::Coordinates;

    static LocalCoordinates tile(int16_t tile_x, int16_t tile_y)
    {
        return LocalCoordinates(convert_to_xy(tile_x), convert_to_xy(tile_y),
                                tile_x, tile_y);
    }

    static LocalCoordinates xy(int16_t x, int16_t y)
    {
        return LocalCoordinates(x, y, convert_to_tile(x), convert_to_tile(y));
    }

    bool within(LocalCoordinates min, LocalCoordinates max)
    {
        return within_xy(min.x(), min.y(), max.x(), max.y());
    }
};

/**
 * Coordinates system of the screen.
 */
class ScreenCoordinates : public Coordinates
{
  public:
    using Coordinates::Coordinates;

    static ScreenCoordinates tile(int16_t tile_x, int16_t tile_y)
    {
        return ScreenCoordinates(convert_to_xy(tile_x), convert_to_xy(tile_y),
                                 tile_x, tile_y);
    }

    static ScreenCoordinates xy(int16_t x, int16_t y)
    {
        return ScreenCoordinates(x, y, convert_to_tile(x), convert_to_tile(y));
    }

    bool within(ScreenCoordinates min, ScreenCoordinates max)
    {
        return within_xy(min.x(), min.y(), max.x(), max.y());
    }
};

inline bool operator==(const GlobalCoordinates &lhs,
                       const GlobalCoordinates &rhs)
{
    return lhs.x() == rhs.x() && lhs.y() == rhs.y();
}

inline bool operator!=(const GlobalCoordinates &lhs,
                       const GlobalCoordinates &rhs)
{
    return !(lhs == rhs);
}

inline bool operator==(const LocalCoordinates &lhs, const LocalCoordinates &rhs)
{
    return lhs.x() == rhs.x() && lhs.y() == rhs.y();
}

inline bool operator!=(const LocalCoordinates &lhs, const LocalCoordinates &rhs)
{
    return !(lhs == rhs);
}

inline bool operator==(const ScreenCoordinates &lhs,
                       const ScreenCoordinates &rhs)
{
    return lhs.x() == rhs.x() && lhs.y() == rhs.y();
}

inline bool operator!=(const ScreenCoordinates &lhs,
                       const ScreenCoordinates &rhs)
{
    return !(lhs == rhs);
}

} // namespace rpg
