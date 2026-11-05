#include "Wave.hpp"

#include <cmath>
#include <limits>
#include <stdexcept>

Wave::Wave(const std::vector<SpawnGroup>& groups)
    : groups(groups),
      groupIndex(0),
      spawnedInGroup(0),
      timer(0.0),
      totalEnemies(0) {
    if (groups.empty()) {
        throw std::invalid_argument("Uma onda precisa ter pelo menos um grupo.");
    }

    for (std::size_t i = 0; i < groups.size(); ++i) {
        if (groups[i].count <= 0) {
            throw std::invalid_argument(
                "A quantidade de inimigos do grupo deve ser positiva.");
        }
        if (!std::isfinite(groups[i].interval) || groups[i].interval < 0.0f) {
            throw std::invalid_argument(
                "O intervalo do grupo deve ser finito e não negativo.");
        }
        if (groups[i].count > std::numeric_limits<int>::max() - totalEnemies) {
            throw std::overflow_error(
                "A quantidade total de inimigos excede o limite suportado.");
        }

        totalEnemies += groups[i].count;
    }
}

std::vector<EnemyType> Wave::update(float deltaTime) {
    if (!std::isfinite(deltaTime) || deltaTime < 0.0f) {
        throw std::invalid_argument(
            "O tempo do tick deve ser finito e não negativo.");
    }

    std::vector<EnemyType> born;
    if (isFinishedSpawning()) {
        return born;
    }

    const SpawnGroup& group = groups[groupIndex];
    if (spawnedInGroup == 0) {
        born.push_back(group.type);
        ++spawnedInGroup;

        if (spawnedInGroup == group.count) {
            advanceGroup();
        } else if (group.interval == 0.0f) {
            while (spawnedInGroup < group.count) {
                born.push_back(group.type);
                ++spawnedInGroup;
            }
            advanceGroup();
        }
        return born;
    }

    timer += static_cast<double>(deltaTime);
    while (!isFinishedSpawning()) {
        const SpawnGroup& currentGroup = groups[groupIndex];
        if (timer < static_cast<double>(currentGroup.interval)) {
            break;
        }

        timer -= static_cast<double>(currentGroup.interval);
        born.push_back(currentGroup.type);
        ++spawnedInGroup;

        if (spawnedInGroup == currentGroup.count) {
            advanceGroup();
            break;
        }
    }

    return born;
}

bool Wave::isFinishedSpawning() const {
    return groupIndex >= groups.size();
}

int Wave::getTotalEnemies() const {
    return totalEnemies;
}

void Wave::reset() {
    groupIndex = 0;
    spawnedInGroup = 0;
    timer = 0.0;
}

void Wave::advanceGroup() {
    ++groupIndex;
    spawnedInGroup = 0;
    timer = 0.0;
}
