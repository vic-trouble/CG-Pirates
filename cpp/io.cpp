#include "io.h"

std::ostream &operator << (std::ostream &os, const hex::Pos &pos)
{
    return os << "(" << pos.x_ << ", " << pos.y_ << ")";
}

std::ostream & operator<<(std::ostream & os, const game::Ship & ship)
{
    return os << "Ship#" << ship.id_ << "(" << ship.pos_ 
        << ", orient=" << ship.orient_ 
        << ", speed=" << ship.speed_
        << ", health=" << ship.health_
        << ")";
}

std::ostream & operator<<(std::ostream & os, const game::Barrel & barrel)
{
    return os << "Barrel#" << barrel.id_ << "(" << barrel.pos_
        << ", health=" << barrel.health_
        << ")";
}

std::ostream & operator<<(std::ostream & os, const game::Shot & shot)
{
    return os << "Shot#" << shot.id_ << "(" << shot.pos_
        << ", origin=" << shot.originId_
        << ", turnsToLand=" << shot.ticksToLand_
        << ")";
}

std::ostream & operator<<(std::ostream & os, const game::Mine & mine)
{
    return os << "Mine#" << mine.id_ << "(" << mine.pos_
        << ")";
}

