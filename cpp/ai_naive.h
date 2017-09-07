#pragma once

#include "commands.h"
#include "game.h"

#include <memory>

namespace ai
{

    std::unique_ptr<game::ICommand> naive(const game::Ship &myShip, const game::Game &game);

}
