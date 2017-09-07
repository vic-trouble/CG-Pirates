#include "ai_naive.h"
#include "cxxtweaks.h"

#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace
{

    template <typename T>
    T clamp(T value, T min, T max)
    {
        return std::min(std::max(min, value), max);
    }

}

std::unique_ptr<game::ICommand> ai::naive(const game::Ship & myShip, const game::Game & game)
{
    const auto &theirShips = myShip.mine_ ? game.theirShips_ : game.myShips_;

    auto distToMe = [&myShip](const game::Entity &e)
    {
        auto d = e.pos_ - myShip.pos_;
        return std::sqrt(d.x_*d.x_ + d.y_*d.y_);
    };

    auto distComp = [&distToMe](const game::Entity &left, const game::Entity &right)
    {
        return distToMe(left) < distToMe(right);
    };

    // plant mine if the enemy is behind my stern
    if (!myShip.mineCooldown_)
    {
        auto mine = hex::neighbor(myShip.stern(), hex::opposite(myShip.orient_));
        for (const auto &theirShip: theirShips)
        {
            auto movingBow = theirShip.bow();
            for (int i = 0; i < theirShip.speed_; i++)
            {
                movingBow = hex::neighbor(movingBow, theirShip.orient_);
                if (movingBow == mine)
                    return std::make_unique<game::PlantMine>();
            }
        }
    }
    
    // first try to attack enemy
    if (!myShip.cannonCooldown_)
    {
        for (const auto &theirShip : theirShips)
        {
            auto dist = distToMe(theirShip);
            if (dist < 5)
            {
                int next_x = std::lround(theirShip.pos_.x_ + theirShip.speed_ * dist * std::cos(theirShip.orient_ * std::PI / 3));
                int next_y = std::lround(theirShip.pos_.y_ + theirShip.speed_ * dist * std::sin(theirShip.orient_ * std::PI / 3));
                return std::make_unique<game::Fire>(hex::Pos(clamp(next_x, 0, game::MAP_WIDTH-1), clamp(next_y, 0, game::MAP_HEIGHT-1)));
            }
        }
    }

    /*
    // then attack mines
    if (!game.mines_.empty())
    {
        auto it = std::min_element(game.mines_.cbegin(), game.mines_.cend(), distComp);
        if (distToMe(*it) < 3 && (game.tick_ & 1) && !bombedMines.count(it->id_))
        {
            bombedMines.insert(it->id_);
            return std::make_unique<game::Fire>(it->pos_);
        }
    }
    */
    
    // then collect barrels
    if (!game.barrels_.empty())
    {
        auto it = std::min_element(game.barrels_.cbegin(), game.barrels_.cend(), distComp);
        return std::make_unique<game::Move>(it->pos_);
    }

    // then pursue their ships
    auto it = std::min_element(theirShips.cbegin(), theirShips.cend(), distComp);
    return std::make_unique<game::Move>(it->pos_);
}
