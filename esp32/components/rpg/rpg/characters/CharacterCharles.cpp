#include "rpg/characters/CharacterCharles.h"

#include "rpg/Viewport.h"

namespace rpg
{

void CharacterCharles::render(Viewport &viewport)
{
    render_animation_variant(viewport, Appearance::standing, 30);
    Character::render(viewport);
}

} // namespace rpg
