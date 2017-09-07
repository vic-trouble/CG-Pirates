#include "game.h"

#include <algorithm>

int game::Entity::uniqueId_ = 0;

bool game::valid(const hex::Pos & pos)
{
    return 0 <= pos.x_ && pos.x_ < MAP_WIDTH && 0 <= pos.y_ && pos.y_ < MAP_HEIGHT;
}

hex::Pos game::Ship::bow() const
{
    return hex::neighbor(pos_, orient_);
}

hex::Pos game::Ship::stern() const
{
    return hex::neighbor(pos_, hex::opposite(orient_));
}

hex::Pos game::Ship::newBow() const
{
    return hex::neighbor(pos_, newOrient_);
}

hex::Pos game::Ship::newStern() const
{
    return hex::neighbor(pos_, hex::opposite(newOrient_));
}

bool game::Ship::dead() const
{
    return health_ <= 0;
}

void game::Ship::hurt(int damage)
{
    health_ = std::max(health_ - damage, 0);
}

void game::Ship::heal(int health)
{
    health_ = std::min(health_ + health, game::MAX_SHIP_HEALTH);
}
