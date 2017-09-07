#include "ai_tactics.h"
#include "cxxtweaks.h"

#include <cmath>

int ai::ticksToTarget(const game::Ship &ship, const hex::Pos &target)
{
    int baseDistance = hex::distance(ship.pos_, target);
    double fullDistance = baseDistance * (1 + std::abs(hex::angle(ship.pos_, target) - ship.orient_) / 6.0);
    return static_cast<int>(fullDistance / game::MAX_SHIP_SPEED) + game::MAX_SHIP_SPEED - ship.speed_;
}

std::unique_ptr<game::ICommand> ai::tactical(const game::Ship &myShip, const game::Game &gameState)
{
    return {};
}
