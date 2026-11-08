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
     * @param startX Posição horizontal inicial.
     * @param startY Posição vertical inicial.
     * @param width Largura do inimigo.
     * @param height Altura do inimigo.
     * @param hp Pontos de vida iniciais.
     * @param speed Velocidade de deslocamento (unidades por segundo).
     * @param goldReward Recompensa em CPU concedida ao ser destruído.
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
     * @return true se a vida chegou a 0.
     */
    bool isDead() const;

    /**
     * @brief Verifica se o inimigo alcançou o final da rota de waypoints.
     * @return true se atingiu o último waypoint.
     */
    bool hasReachedEnd() const;

    /**
     * @brief Sobrecarga legada com parâmetro para compatibilidade transitória.
     * @param waypoints Rota a conferir.
     * @return true se atingiu o último ponto da rota informada.
     */
    bool hasReachedEnd(const std::vector<Vector2>& waypoints) const;

    /**
     * @brief Método legado de compatibilidade transitória.
     * @param waypoints Rota a percorrer.
     * @param deltaTime Tempo decorrido desde a última atualização (em segundos).
     */
    void moveAlongPath(const std::vector<Vector2>& waypoints, float deltaTime);

    /**
     * @brief Retorna os pontos de vida atuais.
     * @return Os pontos de vida atuais.
     */
    float getHp() const;

    /**
     * @brief Retorna os pontos de vida máximos.
     * @return Os pontos de vida máximos.
     */
    float getMaxHp() const;

    /**
     * @brief Retorna a velocidade, em células por segundo.
     * @return A velocidade, em células por segundo.
     */
    float getSpeed() const;

    /**
     * @brief Retorna a recompensa em CPU dada ao ser destruído.
     * @return A recompensa em CPU dada ao ser destruído.
     */
    int getReward() const;

    /**
     * @brief Retorna o dano causado ao /root ao chegar ao fim do caminho.
     * @return O dano causado ao /root ao chegar ao fim do caminho.
     */
    int getDamageToRoot() const;

    /**
     * @brief Retorna a distância já percorrida no caminho.
     * @return A distância já percorrida no caminho.
     */
    float getDistanceTravelled() const;

    /**
     * @brief Retorna o índice do último ponto do caminho já alcançado.
     * @return O índice do último ponto do caminho já alcançado.
     */
    std::size_t getWaypointIndex() const;

    /**
     * @brief Nome legado de getReward(), mantido por compatibilidade.
     * @return A recompensa em CPU dada ao ser destruído.
     */
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