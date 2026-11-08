#ifndef SPAWN_GROUP_HPP
#define SPAWN_GROUP_HPP

#include "EnemyType.hpp"

/**
 * @brief Configuração de um grupo de inimigos de uma onda.
 */
struct SpawnGroup {
    EnemyType type; ///< Tipo dos inimigos do grupo.
    int count;      ///< Quantos inimigos o grupo tem.
    float interval; ///< Segundos entre um nascimento e o próximo.
};

#endif // SPAWN_GROUP_HPP
