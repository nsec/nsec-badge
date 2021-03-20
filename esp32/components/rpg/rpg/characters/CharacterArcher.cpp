#include "rpg/characters/CharacterArcher.h"

#include "rpg/Viewport.h"

namespace rpg
{

void CharacterArcher::render(Viewport &viewport)
{
    render_animation_variant(viewport, Appearance::standing, 2);
}

} // namespace rpg
