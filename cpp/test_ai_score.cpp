/*

TODO:
- enemy closer to the barrel => worse?
- when no more barrels, and my health is higher than theirs => flee
- when no more barrels, and my health is lower than theirs => pursue
- penalize for pursuing same barrel?

*/

#include <gtest/gtest.h>

#include "ai_score.h"
#include "proxies.h"

using namespace test;

TEST(Score, Should_Be_More_When_LessEnemyShips)
{
    // arrange
    game::Game g;
    g.theirShips_.push_back(ShipProxy().Their());
    g.theirShips_.push_back(ShipProxy().Their());

    game::Game lessEnemy;
    lessEnemy.theirShips_.push_back(ShipProxy().Their());

    // act
    auto score = ai::score(g), scoreLessEnemy = ai::score(lessEnemy);

    // assert
    EXPECT_GT(scoreLessEnemy, score);
}

TEST(Score, Should_Be_More_When_MoreMyShips)
{
    // arrange
    game::Game g;
    g.myShips_.push_back(ShipProxy().Mine());

    game::Game moreMine;
    moreMine.myShips_.push_back(ShipProxy().Mine());
    moreMine.myShips_.push_back(ShipProxy().Mine());

    // act
    auto score = ai::score(g), scoreMoreMine = ai::score(moreMine);

    // assert
    EXPECT_GT(scoreMoreMine, score);
}

TEST(Score, Should_Be_More_When_WeHaveMoreHealth)
{
    // arrange
    game::Game g;
    g.myShips_.push_back(ShipProxy().Mine().Health(10));

    game::Game moreHealth;
    moreHealth.myShips_.push_back(ShipProxy().Mine().Health(20));

    // act
    auto score = ai::score(g), scoreMoreHealth = ai::score(moreHealth);

    // assert
    EXPECT_GT(scoreMoreHealth, score);
}

TEST(Score, Should_Be_Less_When_TheyHaveMoreHealth)
{
    // arrange
    game::Game g;
    g.theirShips_.push_back(ShipProxy().Their().Health(10));

    game::Game moreHealth;
    moreHealth.theirShips_.push_back(ShipProxy().Their().Health(20));

    // act
    auto score = ai::score(g), scoreMoreHealth = ai::score(moreHealth);

    // assert
    EXPECT_LT(scoreMoreHealth, score);
}

TEST(Score, Should_Be_More_When_WeCloserToBarrel)
{
    // arrange
    game::Game g;
    g.barrels_.push_back(BarrelProxy().Pos(15, 15));
    g.myShips_.push_back(ShipProxy().Mine().Pos(10, 10));

    game::Game closerToBarrel;
    closerToBarrel.barrels_.push_back(BarrelProxy().Pos(15, 15));
    closerToBarrel.myShips_.push_back(ShipProxy().Mine().Pos(11, 11));

    // act
    auto score = ai::score(g), scoreCloserToBarell = ai::score(closerToBarrel);

    // assert
    EXPECT_GT(scoreCloserToBarell, score);
}

TEST(Score, Should_Be_More_When_AngleToBarrelIsLess)
{
    // arrange
    game::Game g;
    g.barrels_.push_back(BarrelProxy().Pos(10, 15));
    g.myShips_.push_back(ShipProxy().Mine().Pos(10, 10).Orientation(0));

    game::Game betterAngle;
    betterAngle.barrels_.push_back(BarrelProxy().Pos(10, 15));
    betterAngle.myShips_.push_back(ShipProxy().Mine().Pos(10, 10).Orientation(5));

    // act
    auto score = ai::score(g), scoreBetterAngle = ai::score(betterAngle);

    // assert
    EXPECT_GT(scoreBetterAngle, score);
}

TEST(Score, Should_Be_More_When_MySpeedIsHigher)
{
    // arrange
    game::Game g;
    g.myShips_.push_back(ShipProxy().Mine().Speed(1));

    game::Game higherSpeed;
    higherSpeed.myShips_.push_back(ShipProxy().Mine().Speed(2));

    // act
    auto score = ai::score(g), scoreHigherSpeed = ai::score(higherSpeed);

    // assert
    EXPECT_GT(scoreHigherSpeed, score);
}

TEST(Score, ShouldNot_CountShip_When_SheIsDead)
{
    // arrange
    game::Game empty;
    game::Game g;
    g.myShips_.push_back(ShipProxy().Mine().Health(0));

    // act
    auto score = ai::score(g), scoreEmpty = ai::score(empty);

    // assert
    EXPECT_EQ(scoreEmpty, score);
}

TEST(Score, ShouldBe_More_When_InTheFinalWeAreHealthierAndFartherFromEnemy)
{
    // arrange
    game::Game g;
    g.theirShips_.push_back(ShipProxy().Their().Pos(1, 1).Health(50));
    g.myShips_.push_back(ShipProxy().Mine().Pos(10, 1).Health(100));

    game::Game gFarther;
    gFarther.theirShips_.push_back(ShipProxy().Their().Pos(1, 1).Health(50));
    gFarther.myShips_.push_back(ShipProxy().Mine().Pos(11, 1).Health(100));

    // act
    auto score = ai::score(g), scoreFarther = ai::score(gFarther);
    
    // assert
    EXPECT_GT(scoreFarther, score);
}

TEST(Score, ShouldBe_More_When_InTheFinalWeAreWeakerButCloserToEnemy)
{
    // arrange
    game::Game g;
    g.theirShips_.push_back(ShipProxy().Their().Pos(1, 1).Health(100));
    g.myShips_.push_back(ShipProxy().Mine().Pos(10, 1).Health(50));
    
    game::Game gCloser;
    gCloser.theirShips_.push_back(ShipProxy().Their().Pos(1, 1).Health(100));
    gCloser.myShips_.push_back(ShipProxy().Mine().Pos(9, 1).Health(50));
    
    // act
    auto score = ai::score(g), scoreCloser = ai::score(gCloser);
    
    // assert
    EXPECT_GT(scoreCloser, score);
}

TEST(Score, ShouldBe_Less_When_StuckAtTheBorder)
{
    // arrange
    game::Game g;
    g.myShips_.push_back(ShipProxy().Mine().Pos(0, 0).Orientation(0));

    game::Game gStuck;
    gStuck.myShips_.push_back(ShipProxy().Mine().Pos(0, 0).Orientation(-3));
    
    // act
    auto score = ai::score(g), scoreStuck = ai::score(gStuck);
    
    // assert
    EXPECT_LT(scoreStuck, score);
}
