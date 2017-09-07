#include "ai_score.h"

#include <algorithm>
#include <cmath>

namespace
{
    double score(const game::Game &gameState, bool forEnemy = false)
    {
        // omitted for not violating contest rules
    }
}

double ai::score(const game::Game & gameState)
{
    return ::score(gameState) - ::score(gameState, true);
}
