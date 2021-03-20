#pragma once

#include "rpg/SceneObject.h"
#include "rpg/Viewport.h"

namespace rpg
{

class TileObject : public SceneObject
{
  public:
    TileObject(GlobalCoordinates coordinates, int width, int height,
               int tile_image, int sinkline)
        : coordinates{coordinates}, width{width}, height{height},
          tile_image{tile_image}, sinkline{sinkline}
    {
    }

    virtual ~TileObject();

    virtual GlobalCoordinates get_coordinates() const override
    {
        return coordinates;
    }

    virtual int get_height() const override
    {
        return height;
    }

    int get_image() const
    {
        return tile_image;
    }

    virtual int get_sinkline() const override
    {
        return sinkline;
    }

    virtual int get_width() const override
    {
        return width;
    }

    virtual void render(Viewport &viewport) override;

  private:
    const GlobalCoordinates coordinates;
    const int width;
    const int height;
    const int tile_image;
    const int sinkline;
};

} // namespace rpg
