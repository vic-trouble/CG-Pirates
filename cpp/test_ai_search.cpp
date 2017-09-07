#include <gtest/gtest.h>

#include "ai_search.h"
#include "assertions.h"
#include "game.h"
#include "io.h"
#include "proxies.h"
#include "simulation.h"

using namespace test;

TEST(Search, Should_SpeedUpTowardsTheBarell_When_ItIsStraightAhead)
{
    // arrange
    game::Ship ship = ShipProxy().Mine().Pos(0, 0).Orientation(0);
    game::Game g;
    g.myShips_.push_back(ship);
    g.barrels_.push_back(BarrelProxy().Pos(10, 0));

    // act
    auto commands = ai::search(g);

    // assert
    EXPECT_TRUE(isCommand<game::SpeedUp>(commands[ship.id_]));
}

TEST(Search, ShouldNot_Stuck_WhenBarrelUnreachable)
{
    // arrange
    game::Ship ship = ShipProxy().Mine().Pos(0, 0).Orientation(5);
    game::Game g;
    g.myShips_.push_back(ship);
    g.barrels_.push_back(BarrelProxy().Pos(1, 1));

    // act
    auto commands = ai::search(g);

    // assert
    EXPECT_FALSE(isCommand<game::Wait>(commands[ship.id_]));
}

TEST(Search, ShouldNot_RushIntoMine_When_BarrelBehindIt)
{
    // arrange
    game::Ship ship = ShipProxy().Mine().Pos(0, 0).Orientation(0).Speed(2).Health(20);
    game::Game g;
    g.myShips_.push_back(ship);
    g.theirShips_.push_back(ShipProxy().Their().Pos(10, 10));
    g.mines_.push_back(MineProxy().Pos(4, 0));
    g.barrels_.push_back(BarrelProxy().Pos(6, 0));

    // act
    auto commands = ai::search(g, 4);

    // assert
    EXPECT_TRUE(isCommand<game::TurnRight>(commands[ship.id_]));
}

TEST(Search, Should_NegoitateConflictOverBarrel)
{
    // arrange
    game::Ship left = ShipProxy().Mine().Pos(1, 3).Orientation(0).Speed(0);
    game::Ship right = ShipProxy().Mine().Pos(7, 3).Orientation(3).Speed(0);

    game::Game g;
    g.myShips_ = {left, right};
    g.barrels_.push_back(BarrelProxy().Pos(4, 3));

    // act
    auto commands = ai::search(g);

    // assert
    EXPECT_FALSE(isCommand<game::Wait>(commands[left.id_]));
    EXPECT_FALSE(isCommand<game::Wait>(commands[right.id_]));
}

TEST(Search, Should_ShootStillEnemy_When_NearBy)
{
    // arrange
    game::Ship myShip = ShipProxy().Mine().Pos(1, 1);
    
    game::Game g;
    g.myShips_.push_back(myShip);
    g.theirShips_.push_back(ShipProxy().Their().Pos(2, 2));

    // act
    auto commands = ai::search(g, 2);

    // assert
    EXPECT_TRUE(isFireCommand(hex::Pos(2, 2), commands[myShip.id_]));
}

TEST(Search, Should_Stop_When_ShipDies)
{
    // arrange
    game::Game gameState;
    gameState.myShips_.push_back(ShipProxy().Pos(1, 1).Health(40).Speed(0));
    gameState.shots_.push_back(ShotProxy().Pos(1, 1).TicksToLand(1));

    // act, should not crash
    EXPECT_NO_THROW(ai::search(gameState));
}

TEST(Search, Should_PlantMine_When_EnemyBehind)
{
    // arrange
    game::Ship myShip = ShipProxy().Mine().Pos(8, 1).Orientation(0).Speed(1).CannonCooldown(1);

    game::Game g;
    g.myShips_.push_back(myShip);
    g.theirShips_.push_back(ShipProxy().Their().Pos(4, 1).Orientation(0).Speed(2));

    // act
    auto commands = ai::search(g, 2);

    // assert
    EXPECT_TRUE(isCommand<game::PlantMine>(commands[myShip.id_]));
}

TEST(Search, Should_ShootMovingEnemy_When_OneShotAway)
{
    // arrange
    game::Ship myShip = ShipProxy().Mine().Pos(3, 1).Speed(1);

    game::Game g;
    g.myShips_.push_back(myShip);
    g.theirShips_.push_back(ShipProxy().Their().Pos(2, 2).Speed(1));

    // act
    auto commands = ai::search(g, 2);

    // assert
    EXPECT_TRUE(isFireCommand(hex::Pos(4, 2), commands[myShip.id_]));
}

TEST(Search, Should_ShootMovingEnemy_When_HeIsMovingTowardsBarrel)
{
    // arrange
    game::Ship myShip = ShipProxy().Mine().Pos(3, 1).Speed(1);

    game::Game g;
    g.myShips_.push_back(myShip);
    g.theirShips_.push_back(ShipProxy().Their().Pos(1, 2).Speed(0));
    g.barrels_.push_back(BarrelProxy().Pos(8, 2));

    // act
    auto commands = ai::search(g, 2);

    // assert
    EXPECT_TRUE(isFireCommand(hex::Pos(3, 2), commands[myShip.id_]));
}

