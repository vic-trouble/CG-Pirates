#include <gtest/gtest.h>

#include "ai_naive.h"
#include "proxies.h"

using namespace test;

TEST(Naive, Should_AttackTheirShip_When_NearBy)
{
    // arrange
    game::Ship myShip = ShipProxy().Mine().Pos(1, 1);

    game::Game gameState;
    gameState.myShips_.push_back(myShip);
    gameState.theirShips_.push_back(ShipProxy().Their().Pos(5, 1));

    // act
    auto command = ai::naive(myShip, gameState);

    // assert
    game::Fire *fireCommand = dynamic_cast<game::Fire *>(command.get());
    ASSERT_TRUE(fireCommand);
    EXPECT_EQ(hex::Pos(5, 1), fireCommand->pos_);
}

TEST(Naive, Should_AttackMyShip_When_InvokedForEnemy)
{
    // arrange
    game::Ship theirShip = ShipProxy().Their().Pos(1, 1);
    
    game::Game gameState;
    gameState.myShips_.push_back(ShipProxy().Mine().Pos(5, 1));
    gameState.theirShips_.push_back(theirShip);

    // act
    auto command = ai::naive(theirShip, gameState);

    // assert
    game::Fire *fireCommand = dynamic_cast<game::Fire *>(command.get());
    ASSERT_TRUE(fireCommand);
    EXPECT_EQ(hex::Pos(5, 1), fireCommand->pos_);
}

TEST(Naive, Should_PlantMine_When_EnemyBehind)
{
    // arrange
    game::Ship myShip = ShipProxy().Mine().Pos(8, 1).Orientation(0).Speed(1).CannonCooldown(1);
    
    game::Game g;
    g.myShips_.push_back(myShip);
    g.theirShips_.push_back(ShipProxy().Their().Pos(4, 1).Orientation(0).Speed(2));
    
    // act
    auto command = ai::naive(myShip, g);
    
    // assert
    EXPECT_TRUE(dynamic_cast<game::PlantMine *>(command.get()));
}
