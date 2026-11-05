#ifndef WORM_ENEMY_HPP
#define WORM_ENEMY_HPP

#include "Enemy.hpp"
#include "Vector2.hpp"

#include <vector>

/**
 * @brief Malware rápido e frágil.
 */
class WormEnemy : public Enemy {
public:
    /**
     * @brief Cria um worm que percorre o caminho informado.
     * @param path Caminho mantido pelo mapa durante a vida do inimigo.
     * @throws std::invalid_argument se o caminho estiver vazio.
     */
    explicit WormEnemy(const std::vector<Vector2>& path);

    /** @brief Retorna o símbolo 'w'. */
    char getSymbol() const;
};

#endif // WORM_ENEMY_HPP
