#pragma once

#include "commands.h"
#include "game.h"

#include <memory>
#include <unordered_map>

namespace simulation
{

    /**
    Game "physics", ported from referee.java
    */
    class Simulation
    {
    public:
        explicit Simulation(game::Game &gameState);

        void newTick();

        void executeCommands(const std::unordered_map<int, std::unique_ptr<game::ICommand>> &commands);

        void moveShots();
        void decrementRum();
        void moveShips();
        void rotateShips();

        void explodeShips();
        void explodeMines();
        void explodeBarrels();

        void createLootRum();

        bool collideNewBow(const game::Ship &ship) const;
        bool collideNew(const game::Ship &ship) const;
        void collectPerks();

    private:
        game::Game &gameState_;
        std::vector<game::Ship *> allShips_;
        std::vector<hex::Pos> shotExplosions_;
        std::unordered_map<int, int> initialHealth_;
    };

    void tick(game::Game &gameState, const std::unordered_map<int, std::unique_ptr<game::ICommand>> &commands);

}
