#include "ai_naive.h"
#include "ai_search.h"
#include "commands.h"
#include "cxxtweaks.h"
#include "game.h"
#include "io.h"
#include "simulation.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

// undefine to do runtime simulation check
//#define CHECK_SIMULATION

///
/// Create entity from the referee's input
///
std::unique_ptr<game::Entity> parseEntity(const std::string &type, int x, int y, int args[4])
{
    hex::Pos pos(x, y);
    if (type == "SHIP")
        return std::make_unique<game::Ship>(pos, args[0], args[1], args[2], args[3] == 1);
    else if (type == "BARREL")
        return std::make_unique<game::Barrel>(pos, args[0]);
    else if (type == "CANNONBALL")
        return std::make_unique<game::Shot>(pos, args[0], args[1]);
    else if (type == "MINE")
        return std::make_unique<game::Mine>(pos);
    else
        throw std::logic_error("unknown entity " + type);
}

/**
Measuring tick performance
*/
class StopWatch
{
public:
    StopWatch()
        : start_(std::chrono::high_resolution_clock::now())
    {
    }

    ~StopWatch()
    {
        auto dur = std::chrono::high_resolution_clock::now() - start_;
        std::cerr << "ticked in " << std::chrono::duration_cast<std::chrono::milliseconds>(dur).count() << " ms" << std::endl;
    }

private:
    std::chrono::high_resolution_clock::time_point start_;
};

///
/// String representation of the game state, for debug reasons
///
std::string ToString(const game::Game &g)
{
    std::stringstream ss;
    ss << "myShips = {";
    for (const auto &ship : g.myShips_)
        ss << ship << ", ";
    ss << "}";
/*  // we don't control them, no sense in comparing them
    ss << "\ntheirShips = {";
    for (const auto &ship : g.theirShips_)
        ss << ship << ", ";
    ss << "}";

    ss << "\nbarrels = {";
    for (const auto &barrel : g.barrels_)
        ss << barrel << ", ";
    ss << "}";

    ss << "\nshots = {";
    for (const auto &shot : g.shots_)
        ss << shot << ", ";
    ss << "}";

    ss << "\nmines = {";
    for (const auto &mine : g.mines_)
        ss << mine << ", ";
    ss << "}";
*/
    return ss.str();
}

/**
Holds information between turns
*/
class Memory
{
public:
    void memorize(const game::Game &gameState)
    {
        memorize(gameState.myShips_);
        memorize(gameState.theirShips_);
    }

    void retrieve(game::Game &gameState)
    {
        retrieve(gameState.myShips_);
        retrieve(gameState.theirShips_);
    }

private:
    void memorize(const std::vector<game::Ship> &ships)
    {
        for (const auto &ship : ships)
        {
            cannonCooldown_[ship.id_] = ship.cannonCooldown_;
            mineCooldown_[ship.id_] = ship.mineCooldown_;
        }
    }

    void retrieve(std::vector<game::Ship> &ships)
    {
        for (auto &ship : ships)
        {
            ship.cannonCooldown_ = cannonCooldown_[ship.id_];
            if (ship.cannonCooldown_)
                ship.cannonCooldown_--;
            
            ship.mineCooldown_ = mineCooldown_[ship.id_];
            if (ship.mineCooldown_)
                ship.mineCooldown_--;
        }
    }

private:
    std::unordered_map<int, int> cannonCooldown_;
    std::unordered_map<int, int> mineCooldown_;
};

///
/// Entry point
///
int main()
{
#ifdef CHECK_SIMULATION
    game::Game nextGameState;
#endif

    Memory memory;
    for (int tick = 0;; tick++)
    {
        StopWatch stopWatch;

        // read inputs
        game::Game gameState;
        gameState.tick_ = tick;
        int myShipCount; // the number of remaining ships
        std::cin >> myShipCount; std::cin.ignore();
        int entityCount; // the number of entities (e.g. ships, mines or cannonballs)
        std::cin >> entityCount; std::cin.ignore();
        for (int i = 0; i < entityCount; i++) {
            int entityId;
            std::string entityType;
            int x;
            int y;
            int args[4];
            std::cin >> entityId >> entityType >> x >> y >> args[0] >> args[1] >> args[2] >> args[3]; std::cin.ignore();
            std::unique_ptr<game::Entity> entity = parseEntity(entityType, x, y, args);
            entity->id_ = entityId;
            if (game::Ship *ship = dynamic_cast<game::Ship *>(entity.get()))
            {
                if (ship->mine_)
                    gameState.myShips_.push_back(*ship);
                else
                    gameState.theirShips_.push_back(*ship);
            }
            else if (game::Barrel *barrel = dynamic_cast<game::Barrel *>(entity.get()))
                gameState.barrels_.push_back(*barrel);
            else if (game::Mine *mine = dynamic_cast<game::Mine *>(entity.get()))
                gameState.mines_.push_back(*mine);
            else if (game::Shot *shot = dynamic_cast<game::Shot *>(entity.get()))
                gameState.shots_.push_back(*shot);
            else
                throw std::logic_error("unknown entity");
        }

#ifdef CHECK_SIMULATION
        if (tick)
        {
            std::string expected = ToString(nextGameState), actual = ToString(gameState);
            if (expected != actual)
                std::cerr << "Simulation error, expected\n" << expected << "\n, actual\n" << actual << std::endl;
        }
#endif

        // sync from memory
        memory.retrieve(gameState);

        // call AI
        auto commands = ai::search(gameState);
        for (auto &myShip: gameState.myShips_)
        {
            std::cout << commands[myShip.id_]->Encode() << std::endl;
            commands[myShip.id_]->Execute(gameState, myShip);
        }

        // sync to memory
        memory.memorize(gameState);

#ifdef CHECK_SIMULATION
        nextGameState = gameState;
        simulation::tick(nextGameState, commands);
#endif
    }
}
