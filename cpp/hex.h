#pragma once

namespace hex
{

    /**
    2d position on the hex grid
    */
    class Pos
    {
    public:
        explicit Pos(int x = 0, int y = 0)
            : x_(x)
            , y_(y)
        {
        }

        int x_, y_;
    };

    inline Pos operator - (const Pos &left, const Pos &right)
    {
        return Pos(left.x_ - right.x_, left.y_ - right.y_);
    }

    inline bool operator == (const Pos &left, const Pos &right)
    {
        return left.x_ == right.x_ && left.y_ == right.y_;
    }

    ///
    /// Distance between two cells, always integral
    ///
    int distance(const Pos &src, const Pos &dst);

    ///
    /// Angle between two cells, between 0 and 5 (inclusive)
    ///
    double angle(const Pos &src, const Pos &dst);

    ///
    /// Neighbor cell, in one of six directions
    ///
    Pos neighbor(const Pos &pos, int direction);

    ///
    /// Opposite direction
    ///
    int opposite(int direction);

    ///
    /// Port (left turn) direction
    ///
    int port(int direction);

    ///
    /// Starboard (right turn) direction
    ///
    int starboard(int direction);
}