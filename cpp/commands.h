#pragma once

#include "game.h"
#include "hex.h"

#include <string>

namespace game
{

    /**
    Command interface
    */
    struct ICommand
    {
        virtual ~ICommand() {}

        ///
        /// Encode command for passing to the referee
        ///
        virtual std::string Encode() const = 0;

        ///
        /// Apply command's effects
        ///
        virtual void Execute(Game &gameState, Ship &myShip) = 0;
    };

    /**
    Move command
    */
    class Move: public ICommand
    {
    public:
        explicit Move(const hex::Pos &pos)
            : pos_(pos)
        {
        }

        std::string Encode() const override
        {
            return "MOVE " + std::to_string(pos_.x_) + " " + std::to_string(pos_.y_);
        }

        void Execute(Game &gameState, Ship &myShip) override;

    private:
        hex::Pos pos_;
    };

    /**
    No-op
    */
    class Wait: public ICommand
    {
    public:
        std::string Encode() const override
        {
            return "WAIT";
        }

        void Execute(Game &gameState, Ship &myShip) override
        {
        }
    };

    /**
    Fire a cannonball
    */
    class Fire: public ICommand
    {
    public:
        explicit Fire(const hex::Pos &pos)
            : pos_(pos)
        {
        }

        std::string Encode() const override
        {
            return "FIRE " + std::to_string(pos_.x_) + " " + std::to_string(pos_.y_);
        }

        void Execute(Game &gameState, Ship &myShip) override;

        hex::Pos pos_;
    };

    /**
    Plant a mine
    */
    class PlantMine: public ICommand
    {
    public:
        std::string Encode() const override
        {
            return "MINE";
        }

        void Execute(Game &gameState, Ship &myShip) override;
    };

    /**
    Slow down
    */
    class SlowDown: public ICommand
    {
    public:
        std::string Encode() const override
        {
            return "SLOWER";
        }

        void Execute(Game &gameState, Ship &myShip) override;
    };

    /**
    Speed up
    */
    class SpeedUp: public ICommand
    {
    public:
        std::string Encode() const override
        {
            return "FASTER";
        }

        void Execute(Game &gameState, Ship &myShip) override;
    };

    /**
    Turn left
    */
    class TurnLeft: public ICommand
    {
    public:
        std::string Encode() const override
        {
            return "PORT";
        }

        void Execute(Game &gameState, Ship &myShip) override;
    };

    /**
    Turn right
    */
    class TurnRight: public ICommand
    {
    public:
        std::string Encode() const override
        {
            return "STARBOARD";
        }

        void Execute(Game &gameState, Ship &myShip) override;
    };

}
