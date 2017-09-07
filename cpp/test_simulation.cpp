#include <gtest/gtest.h>

#include "cxxtweaks.h"
#include "game.h"
#include "proxies.h"
#include "simulation.h"

using namespace test;

TEST(Simulation, Should_AcceptEmptyCommands)
{
    // arrange
    game::Game gameState;
    gameState.myShips_.push_back(ShipProxy().Mine().Pos(1, 1));

    std::unordered_map<int, std::unique_ptr<game::ICommand>> empty;

    // act, assert
    EXPECT_NO_THROW(simulation::tick(gameState, empty));
}

TEST(Simulation, Should_DenyRotationCollision)
{
    // arrange
    game::Ship still = ShipProxy().Mine().Pos(2, 2).Orientation(1);
    game::Ship moving = ShipProxy().Mine().Pos(3, 4).Orientation(3).Speed(1);

    game::Game gameState;
    gameState.myShips_ = {still, moving};

    std::unordered_map<int, std::unique_ptr<game::ICommand>> commands;
    commands[moving.id_] = std::make_unique<game::TurnRight>();

    // act
    simulation::tick(gameState, commands);

    // assert
    EXPECT_EQ(hex::Pos(2, 4), gameState.myShips_.back().pos_);
    EXPECT_EQ(3, gameState.myShips_.back().orient_);
    EXPECT_EQ(0, gameState.myShips_.back().speed_);
}

TEST(Simulation, Should_CreateLootRum_When_ShipIsSunk)
{
    // arrange
    game::Game gameState;
    gameState.myShips_.push_back(ShipProxy().Pos(1, 1).Health(40).Speed(0));
    gameState.shots_.push_back(ShotProxy().Pos(1, 1).TicksToLand(1));

    // act
    simulation::tick(gameState, {});

    // assert
    ASSERT_EQ(1, gameState.barrels_.size());
    EXPECT_EQ(hex::Pos(1, 1), gameState.barrels_.front().pos_);
    EXPECT_EQ(30, gameState.barrels_.front().health_);
}

TEST(Simulation, Should_DecrementCannonCooldown)
{
    // arrange
    game::Game gameState;
    gameState.myShips_.push_back(ShipProxy().CannonCooldown(2));

    // act
    simulation::tick(gameState, {});

    // assert
    EXPECT_EQ(1, gameState.myShips_.back().cannonCooldown_);
}

TEST(Simulation, Should_DecrementMineCooldown)
{
    // arrange
    game::Game gameState;
    gameState.myShips_.push_back(ShipProxy().MineCooldown(2));

    // act
    simulation::tick(gameState, {});

    // assert
    EXPECT_EQ(1, gameState.myShips_.back().mineCooldown_);
}

TEST(Simulation, Should_IncrementTick)
{
    // arrange
    game::Game gameState;
    gameState.tick_ = 10;

    // act
    simulation::tick(gameState, {});

    // assert
    EXPECT_EQ(11, gameState.tick_);
}
