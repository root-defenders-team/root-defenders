#ifndef SPAWN_GROUP_HPP
#define SPAWN_GROUP_HPP

#include "EnemyType.hpp"

/**
 * @brief Configuração de um grupo de inimigos de uma onda.
 */
struct SpawnGroup {
    EnemyType type;
    int count;
    float interval;
};

#endif // SPAWN_GROUP_HPP
