#pragma once

#include "commands.h"
#include "game.h"

#include <memory>
#include <unordered_map>

namespace ai
{

    ///
    /// Search through the solution space in order to find best score
    ///
    std::unordered_map<int, std::unique_ptr<game::ICommand>> search(const game::Game &gameState, unsigned depthLimit = 0);

}
