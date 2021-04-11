#pragma once

#include "rpg/Coordinates.h"
#include "rpg_const.h"

namespace rpg
{

class Viewport;

class SceneObject
{
  public:
    virtual ~SceneObject()
    {
    }

    virtual GlobalCoordinates get_coordinates() const = 0;

    virtual int get_height() const = 0;

    virtual SceneObjectIdentity get_identity() const = 0;

    virtual int get_sinkline() const = 0;

    virtual int get_width() const = 0;

    virtual void post_render(Viewport &viewport) = 0;

    virtual void render(Viewport &viewport) = 0;
};

} // namespace rpg
