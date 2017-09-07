#include "ai_naive.h"
#include "ai_score.h"
#include "ai_search.h"
#include "ai_tactics.h"
#include "cxxtweaks.h"
#include "log.h"
#include "simulation.h"

#include <cassert>
#include <chrono>
#include <cstdint>
#include <queue>
#include <sstream>
#include <tuple>
#include <unordered_set>
#include <vector>

namespace
{
    /**
    Outcome of simulation
    */
    enum class SimulationResult: std::uint8_t
    {
        WON,
        GAME_ON,
        LOST
    };

    std::ostream &operator << (std::ostream &os, SimulationResult result)
    {
        static const char *str[] = { "won", "gameOn", "lost" };
        return os << str[static_cast<int>(result)];
    }

    /*
    bool operator < (const SimulationResult &left, const SimulationResult &right)
    {
        return static_cast<int>(left) < static_cast<int>(right);
    }
    */

    /**
    Subject of the search
    */
    struct Choice
    {
        SimulationResult result;
        double score;
        unsigned depth;
        size_t parent_index;
        game::Game game;   // TODO: move up
        std::unique_ptr<game::ICommand> command;

        Choice(Choice &&i_other) = default;
        Choice &operator =(Choice &&i_other) = default;

        bool better(const Choice &right) const
        {
            if (result != right.result)
                return static_cast<int>(result) < static_cast<int>(right.result);

            if (score != right.score)
                return score > right.score;

            return depth < right.depth;
        }
    };

    /**
    Get ship by her id, may return null if ship is not found
    */
    const game::Ship *getShipById(const std::vector<game::Ship> &ships, int shipId)
    {
        const game::Ship *myShip = nullptr;
        for (const auto &ship : ships)
        {
            if (ship.id_ == shipId)
            {
                myShip = &ship;
                break;
            }
        }
        return myShip;
    }

    /**
    Determines simulation's result
    */
    SimulationResult gameResult(const game::Game &gameState)
    {
        if (gameState.myShips_.empty())
            return SimulationResult::LOST;

        if (gameState.theirShips_.empty())
            return SimulationResult::WON;

        return SimulationResult::GAME_ON;
    }
}

/**
Generate possible moves
*/
std::vector<std::unique_ptr<game::ICommand>> generateCommands(int shipId, const game::Game &gameState)
{
    const game::Ship *myShip = getShipById(gameState.myShips_, shipId);
    assert(myShip);
    
    // navigation
    std::vector<std::unique_ptr<game::ICommand>> commands;
    commands.push_back(std::make_unique<game::Wait>());
    commands.push_back(std::make_unique<game::TurnLeft>());
    commands.push_back(std::make_unique<game::TurnRight>());
    if (myShip->speed_ > 0)
        commands.push_back(std::make_unique<game::SlowDown>());
    if (myShip->speed_ < game::MAX_SHIP_SPEED)
        commands.push_back(std::make_unique<game::SpeedUp>());

    // offense
    if (!myShip->mineCooldown_)
        commands.push_back(std::make_unique<game::PlantMine>());

    if (!myShip->cannonCooldown_)
    {
        for (const auto &theirShip : gameState.theirShips_)
        {
            auto cannon = myShip->bow();
            game::Ship target = theirShip;
            game::Game targetGame = gameState;
            for (int i = 1; i <= game::FIRE_DISTANCE_MAX; i++)
            {
                std::unordered_map<int, std::unique_ptr<game::ICommand>> targetCommands;
                targetCommands[target.id_] = ai::naive(target, targetGame);
                simulation::tick(targetGame, targetCommands);
                const game::Ship *newTarget = getShipById(targetGame.theirShips_, target.id_);
                if (!newTarget)
                    break;
                target = *newTarget;
                int distance = hex::distance(cannon, target.pos_);
                if (distance <= game::FIRE_DISTANCE_MAX && 2 + std::lround(distance / 3.0) <= i)
                {
                    commands.push_back(std::make_unique<game::Fire>(target.pos_));
                    break;
                }
            }
        }
    }

    return commands;
}

std::vector<Choice> choices;

class IndexComp
{
public:
    bool operator()(size_t i_left, size_t i_right) const
    {
        return choices[i_right].better(choices[i_left]);
    }
};

std::tuple<std::unique_ptr<game::ICommand>, double, SimulationResult> searchImpl(int shipId, const game::Game &gameState, unsigned depthLimit, const std::chrono::milliseconds &timeLimit)
{
    const std::chrono::high_resolution_clock::time_point &startTime = std::chrono::high_resolution_clock::now();

    // prepare choices
    auto commands = generateCommands(shipId, gameState);

    unsigned maxDepth = 0;
    size_t maxScoreIndex = 0;
    
    constexpr int batchSize = 10;

    // choices are all considered simulations
    choices.clear();
    choices.reserve(1000000);
    choices.push_back({SimulationResult::GAME_ON, 0, 0, 0, gameState, nullptr});

    // hashes of simulations - for quick presence check
    //std::unordered_set<size_t> gameHashes;
    //gameHashes.insert(std::hash<game::Game>()(choices.front().game));

    // running priority queue of adding new simulations
    std::priority_queue<size_t, std::vector<size_t>, IndexComp> indexQueue;
    indexQueue.push(0);

    size_t counter = 0;
#ifdef _DEBUG
    const size_t counterFrequencyCheck = 1000;
#else
    const size_t counterFrequencyCheck = 10;
#endif
    for (bool timedOut = false; !indexQueue.empty() && !timedOut; )
    {
#ifdef _DEBUG
        {
            auto dupQueue = indexQueue;
            std::vector<double> score;
            for (; !dupQueue.empty(); dupQueue.pop())
                score.push_back(choices[dupQueue.top()].score);
            std::reverse(score.begin(), score.end());
            assert(std::is_sorted(score.cbegin(), score.cend()));
        }
#endif

        std::vector<size_t> batch;
        for (int i = 0; i < batchSize && !indexQueue.empty(); i++, indexQueue.pop())
            batch.push_back(indexQueue.top());
        
        for (const auto parentIndex: batch)
        {
        const Choice &parent = choices[parentIndex];
        if (parent.result != SimulationResult::GAME_ON)
            continue;

        if (!getShipById(parent.game.myShips_, shipId))
            continue;
        
        // search in moves
        for (auto &command : generateCommands(shipId, parent.game))
        {
            // check timeOut & depthLimit
            if (!depthLimit)
            {
                ++counter;
                if (counter % counterFrequencyCheck == 0)  // TODO: subject for optimization
                {
                    counter = 0;
                    if (std::chrono::high_resolution_clock::now() - startTime >= timeLimit)
                    {
                        timedOut = true;
                        break;
                    }
                }
            }
            
            if (depthLimit && parent.depth > depthLimit)
            {
                timedOut = true;
                break;
            }

            // update stats
            maxDepth = std::max(maxDepth, parent.depth + 1);

            // compute new simulation
            game::Game sim = parent.game;
            std::unordered_map<int, std::unique_ptr<game::ICommand>> commands;
            commands[shipId] = std::move(command);
            for (const auto &theirShip : sim.theirShips_)
                commands[theirShip.id_] = ai::naive(theirShip, sim);
            simulation::tick(sim, commands);
            SimulationResult result = gameResult(sim);

            // and place it in the collection, unless it's already there
            //size_t new_game_hash = std::hash<GameState>()(new_game);
            //if (gameHashes.count(new_game_hash) == 0)
            {
                size_t newIndex = choices.size();
                choices.push_back({result, ai::score(sim), parent.depth + 1, parentIndex, sim, std::move(commands[shipId])});
                indexQueue.push(newIndex);
                //gameHashes.insert(new_game_hash);

                if (!maxScoreIndex || choices.back().better(choices[maxScoreIndex]))
                    maxScoreIndex = newIndex;                
            }
        }

        }
    }

    LOG("searched " << maxDepth << " depth with " << choices.size() << " simulations");

    if (!maxScoreIndex)
    {
        LOG("search failed");
        return{};
    }

    LOG("best score " << choices[maxScoreIndex].score << " in state " << choices[maxScoreIndex].result);

#ifndef SUBMISSION
    std::string history;
    auto choiceToString = [](const Choice &choice)
    {
        std::stringstream ss;
        ss << choice.command->Encode() << " (" << choice.score << ")";
        return ss.str();
    };
    size_t index = maxScoreIndex;
    while (choices[index].parent_index)
    {
        history = choiceToString(choices[index]) + " -> " + history;
        index = choices[index].parent_index;
    }
    history = choiceToString(choices[index]) + " -> " + history;
    LOG(history);
#else
    size_t index = maxScoreIndex;
    while (choices[index].parent_index)
    {
        index = choices[index].parent_index;
    }
#endif

    return std::make_tuple(std::move(choices[index].command), choices[maxScoreIndex].score, choices[maxScoreIndex].result);
}

std::unordered_map<int, std::unique_ptr<game::ICommand>> ai::search(const game::Game & gameState, unsigned depthLimit /* = 0 */)
{
    std::unordered_map<int, std::unique_ptr<game::ICommand>> best;
    for (const auto &myShip : gameState.myShips_)
    {
        // TODO: account for already known friendly movement
        auto tacticalCommand = ai::tactical(myShip, gameState);
        if (tacticalCommand)
        {
            best[myShip.id_] = std::move(tacticalCommand);
        }
        else
        {
            auto res = searchImpl(myShip.id_, gameState, depthLimit, std::chrono::milliseconds(40 / gameState.myShips_.size()));
            best[myShip.id_] = std::move(std::get<0>(res));
        }
    }
    return best;
}
