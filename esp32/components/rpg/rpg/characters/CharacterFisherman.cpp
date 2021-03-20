#include "rpg/characters/CharacterFisherman.h"

#include "rpg/Viewport.h"

namespace rpg
{

void CharacterFisherman::render(Viewport &viewport)
{
    render_animation_variant(viewport, Appearance::standing, 4);
    Character::render(viewport);
}

} // namespace rpg
