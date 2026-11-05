#include "VirusEnemy.hpp"

namespace {

const float VIRUS_HP = 30.0f;
const float VIRUS_SPEED = 1.0f;
const int VIRUS_REWARD = 10;
const int VIRUS_DAMAGE_TO_ROOT = 10;

} // namespace

VirusEnemy::VirusEnemy(const std::vector<Vector2>& path)
    : Enemy(path, VIRUS_HP, VIRUS_SPEED, VIRUS_REWARD,
            VIRUS_DAMAGE_TO_ROOT) {}

char VirusEnemy::getSymbol() const {
    return 'v';
}
