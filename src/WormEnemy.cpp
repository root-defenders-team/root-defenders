#include "WormEnemy.hpp"

namespace {

const float WORM_HP = 15.0f;
const float WORM_SPEED = 2.0f;
const int WORM_REWARD = 8;
const int WORM_DAMAGE_TO_ROOT = 5;

} // namespace

WormEnemy::WormEnemy(const std::vector<Vector2>& path)
    : Enemy(path, WORM_HP, WORM_SPEED, WORM_REWARD,
            WORM_DAMAGE_TO_ROOT) {}

char WormEnemy::getSymbol() const {
    return 'w';
}
