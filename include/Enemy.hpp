#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Entity.hpp"
#include "Vector2.hpp"

#include <cstddef>
#include <vector>

/**
 * @brief Classe abstrata base para todos os malwares (inimigos) do jogo.
 *
 * Herda de Entity e gerencia pontos de vida (HP), velocidade de deslocamento,
 * recompensa em CPU, dano causado ao /root e rastreamento da rota de waypoints.
 */
class Enemy : public Entity {
public:
    /**
     * @brief Construtor da classe Enemy conforme o Documento Técnico (Doc 2, pág. 11).
     * @param path Referência para a rota de waypoints mantida pelo mapa.
     * @param maxHp Pontos de vida máximos iniciais.
     * @param speed Velocidade de deslocamento (unidades por segundo).
     * @param reward Recompensa em CPU concedida ao ser destruído.
     * @param damageToRoot Dano causado ao /root ao atingir o final do caminho.
     * @throws std::invalid_argument se path tiver menos de 2 pontos,
     *         ou se maxHp <= 0, speed <= 0, reward < 0 ou damageToRoot <= 0.
     */
    Enemy(const std::vector<Vector2>& path, float maxHp, float speed,
          int reward, int damageToRoot);

    /**
     * @brief Construtor legado mantido para compatibilidade transitória com testes de torres.
     */
    Enemy(float startX, float startY, float width, float height,
          float hp, float speed, int goldReward);

    virtual ~Enemy() = default;

    /**
     * @brief Atualiza a posição do inimigo ao longo do caminho.
     * Anda speed * deltaTime em direção ao próximo ponto e acumula distanceTravelled.
     * @param deltaTime Tempo decorrido desde a última atualização (em segundos).
     */
    void update(float deltaTime) override;

    /**
     * @brief Aplica dano ao inimigo (reduz seu HP).
     * Declarado como virtual para permitir blindagem polimórfica (ex: RootkitEnemy).
     * @param amount Quantidade de dano recebido.
     * @throws std::invalid_argument se amount for negativo.
     */
    virtual void takeDamage(float amount);

    /**
     * @brief Verifica se o inimigo foi derrotado (HP <= 0).
     */
    bool isDead() const;

    /**
     * @brief Verifica se o inimigo alcançou o final da rota de waypoints.
     * @return true se atingiu o último waypoint.
     */
    bool hasReachedEnd() const;

    /**
     * @brief Sobrecarga legada com parâmetro para compatibilidade transitória.
     */
    bool hasReachedEnd(const std::vector<Vector2>& waypoints) const;

    /**
     * @brief Método legado de compatibilidade transitória.
     */
    void moveAlongPath(const std::vector<Vector2>& waypoints, float deltaTime);

    float getHp() const;
    float getMaxHp() const;
    float getSpeed() const;
    int getReward() const;
    int getDamageToRoot() const;
    float getDistanceTravelled() const;
    std::size_t getWaypointIndex() const;

    // Alias legado de compatibilidade
    int getGoldReward() const;

protected:
    const std::vector<Vector2>& path; ///< Rota mantida pelo mapa (não proprietária).
    float hp;                         ///< Pontos de vida atuais.
    float maxHp;                      ///< Pontos de vida máximos.
    float speed;                      ///< Velocidade de movimento.
    int reward;                       ///< Recompensa em CPU.
    int damageToRoot;                 ///< Dano causado ao /root.
    std::size_t waypointIndex;        ///< Próximo waypoint a alcançar.
    float distanceTravelled;          ///< Distância percorrida acumulada.

private:
    std::vector<Vector2> fallbackPath_; ///< Usado quando chamado pelo construtor legado.
};

#endif // ENEMY_HPP