#pragma once

#include "game.h"
#include "hex.h"

#include <ostream>

std::ostream &operator << (std::ostream &os, const hex::Pos &pos);
std::ostream &operator << (std::ostream &os, const game::Ship &ship);
std::ostream &operator << (std::ostream &os, const game::Barrel &barrel);
std::ostream &operator << (std::ostream &os, const game::Shot &shot);
std::ostream &operator << (std::ostream &os, const game::Mine &mine);
