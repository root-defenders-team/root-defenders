#include "RootkitEnemy.hpp"

#include <algorithm>
#include <stdexcept>

namespace {

const float ENEMY_SIZE = 0.6f;
const float ROOTKIT_HP = 120.0f;
const float ROOTKIT_SPEED = 0.6f;
const int ROOTKIT_REWARD = 25;
const int ROOTKIT_DAMAGE_TO_ROOT = 30;
const float ROOTKIT_ARMOR = 3.0f;
const float MINIMUM_ARMOR_DAMAGE = 1.0f;

} // namespace

RootkitEnemy::RootkitEnemy(const std::vector<Vector2>& path)
    : Enemy(path.empty() ? 0.0f : path.front().x,
            path.empty() ? 0.0f : path.front().y,
            ENEMY_SIZE, ENEMY_SIZE, ROOTKIT_HP, ROOTKIT_SPEED,
            ROOTKIT_REWARD),
      path_(path),
      damageToRoot_(ROOTKIT_DAMAGE_TO_ROOT) {
    if (path_.empty()) {
        throw std::invalid_argument("O caminho do rootkit não pode estar vazio.");
    }
}

void RootkitEnemy::update(float deltaTime) {
    if (isActive() && !isDead()) {
        moveAlongPath(path_, deltaTime);
    }
}

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

int RootkitEnemy::getDamageToRoot() const {
    return damageToRoot_;
}
