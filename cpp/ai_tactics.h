#pragma once

#include "commands.h"
#include "game.h"

#include <memory>

namespace ai
{
 
    /**
     Approximate number of ticks that are needed to reach the target based on the speed and orientation
     */
    int ticksToTarget(const game::Ship &ship, const hex::Pos &target);
    
    /**
     Advanced behavior that I could not implement with search :P
     May return empty command
     */
    std::unique_ptr<game::ICommand> tactical(const game::Ship &myShip, const game::Game &gameState);
    
}
