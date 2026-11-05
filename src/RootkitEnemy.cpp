#include "RootkitEnemy.hpp"

#include <algorithm>
#include <stdexcept>

namespace {

const float ROOTKIT_HP = 120.0f;
const float ROOTKIT_SPEED = 0.6f;
const int ROOTKIT_REWARD = 30;
const int ROOTKIT_DAMAGE_TO_ROOT = 25;
const float ROOTKIT_ARMOR = 3.0f;
const float MINIMUM_ARMOR_DAMAGE = 1.0f;

} // namespace

RootkitEnemy::RootkitEnemy(const std::vector<Vector2>& path)
    : Enemy(path, ROOTKIT_HP, ROOTKIT_SPEED, ROOTKIT_REWARD,
            ROOTKIT_DAMAGE_TO_ROOT) {}

char RootkitEnemy::getSymbol() const {
    return 'r';
}

void RootkitEnemy::takeDamage(float amount) {
    if (amount < 0.0f) {
        throw std::invalid_argument("Enemy damage amount cannot be negative");
    }
    if (amount == 0.0f) {
        return;
    }

    Enemy::takeDamage(std::max(MINIMUM_ARMOR_DAMAGE, amount - ROOTKIT_ARMOR));
}
