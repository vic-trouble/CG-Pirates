#include "simulation.h"

#include <algorithm>

simulation::Simulation::Simulation(game::Game &gameState)
    : gameState_(gameState)
{
    for (auto &ship : gameState.myShips_)
        allShips_.push_back(&ship);
    for (auto &ship : gameState.theirShips_)
        allShips_.push_back(&ship);
}

void simulation::Simulation::newTick()
{
    gameState_.tick_++;

    for (auto &ship : allShips_)
    {
        initialHealth_[ship->id_] = ship->health_;
        ship->newOrient_ = ship->orient_;

        if (ship->cannonCooldown_)
            ship->cannonCooldown_--;
        if (ship->mineCooldown_)
            ship->mineCooldown_--;
    }
}

void simulation::Simulation::executeCommands(const std::unordered_map<int, std::unique_ptr<game::ICommand>>& commands)
{
    for (auto &ship : allShips_)
    {
        if (commands.count(ship->id_))
            commands.at(ship->id_)->Execute(gameState_, *ship);
    }
}

void simulation::Simulation::moveShots()
{
    for (auto it = gameState_.shots_.begin(); it != gameState_.shots_.end(); )
    {
        if (it->ticksToLand_ == 0) 
        {
            it = gameState_.shots_.erase(it);
            continue;
        }

        if (--it->ticksToLand_ == 0)
            shotExplosions_.emplace_back(it->pos_);

        ++it;
    }
}

void simulation::Simulation::decrementRum()
{
    for (auto &ship : allShips_)
        ship->hurt(1);
}

void simulation::Simulation::moveShips()
{
    for (int i = 1; i <= game::MAX_SHIP_SPEED; i++) 
    {
        for (auto &ship : allShips_) 
        {
            if (ship->dead())
                continue;

            ship->newPos_ = ship->pos_;
            ship->newBow_ = ship->bow();
            ship->newStern_ = ship->stern();

            if (i > ship->speed_)
                continue;

            hex::Pos newPos = hex::neighbor(ship->pos_, ship->orient_);

            if (game::valid(newPos)) 
            {
                // Set new coordinate.
                ship->newPos_ = newPos;
                ship->newBow_ = hex::neighbor(newPos, ship->orient_);
                ship->newStern_ = hex::neighbor(newPos, hex::opposite(ship->orient_));
            }
            else 
            {
                // Stop ship!
                ship->speed_ = 0;
            }
        }

        // Check ship and obstacles collisions
        std::vector<game::Ship *> collisions;
        bool collisionDetected = true;
        while (collisionDetected) 
        {
            collisionDetected = false;

            for (auto &ship : allShips_) 
            {
                if (collideNewBow(*ship)) 
                    collisions.push_back(ship);
            }

            for (auto &ship : collisions) 
            {
                // Revert last move
                ship->newPos_ = ship->pos_;
                ship->newBow_ = ship->bow();
                ship->newStern_ = ship->stern();

                // Stop ships
                ship->speed_ = 0;

                collisionDetected = true;
            }
            collisions.clear();
        }

        // Apply movement
        for (auto &ship : allShips_)
            ship->pos_ = ship->newPos_;

        // Collect barrels and mines
        collectPerks();
    }
}

void simulation::Simulation::rotateShips()
{
    for (auto &ship : allShips_) 
    {
        ship->newPos_ = ship->pos_;
        ship->newBow_ = ship->newBow();
        ship->newStern_ = ship->newStern();
    }

    // Check collisions
    bool collisionDetected = true;
    std::vector<game::Ship *> collisions;
    while (collisionDetected)
    {
        collisionDetected = false;

        for (auto &ship : allShips_)
        {
            if (collideNew(*ship))
                collisions.push_back(ship);
        }

        for (auto &ship : collisions)
        {
            ship->newOrient_ = ship->orient_;
            ship->newBow_ = ship->newBow();
            ship->newStern_ = ship->newStern();
            ship->speed_ = 0;
            collisionDetected = true;
        }

        collisions.clear();
    }

    // Apply rotation
    for (auto &ship : allShips_) 
    {
        if (ship->dead())
            continue;

        ship->orient_ = ship->newOrient_;
    }

    // Collect barrels and mines
    collectPerks();
}

void simulation::Simulation::explodeShips()
{
    for (auto it = shotExplosions_.begin(); it != shotExplosions_.end();)
    {
        bool exploded = false;
        for (auto &ship: allShips_)
        {
            if (*it == ship->bow() || *it == ship->stern())
            {
                ship->hurt(game::LOW_DAMAGE);
                exploded = true;
                break;
            }
            else if (*it == ship->pos_)
            {
                ship->hurt(game::HIGH_DAMAGE);
                exploded = true;
                break;
            }
        }
        if (exploded)
            it = shotExplosions_.erase(it);
        else
            ++it;
    }
}

void simulation::Simulation::explodeMines()
{
    for (auto it = shotExplosions_.begin(); it != shotExplosions_.end();)
    {
        bool explode = false;
        for (auto mit = gameState_.mines_.begin(); mit != gameState_.mines_.end(); ++mit)
        {
            if (mit->pos_ == *it)
            {
                hex::Pos adjacent[6];
                for (int i = 0; i < 6; i++)
                    adjacent[i] = hex::neighbor(mit->pos_, i);
                for (const auto &ship : allShips_)
                {
                    for (const auto &dmg : adjacent)
                    {
                        if (dmg == ship->pos_ || dmg == ship->bow() || dmg == ship->stern())
                        {
                            ship->hurt(game::NEAR_MINE_DAMAGE);
                            break;
                        }
                    }
                }
                gameState_.mines_.erase(mit);
                explode = true;
                break;
            }
        }
        if (explode)
            it = shotExplosions_.erase(it);
        else
            ++it;
    }
}

void simulation::Simulation::explodeBarrels()
{
    for (auto it = shotExplosions_.begin(); it != shotExplosions_.end();)
    {
        bool explode = false;
        for (auto bit = gameState_.barrels_.begin(); bit != gameState_.barrels_.end(); ++bit)
        {
            if (bit->pos_ == *it)
            {
                gameState_.barrels_.erase(bit);
                explode = true;
                break;
            }
        }
        if (explode)
            it = shotExplosions_.erase(it);
        else
            ++it;
    }
}

void simulation::Simulation::createLootRum()
{
    for (const auto &ship : allShips_)
    {
        if (ship->dead())
            gameState_.barrels_.emplace_back(ship->pos_, std::min(game::REWARD_RUM_BARREL_VALUE, initialHealth_[ship->id_]));
    }
}

bool simulation::Simulation::collideNewBow(const game::Ship & ship) const
{
    for (const auto &otherShip : allShips_)
    {
        if (otherShip == &ship)
            continue;

        if (ship.newBow_ == otherShip->newPos_ || ship.newBow_ == otherShip->newBow_ || ship.newBow_ == otherShip->newStern_)
            return true;
    }
    return false;
}

bool simulation::Simulation::collideNew(const game::Ship & ship) const
{
    for (const auto &otherShip : allShips_)
    {
        if (otherShip == &ship)
            continue;

        if (ship.newBow_ == otherShip->newBow_ || ship.newBow_ == otherShip->newPos_ || ship.newBow_ == otherShip->newStern_)
            return true;
        if (ship.newPos_ == otherShip->newBow_ || ship.newPos_ == otherShip->newPos_ || ship.newPos_ == otherShip->newStern_)
            return true;
        if (ship.newStern_ == otherShip->newBow_ || ship.newStern_ == otherShip->newPos_ || ship.newStern_ == otherShip->newStern_)
            return true;
    }
    return false;
}

void simulation::Simulation::collectPerks()
{
    // Collision with barrels
    for (auto &ship: allShips_)
    {
        hex::Pos bow = ship->bow();
        hex::Pos stern = ship->stern();
        hex::Pos center = ship->pos_;

        // Collision with the barrels
        for (auto it = gameState_.barrels_.begin(); it != gameState_.barrels_.end();)
        {
            if (it->pos_ == bow || it->pos_ == stern || it->pos_ == center)
            {
                ship->heal(it->health_);
                it = gameState_.barrels_.erase(it);
            }
            else
                ++it;
        }
    }

    // Collision with the mines
    for (auto &ship: allShips_)
    {
        hex::Pos bow = ship->bow();
        hex::Pos stern = ship->stern();
        hex::Pos center = ship->pos_;

        for (auto it = gameState_.mines_.begin(); it != gameState_.mines_.end();)
        {
            if (it->pos_ == bow || it->pos_ == stern || it->pos_ == center)
            {
                ship->hurt(game::MINE_DAMAGE);

                hex::Pos adjacent[6];
                for (int i = 0; i < 6; i++)
                    adjacent[i] = hex::neighbor(it->pos_, i);
                for (const auto &otherShip : allShips_)
                {
                    if (otherShip == ship)
                        continue;
                    for (const auto &dmg : adjacent)
                        if (dmg == otherShip->pos_ || dmg == otherShip->bow() || dmg == otherShip->stern())
                            otherShip->hurt(game::NEAR_MINE_DAMAGE);
                }

                it = gameState_.mines_.erase(it);
            }
            else
                ++it;
        }
    }
}

void simulation::tick(game::Game & gameState, const std::unordered_map<int, std::unique_ptr<game::ICommand>> &commands)
{
    Simulation sim(gameState);

    sim.newTick();

    sim.moveShots();
    sim.decrementRum();

    sim.executeCommands(commands);

    sim.moveShips();
    sim.rotateShips();

    sim.explodeShips();
    sim.explodeMines();
    sim.explodeBarrels();

    sim.createLootRum();

    for (auto it = gameState.myShips_.begin(); it != gameState.myShips_.end();)
    {
        if (it->dead())
            it = gameState.myShips_.erase(it);
        else
            ++it;
    }
    for (auto it = gameState.theirShips_.begin(); it != gameState.theirShips_.end();)
    {
        if (it->dead())
            it = gameState.theirShips_.erase(it);
        else
            ++it;
    }
}
