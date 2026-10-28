#ifndef TOWER_HPP
#define TOWER_HPP

#include "Entity.hpp"
#include "TargetPriority.hpp"
#include <vector>

class Enemy;
class Projectile;

/**
 * @brief Classe abstrata base para todas as torres defensivas do jogo.
 * 
 * Herda de Entity e gerencia os atributos e comportamentos comuns de combate,
 * como alcance, dano, cadência de tiro, recarga, mira e melhorias.
 */
class Tower : public Entity {
protected:
    float _range;                   ///< Raio de alcance da torre.
    float _damage;                  ///< Dano base infligido pelos disparos.
    float _fireRate;                ///< Cadência de tiro (disparos por segundo).
    float _fireCooldown;            ///< Temporizador interno para controlar o intervalo entre tiros.
    int _cost;                      ///< Custo em ouro para construir ou melhorar a torre.
    int _level;                     ///< Nível atual da torre.
    TargetPriority _targetPriority; ///< Critério de prioridade para seleção de alvos.

public:
    /**
     * @brief Construtor da classe Tower.
     * @param startX Posição X inicial no mapa.
     * @param startY Posição Y inicial no mapa.
     * @param width Largura da torre.
     * @param height Altura da torre.
     * @param range Alcance de ataque.
     * @param damage Dano dos disparos.
     * @param fireRate Cadência de tiros (disparos por segundo).
     * @param cost Custo da torre.
     * @param priority Critério de seleção de alvo (padrão: First).
     */
    Tower(float startX, float startY, float width, float height, 
          float range, float damage, float fireRate, int cost,
          TargetPriority priority = TargetPriority::First);

    /**
     * @brief Destrutor virtual da torre.
     */
    virtual ~Tower() = default;

    /**
     * @brief Atualiza temporizadores internos da torre (ex: recarga).
     * @param deltaTime Tempo decorrido desde a última atualização (em segundos).
     */
    virtual void update(float deltaTime) override;

    /**
     * @brief Método virtual puro para o ataque polimórfico.
     * @param enemies Referência para o vetor de inimigos ativos no mapa.
     * @param projectiles Referência para o vetor de projéteis ativos no jogo.
     * @param deltaTime Tempo decorrido desde o último frame.
     */
    virtual void attack(std::vector<Enemy*>& enemies, std::vector<Projectile*>& projectiles, float deltaTime) = 0;

    /**
     * @brief Realiza a melhoria (upgrade) da torre, aumentando seus atributos.
     */
    virtual void upgrade();

    /**
     * @brief Verifica se um inimigo está dentro do raio de alcance da torre.
     * @param enemy Inimigo a ser verificado.
     * @return true se a distância for menor ou igual ao alcance.
     */
    bool isInRange(const Enemy& enemy) const;

    /**
     * @brief Verifica se a torre está pronta para disparar (cooldown zerado).
     */
    bool canShoot() const;

    /**
     * @brief Atualiza o temporizador de recarga.
     * @param deltaTime Tempo decorrido em segundos.
     */
    void updateCooldown(float deltaTime);

    /**
     * @brief Reinicia o temporizador de recarga após um disparo.
     */
    void resetCooldown();

    /**
     * @brief Seleciona o melhor alvo dentre os inimigos ativos segundo o critério de prioridade.
     * @param enemies Lista de ponteiros para inimigos ativos no mapa.
     * @return Ponteiro para o inimigo escolhido ou nullptr se nenhum estiver no alcance.
     */
    Enemy* selectTarget(const std::vector<Enemy*>& enemies) const;

    // Getters e Setters públicos (Encapsulamento)
    float getRange() const;
    float getDamage() const;
    int getCost() const;
    int getLevel() const;
    float getFireRate() const;
    float getCooldown() const;
    TargetPriority getTargetPriority() const;
    void setTargetPriority(TargetPriority priority);
};

#endif // TOWER_HPP