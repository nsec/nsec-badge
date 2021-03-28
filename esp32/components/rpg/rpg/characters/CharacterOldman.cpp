#include "rpg/characters/CharacterOldman.h"

#include "rpg/Viewport.h"

namespace rpg
{

void CharacterOldman::render(Viewport &viewport)
{
    render_animation_variant(viewport, Appearance::standing, 25);
    Character::render(viewport);
}

} // namespace rpg
