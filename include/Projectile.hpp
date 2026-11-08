#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "Entity.hpp"

class Enemy;

/**
 * @brief Projétil que persegue um inimigo e aplica dano ao alcançá-lo.
 *
 * O projétil não é dono do alvo. O gerenciador deve manter o inimigo vivo
 * enquanto o projétil estiver ativo.
 */
class Projectile : public Entity {
public:
    /**
     * @brief Cria um projétil direcionado a um inimigo.
     * @param startPosition Posição inicial do projétil.
     * @param target Inimigo perseguido; sua posse não é transferida.
     * @param speed Velocidade em unidades por segundo.
     * @param damage Dano causado no impacto.
     * @throws std::invalid_argument se speed ou damage não forem positivos.
     */
    Projectile(const Vector2& startPosition, Enemy& target,
               float speed, float damage);

    /**
     * @brief Persegue o alvo; ao alcançá-lo, aplica dano e se desativa.
     * @param deltaTime Tempo desde a última atualização, em segundos.
     */
    void update(float deltaTime) override;

    /**
     * @brief Retorna o símbolo usado para representar o projétil no mapa.
     * @return O caractere '*'.
     */
    char getSymbol() const;

    /** @brief Aplica dano ao alvo e desativa o projétil. */
    void hit();

    /**
     * @brief Informa se o projétil atingiu o alvo.
     * @return true se o projétil atingiu o alvo.
     */
    bool hasImpacted() const;

    /**
     * @brief Retorna o alvo sem transferir sua posse.
     * @return O alvo sem transferir sua posse.
     */
    const Enemy* getTarget() const;

    /**
     * @brief Retorna a velocidade do projétil.
     * @return A velocidade do projétil.
     */
    float getSpeed() const;

    /**
     * @brief Retorna o dano causado no impacto.
     * @return O dano causado no impacto.
     */
    float getDamage() const;

    /**
     * @brief Define a velocidade do projétil.
     * @param newSpeed Nova velocidade, em células por segundo.
     * @throws std::invalid_argument se newSpeed não for positivo.
     */
    void setSpeed(float newSpeed);

    /**
     * @brief Define o dano do projétil.
     * @param newDamage Novo dano causado no impacto.
     * @throws std::invalid_argument se newDamage não for positivo.
     */
    void setDamage(float newDamage);

private:
    Enemy* target; ///< Alvo não proprietário, mantido pelo gerenciador.
    float speed; ///< Velocidade em unidades por segundo.
    float damage; ///< Dano causado no impacto.
    bool impacted; ///< Indica se o projétil já atingiu o alvo.
};

#endif // PROJECTILE_HPP
