#ifndef WAVE_HPP
#define WAVE_HPP

#include <cstddef>
#include <vector>

#include "SpawnGroup.hpp"

/**
 * @brief Agenda o nascimento sequencial dos grupos de uma onda.
 */
class Wave {
public:
    /**
     * @brief Cria uma onda a partir dos grupos informados.
     * @throws std::invalid_argument se não houver grupos, se algum count for
     *         menor ou igual a zero, ou se algum intervalo for negativo ou
     *         não finito.
     * @throws std::overflow_error se a quantidade total não couber em int.
     */
    explicit Wave(const std::vector<SpawnGroup>& groups);

    /**
     * @brief Avança o agendamento e retorna os tipos nascidos neste tick.
     * @throws std::invalid_argument se deltaTime for negativo ou não finito.
     */
    std::vector<EnemyType> update(float deltaTime);

    /** @brief Informa se todos os grupos já foram gerados. */
    bool isFinishedSpawning() const;

    /** @brief Retorna a quantidade total de inimigos configurados. */
    int getTotalEnemies() const;

    /** @brief Reinicia o agendamento da onda. */
    void reset();

private:
    void advanceGroup();

    std::vector<SpawnGroup> groups;
    std::size_t groupIndex;
    int spawnedInGroup;
    double timer;
    int totalEnemies;
};

#endif // WAVE_HPP
