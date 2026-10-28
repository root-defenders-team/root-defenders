#ifndef ROOTKIT_ENEMY_HPP
#define ROOTKIT_ENEMY_HPP

#include "Enemy.hpp"
#include "Vector2.hpp"

#include <vector>

/**
 * @brief Malware lento e resistente, com blindagem contra dano.
 */
class RootkitEnemy : public Enemy {
public:
    /**
     * @brief Cria um rootkit que percorre o caminho informado.
     * @param path Caminho mantido pelo mapa durante a vida do inimigo.
     * @throws std::invalid_argument se o caminho estiver vazio.
     */
    explicit RootkitEnemy(const std::vector<Vector2>& path);

    /** @brief Avança pelo caminho de acordo com deltaTime. */
    void update(float deltaTime) override;

    /** @brief Retorna o símbolo 'r'. */
    char getSymbol() const;

    /** @brief Aplica a blindagem, reduzindo o dano recebido em 3, mínimo 1. */
    void takeDamage(float amount);

    /** @brief Retorna o dano causado ao /root ao completar o caminho. */
    int getDamageToRoot() const;

private:
    const std::vector<Vector2>& path_; ///< Caminho não proprietário do mapa.
    int damageToRoot_;
};

#endif // ROOTKIT_ENEMY_HPP
