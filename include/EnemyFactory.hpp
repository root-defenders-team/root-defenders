#ifndef ENEMY_FACTORY_HPP
#define ENEMY_FACTORY_HPP

#include <memory>
#include <string>
#include <vector>

#include "EnemyType.hpp"
#include "Vector2.hpp"

class Enemy;

/**
 * @brief Cria inimigos concretos sem expor suas classes ao restante do jogo.
 */
class EnemyFactory {
public:
    /**
     * @brief Cria um inimigo do tipo informado para percorrer o caminho.
     * @throws std::invalid_argument se o tipo não for conhecido ou o caminho
     *         estiver vazio.
     */
    static std::unique_ptr<Enemy> create(
        EnemyType type, const std::vector<Vector2>& path);

    /**
     * @brief Converte um nome de tipo para EnemyType.
     * @throws std::invalid_argument se o nome não for conhecido.
     */
    static EnemyType parseType(const std::string& name);
};

#endif // ENEMY_FACTORY_HPP
