#include "VirusEnemy.hpp"

#include <stdexcept>

namespace {

const float ENEMY_SIZE = 0.6f;
const float VIRUS_HP = 30.0f;
const float VIRUS_SPEED = 1.0f;
const int VIRUS_REWARD = 10;
const int VIRUS_DAMAGE_TO_ROOT = 10;

} // namespace

VirusEnemy::VirusEnemy(const std::vector<Vector2>& path)
    : Enemy(path.empty() ? 0.0f : path.front().x,
            path.empty() ? 0.0f : path.front().y,
            ENEMY_SIZE, ENEMY_SIZE, VIRUS_HP, VIRUS_SPEED, VIRUS_REWARD),
      path_(path),
      damageToRoot_(VIRUS_DAMAGE_TO_ROOT) {
    if (path_.empty()) {
        throw std::invalid_argument("O caminho do vírus não pode estar vazio.");
    }
}

void VirusEnemy::update(float deltaTime) {
    if (isActive() && !isDead()) {
        moveAlongPath(path_, deltaTime);
    }
}

char VirusEnemy::getSymbol() const {
    return 'v';
}

int VirusEnemy::getDamageToRoot() const {
    return damageToRoot_;
}
