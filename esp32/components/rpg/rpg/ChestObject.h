#pragma once

#include "rpg/Coordinates.h"
#include "rpg/SceneObject.h"

namespace rpg
{

class Viewport;

class ChestObject : public SceneObject
{
  public:
    ChestObject(SceneObjectIdentity identity, GlobalCoordinates coordinates,
                bool opened)
        : coordinates{coordinates}, identity{identity}, opened{opened}
    {
    }

    virtual GlobalCoordinates get_coordinates() const override
    {
        return coordinates;
    }

    virtual int get_height() const override
    {
        return 24;
    }

    virtual SceneObjectIdentity get_identity() const override
    {
        return identity;
    }

    virtual int get_sinkline() const override
    {
        return 23;
    }

    virtual int get_width() const override
    {
        return 24;
    }

    bool is_opened() const
    {
        return opened;
    }

    void open()
    {
        opened = true;
    }

    virtual void post_render(Viewport &viewport) override
    {
        // No-op.
    }

    virtual void render(Viewport &viewport) override;

  private:
    GlobalCoordinates coordinates;
    SceneObjectIdentity identity;

    bool opened = false;
};

} // namespace rpg
