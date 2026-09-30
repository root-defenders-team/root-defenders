#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Entity.hpp"
#include <vector>

/**
 * @brief Classe abstrata base para todos os inimigos do jogo.
 * 
 * Herda de Entity e gerencia pontos de vida (HP), velocidade de deslocamento,
 * recompensas e o rastreamento da rota de waypoints.
 */
class Enemy : public Entity {
protected:
    float _hp;            ///< Pontos de vida atuais do inimigo.
    float _maxHp;         ///< Pontos de vida máximos.
    float _speed;         ///< Velocidade de deslocamento no mapa.
    int _goldReward;      ///< Recompensa em ouro concedida ao ser derrotado.
    size_t _waypointIndex; ///< Índice atual do waypoint que o inimigo está perseguindo.

public:
    /**
     * @brief Construtor da classe Enemy.
     * @param startX Posição X inicial.
     * @param startY Posição Y inicial.
     * @param width Largura do inimigo.
     * @param height Altura do inimigo.
     * @param hp Pontos de vida iniciais.
     * @param speed Velocidade de movimento.
     * @param goldReward Ouro obtido ao derrotar.
     */
    Enemy(float startX, float startY, float width, float height, 
          float hp, float speed, int goldReward);

    /**
     * @brief Destrutor virtual do inimigo.
     */
    virtual ~Enemy() = default;

    /**
     * @brief Atualiza a posição do inimigo seguindo os waypoints do mapa.
     * @param deltaTime Tempo decorrido desde a última atualização.
     */
    virtual void update(float deltaTime) override = 0;

    /**
     * @brief Renderiza o inimigo na janela do jogo.
     * @param window Referência para a janela da SFML.
     */
    virtual void render(sf::RenderWindow& window) override = 0;

    /**
     * @brief Aplica dano ao inimigo (reduz seu HP).
     * @param amount Quantidade de dano recebido.
     */
    void takeDamage(float amount);

    /**
     * @brief Verifica se o inimigo foi derrotado (HP <= 0).
     * @return true se estiver morto, false caso contrário.
     */
    bool isDead() const;

    /**
     * @brief Obtém a recompensa em ouro por derrotar o inimigo.
     * @return Quantidade de ouro.
     */
    int getGoldReward() const;

    /**
     * @brief Verifica se o inimigo chegou ao final da rota de waypoints.
     * @param waypoints Lista de posições que formam o caminho.
     * @return true se atingiu o destino final.
     */
    bool hasReachedEnd(const std::vector<sf::Vector2f>& waypoints) const;
};

#endif // ENEMY_HPP