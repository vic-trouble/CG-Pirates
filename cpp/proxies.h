#pragma once

#include "game.h"

namespace test
{
    /**
    Test-friendly wrapper around Ship
    */
    class ShipProxy
    {
    public:
        ShipProxy()
            : ship_(hex::Pos(), 0, 0, 100, true)
        {
        }

        operator game::Ship() const
        {
            return ship_;
        }

        ShipProxy &Mine()
        {
            ship_.mine_ = true;
            return *this;
        }

        ShipProxy &Their()
        {
            ship_.mine_ = false;
            return *this;
        }

        ShipProxy &Health(int health)
        {
            ship_.health_ = health;
            return *this;
        }

        ShipProxy &Pos(int x, int y)
        {
            ship_.pos_ = hex::Pos(x, y);
            return *this;
        }

        ShipProxy &Orientation(int orientation)
        {
            ship_.orient_ = orientation;
            return *this;
        }

        ShipProxy &Speed(int speed)
        {
            ship_.speed_ = speed;
            return *this;
        }

        ShipProxy &CannonCooldown(int cooldown)
        {
            ship_.cannonCooldown_ = cooldown;
            return *this;
        }

        ShipProxy &MineCooldown(int cooldown)
        {
            ship_.mineCooldown_ = cooldown;
            return *this;
        }

    private:
        game::Ship ship_;
    };

    /**
    Test-friendly wrapper around Barrel
    */
    class BarrelProxy
    {
    public:
        BarrelProxy()
            : barrel_(hex::Pos(), 10)
        {
        }

        operator game::Barrel() const
        {
            return barrel_;
        }

        BarrelProxy &Pos(int x, int y)
        {
            barrel_.pos_ = hex::Pos(x, y);
            return *this;
        }

        BarrelProxy &Health(int health)
        {
            barrel_.health_ = health;
            return *this;
        }

    private:
        game::Barrel barrel_;
    };

    /**
    Test-friendly wrapper around Mine
    */
    class MineProxy
    {
    public:
        MineProxy()
            : mine_(hex::Pos())
        {
        }

        operator game::Mine() const
        {
            return mine_;
        }

        MineProxy &Pos(int x, int y)
        {
            mine_.pos_ = hex::Pos(x, y);
            return *this;
        }

    private:
        game::Mine mine_;
    };

    /**
    Test-friendly wrapper around Shot
    */
    class ShotProxy
    {
    public:
        ShotProxy()
            : shot_(hex::Pos(), 0, 1)
        {
        }

        operator game::Shot() const
        {
            return shot_;
        }

        ShotProxy &Pos(int x, int y)
        {
            shot_.pos_ = hex::Pos(x, y);
            return *this;
        }

        ShotProxy &TicksToLand(int ticks)
        {
            shot_.ticksToLand_ = ticks;
            return *this;
        }

    private:
        game::Shot shot_;
    };
}
