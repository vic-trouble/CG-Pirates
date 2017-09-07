#include "cxxtweaks.h"
#include "hex.h"

#include <cmath>
#include <cstdlib>

namespace
{

    /**
    3d position on the hex grid
    */
    class Cubic
    {
    public:
        explicit Cubic(const hex::Pos &pos)
        {
            x_ = pos.x_ - (pos.y_ - (pos.y_ & 1)) / 2;
            z_ = pos.y_;
            y_ = -(x_ + z_);
        }

        int x_, y_, z_;
    };

    int distance(const Cubic &src, const Cubic &dst)
    {
        return (std::abs(src.x_ - dst.x_) + std::abs(src.y_ - dst.y_) + std::abs(src.z_ - dst.z_)) / 2;
    }

}

int hex::distance(const Pos & src, const Pos & dst)
{
    return distance(Cubic(src), Cubic(dst));
}

double hex::angle(const Pos & src, const Pos & dst)
{
    double dy = (dst.y_ - src.y_) * std::sqrt(3) / 2;
    double dx = dst.x_ - src.x_ + ((src.y_ - dst.y_) & 1) * 0.5;
    double angle = -std::atan2(dy, dx) * 3 / std::PI;
    if (angle < 0)
    {
        angle += 6;
    }
    else if (angle >= 6)
    {
        angle -= 6;
    }
    return angle;
}

hex::Pos hex::neighbor(const Pos & pos, int direction)
{
    static int DIRECTIONS_EVEN[][2] = {{1, 0}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}};
    static int DIRECTIONS_ODD[][2] = {{1, 0}, {1, -1}, {0, -1}, {-1, 0}, {0, 1}, {1, 1}};

    if (pos.y_ % 2 == 1)
        return Pos(pos.x_ + DIRECTIONS_ODD[direction][0], pos.y_ + DIRECTIONS_ODD[direction][1]);
    else
        return Pos(pos.x_ + DIRECTIONS_EVEN[direction][0], pos.y_ + DIRECTIONS_EVEN[direction][1]);
}

int hex::opposite(int direction)
{
    return (direction + 3) % 6;
}

int hex::port(int direction)
{
    return (direction + 1) % 6;
}

int hex::starboard(int direction)
{
    return (direction + 5) % 6;
}
