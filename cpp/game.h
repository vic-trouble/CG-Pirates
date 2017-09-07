#pragma once

#include "hex.h"

#include <vector>

namespace game
{

    constexpr int MAP_WIDTH = 23;
    constexpr int MAP_HEIGHT = 21;
    constexpr int MAX_DISTANCE = 32;
    constexpr int COOLDOWN_CANNON = 2;
    constexpr int COOLDOWN_MINE = 5;
    constexpr int INITIAL_SHIP_HEALTH = 100;
    constexpr int MAX_SHIP_HEALTH = 100;
    constexpr int MAX_SHIP_SPEED = 2;
    constexpr int MIN_SHIPS = 1;
    constexpr int MIN_RUM_BARRELS = 10;
    constexpr int MAX_RUM_BARRELS = 26;
    constexpr int MIN_RUM_BARREL_VALUE = 10;
    constexpr int MAX_RUM_BARREL_VALUE = 20;
    constexpr int REWARD_RUM_BARREL_VALUE = 30;
    constexpr int MINE_VISIBILITY_RANGE = 5;
    constexpr int FIRE_DISTANCE_MAX = 10;
    constexpr int LOW_DAMAGE = 25;
    constexpr int HIGH_DAMAGE = 50;
    constexpr int MINE_DAMAGE = 25;
    constexpr int NEAR_MINE_DAMAGE = 10;

    /**
    Base class for game entities
    */
    class Entity
    {
    public:
        explicit Entity(const hex::Pos &pos)
            : id_(Entity::uniqueId_++)
            , pos_(pos)
        {
        }

        virtual ~Entity()
        {
        }

        int id_;
        hex::Pos pos_;

    private:
        static int uniqueId_;
    };

    /**
    Barrel of rum
    */
    class Barrel: public Entity
    {
    public:
        Barrel(const hex::Pos &pos, int health)
            : Entity(pos)
            , health_(health)
        {
        }

        int health_;
    };

    /**
    Ship
    */
    class Ship: public Entity
    {
    public:
        Ship(const hex::Pos &pos, int orient, int speed, int health, bool mine)
            : Entity(pos)
            , orient_(orient), speed_(speed)
            , health_(health), mine_(mine)
        {
        }

        hex::Pos bow() const;
        hex::Pos stern() const;

        hex::Pos newBow() const;
        hex::Pos newStern() const;

        bool dead() const;
        void hurt(int damage);
        void heal(int health);

        int orient_;
        int speed_;
        int health_;
        bool mine_;

        hex::Pos newPos_, newBow_, newStern_;
        int newOrient_ = 0;

        int cannonCooldown_ = 0;
        int mineCooldown_ = 0;
    };

    /**
    Cannonball
    */
    class Shot: public Entity
    {
    public:
        Shot(const hex::Pos &pos, int originId, int turnsToLand)
            : Entity(pos)
            , originId_(originId)
            , ticksToLand_(turnsToLand)
        {
        }

        int originId_;
        int ticksToLand_;
    };

    /**
    Sea mine
    */
    class Mine: public Entity
    {
    public:
        explicit Mine(const hex::Pos &pos)
            : Entity(pos)
        {
        }
    };

    /**
    Game state
    */
    class Game
    {
    public:
        int tick_ = 0;
        std::vector<Ship> myShips_;
        std::vector<Ship> theirShips_;
        std::vector<Barrel> barrels_;
        std::vector<Mine> mines_;
        std::vector<Shot> shots_;
    };

    ///
    /// Whether give pos is within the map boundaries
    ///
    bool valid(const hex::Pos &pos);
}
