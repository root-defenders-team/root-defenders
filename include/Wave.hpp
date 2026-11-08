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
     * @param groups Grupos de inimigos, na ordem em que nascem.
     * @throws std::invalid_argument se não houver grupos, se algum count for
     *         menor ou igual a zero, ou se algum intervalo for negativo ou
     *         não finito.
     * @throws std::overflow_error se a quantidade total não couber em int.
     */
    explicit Wave(const std::vector<SpawnGroup>& groups);

    /**
     * @brief Avança o agendamento e retorna os tipos nascidos neste tick.
     * @param deltaTime Tempo do tick, em segundos.
     * @return Os tipos dos inimigos que nascem neste tick (pode ser vazio).
     * @throws std::invalid_argument se deltaTime for negativo ou não finito.
     */
    std::vector<EnemyType> update(float deltaTime);

    /**
     * @brief Informa se todos os grupos já foram gerados.
     * @return true se todos os grupos já foram gerados.
     */
    bool isFinishedSpawning() const;

    /**
     * @brief Retorna a quantidade total de inimigos configurados.
     * @return A quantidade total de inimigos configurados.
     */
    int getTotalEnemies() const;

    /** @brief Reinicia o agendamento da onda. */
    void reset();

private:
    /** @brief Passa para o próximo grupo e zera a contagem e o temporizador. */
    void advanceGroup();

    std::vector<SpawnGroup> groups; ///< Grupos da onda, na ordem do arquivo.
    std::size_t groupIndex;         ///< Grupo que está nascendo agora.
    int spawnedInGroup;             ///< Inimigos do grupo atual que já nasceram.
    double timer;                   ///< Tempo até o próximo nascimento, em segundos.
    int totalEnemies;               ///< Soma dos inimigos de todos os grupos.
};

#endif // WAVE_HPP
