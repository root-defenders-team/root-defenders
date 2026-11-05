#ifndef VIRUS_ENEMY_HPP
#define VIRUS_ENEMY_HPP

#include "Enemy.hpp"
#include "Vector2.hpp"

#include <vector>

/**
 * @brief Malware equilibrado em vida e velocidade.
 */
class VirusEnemy : public Enemy {
public:
    /**
     * @brief Cria um vírus que percorre o caminho informado.
     * @param path Caminho mantido pelo mapa durante a vida do inimigo.
     * @throws std::invalid_argument se o caminho estiver vazio.
     */
    explicit VirusEnemy(const std::vector<Vector2>& path);

    /** @brief Retorna o símbolo 'v'. */
    char getSymbol() const;
};

#endif // VIRUS_ENEMY_HPP
