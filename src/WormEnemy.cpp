#include "WormEnemy.hpp"

#include <stdexcept>

namespace {

const float ENEMY_SIZE = 0.6f;
const float WORM_HP = 15.0f;
const float WORM_SPEED = 2.0f;
const int WORM_REWARD = 5;
const int WORM_DAMAGE_TO_ROOT = 8;

} // namespace

WormEnemy::WormEnemy(const std::vector<Vector2>& path)
    : Enemy(path.empty() ? 0.0f : path.front().x,
            path.empty() ? 0.0f : path.front().y,
            ENEMY_SIZE, ENEMY_SIZE, WORM_HP, WORM_SPEED, WORM_REWARD),
      path_(path),
      damageToRoot_(WORM_DAMAGE_TO_ROOT) {
    if (path_.empty()) {
        throw std::invalid_argument("O caminho do worm não pode estar vazio.");
    }
}

void WormEnemy::update(float deltaTime) {
    if (isActive() && !isDead()) {
        moveAlongPath(path_, deltaTime);
    }
}

char WormEnemy::getSymbol() const {
    return 'w';
}

int WormEnemy::getDamageToRoot() const {
    return damageToRoot_;
}
