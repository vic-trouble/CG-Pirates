#include "commands.h"
#include "cxxtweaks.h"
#include "game.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <memory>

void game::Fire::Execute(Game & gameState, Ship & myShip)
{
    assert(game::valid(pos_));
    assert(myShip.cannonCooldown_ == 0);

    int distance = hex::distance(myShip.bow(), pos_);
    if (distance > game::FIRE_DISTANCE_MAX)
        return;

    int travelTime = 1 + std::lround(distance / 3.0);
    gameState.shots_.emplace_back(pos_, myShip.id_, travelTime);
    myShip.cannonCooldown_ = game::COOLDOWN_CANNON;
}

void game::Move::Execute(Game & gameState, Ship & myShip)
{
    hex::Pos currentPosition = myShip.pos_;
    hex::Pos targetPosition = pos_;

    if (currentPosition == targetPosition)
        return game::SlowDown().Execute(gameState, myShip);

    double targetAngle, angleStraight, anglePort, angleStarboard, centerAngle, anglePortCenter, angleStarboardCenter;

    std::unique_ptr<ICommand> subCommand;
    switch (myShip.speed_)
    {
        case 2:
            return game::SlowDown().Execute(gameState, myShip);

        case 1:
        {
            // Suppose we've moved first
            currentPosition = hex::neighbor(currentPosition, myShip.orient_);
            if (!game::valid(currentPosition))
                return game::SlowDown().Execute(gameState, myShip);

            // Target reached at next turn
            if (currentPosition == targetPosition)
                return;

            // For each neighbor cell, find the closest to target
            targetAngle = hex::angle(currentPosition, targetPosition);
            angleStraight = std::min(std::abs(myShip.orient_ - targetAngle), 6 - std::abs(myShip.orient_ - targetAngle));
            anglePort = std::min(std::abs((myShip.orient_ + 1) - targetAngle), std::abs((myShip.orient_ - 5) - targetAngle));
            angleStarboard = std::min(std::abs((myShip.orient_ + 5) - targetAngle), std::abs((myShip.orient_ - 1) - targetAngle));

            centerAngle = hex::angle(currentPosition, hex::Pos(game::MAP_WIDTH / 2, game::MAP_HEIGHT / 2));
            anglePortCenter = std::min(std::abs((myShip.orient_ + 1) - centerAngle), std::abs((myShip.orient_ - 5) - centerAngle));
            angleStarboardCenter = std::min(std::abs((myShip.orient_ + 5) - centerAngle), std::abs((myShip.orient_ - 1) - centerAngle));

            // Next to target with bad angle, slow down then rotate (avoid to turn around the target!)
            if (hex::distance(currentPosition, targetPosition) == 1 && angleStraight > 1.5)
                return game::SlowDown().Execute(gameState, myShip);

            int distanceMin = std::numeric_limits<int>::max();

            // Test forward
            hex::Pos nextPosition = hex::neighbor(currentPosition, myShip.orient_);
            if (game::valid(nextPosition))
                distanceMin = hex::distance(nextPosition, targetPosition);

            // Test port
            nextPosition = hex::neighbor(currentPosition, hex::port(myShip.orient_));
            if (game::valid(nextPosition))
            {
                int distance = hex::distance(nextPosition, targetPosition);
                if (distance < distanceMin || (distance == distanceMin && anglePort < angleStraight - 0.5))
                {
                    distanceMin = distance;
                    subCommand = std::make_unique<TurnLeft>();
                }
            }

            // Test starboard
            nextPosition = hex::neighbor(currentPosition, hex::starboard(myShip.orient_));
            if (game::valid(nextPosition))
            {
                int distance = hex::distance(nextPosition, targetPosition);
                if (distance < distanceMin
                    || (distance == distanceMin && angleStarboard < anglePort - 0.5 && subCommand)
                    || (distance == distanceMin && angleStarboard < angleStraight - 0.5 && !subCommand)
                    || (distance == distanceMin && subCommand && angleStarboard == anglePort
                        && angleStarboardCenter < anglePortCenter)
                    || (distance == distanceMin && subCommand && angleStarboard == anglePort
                        && angleStarboardCenter == anglePortCenter && (myShip.orient_ == 1 || myShip.orient_ == 4)))
                {
                    distanceMin = distance;
                    subCommand = std::make_unique<TurnRight>();
                }
            }
            break;
        }

        case 0:
            // Rotate ship towards target
            targetAngle = hex::angle(currentPosition, targetPosition);
            angleStraight = std::min(std::abs(myShip.orient_ - targetAngle), 6 - std::abs(myShip.orient_ - targetAngle));
            anglePort = std::min(std::abs((myShip.orient_ + 1) - targetAngle), std::abs((myShip.orient_ - 5) - targetAngle));
            angleStarboard = std::min(std::abs((myShip.orient_ + 5) - targetAngle), std::abs((myShip.orient_ - 1) - targetAngle));

            centerAngle = hex::angle(currentPosition, hex::Pos(game::MAP_WIDTH / 2, game::MAP_HEIGHT / 2));
            anglePortCenter = std::min(std::abs((myShip.orient_ + 1) - centerAngle), std::abs((myShip.orient_ - 5) - centerAngle));
            angleStarboardCenter = std::min(std::abs((myShip.orient_ + 5) - centerAngle), std::abs((myShip.orient_ - 1) - centerAngle));

            hex::Pos forwardPosition = hex::neighbor(currentPosition, myShip.orient_);

            if (anglePort <= angleStarboard)
                subCommand = std::make_unique<TurnLeft>();

            if (angleStarboard < anglePort || (angleStarboard == anglePort && angleStarboardCenter < anglePortCenter)
                || (angleStarboard == anglePort && angleStarboardCenter == anglePortCenter && (myShip.orient_ == 1 || myShip.orient_ == 4)))
                subCommand = std::make_unique<TurnRight>();

            if (game::valid(forwardPosition) && angleStraight <= anglePort && angleStraight <= angleStarboard)
                subCommand = std::make_unique<SpeedUp>();
            break;
    }

    if (subCommand)
        subCommand->Execute(gameState, myShip);
}

void game::PlantMine::Execute(Game & gameState, Ship & myShip)
{
    assert(myShip.mineCooldown_ == 0);

    hex::Pos target = hex::neighbor(myShip.stern(), hex::opposite(myShip.orient_));
    if (!game::valid(target))
        return;

    for (const auto &barrel : gameState.barrels_)
        if (barrel.pos_ == target)
            return;

    for (const auto &mine : gameState.mines_)
        if (mine.pos_ == target)
            return;

    for (const auto &ship : gameState.myShips_)
        if (ship.pos_ == target || ship.bow() == target || ship.stern() == target)
            return;

    myShip.mineCooldown_ = game::COOLDOWN_MINE;
    gameState.mines_.emplace_back(target);
}

void game::SlowDown::Execute(Game & gameState, Ship & myShip)
{
    if (myShip.speed_ > 0)
        myShip.speed_--;
}

void game::SpeedUp::Execute(Game & gameState, Ship & myShip)
{
    if (myShip.speed_ < game::MAX_SHIP_SPEED)
        myShip.speed_++;
}

void game::TurnLeft::Execute(Game & gameState, Ship & myShip)
{
    myShip.newOrient_ = hex::port(myShip.orient_);
}

void game::TurnRight::Execute(Game & gameState, Ship & myShip)
{
    myShip.newOrient_ = hex::starboard(myShip.orient_);
}
