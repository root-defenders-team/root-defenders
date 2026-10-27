#ifndef TOWER_HPP
#define TOWER_HPP

#include "Entity.hpp"
#include "Enemy.hpp"
#include "Projectile.hpp"
#include <vector>

/**
 * @brief Classe abstrata base para todas as torres defensivas do jogo.
 * 
 * Herda de Entity e gerencia os atributos e comportamentos comuns de combate,
 * como alcance, dano, cadência de tiro e melhorias.
 */
class Tower : public Entity {
protected:
    float _range;         ///< Raio de alcance da torre.
    float _damage;        ///< Dano base infligido pelos disparos.
    float _fireRate;      ///< Cadência de tiro (disparos por segundo).
    float _fireCooldown;  ///< Temporizador interno para controlar o intervalo entre tiros.
    int _cost;            ///< Custo em ouro para construir ou melhorar a torre.
    int _level;           ///< Nível atual da torre.

public:
    /**
     * @brief Construtor da classe Tower.
     * @param startX Posição X inicial no mapa.
     * @param startY Posição Y inicial no mapa.
     * @param width Largura da torre.
     * @param height Altura da torre.
     * @param range Alcance de ataque.
     * @param damage Dano dos disparos.
     * @param fireRate Cadência de tiros.
     * @param cost Custo da torre.
     */
    Tower(float startX, float startY, float width, float height, 
          float range, float damage, float fireRate, int cost);

    /**
     * @brief Destrutor virtual da torre.
     */
    virtual ~Tower() = default;

    /**
     * @brief Método virtual puro para o ataque polimórfico.
     * 
     * Cada tipo específico de torre (ex: Arqueiro, Canhão, Mago) implementará 
     * sua própria lógica de escolha de alvo e criação de projéteis.
     * 
     * @param enemies Referência para o vetor de inimigos ativos no mapa.
     * @param projectiles Referência para o vetor de projéteis ativos no jogo.
     * @param deltaTime Tempo decorrido desde o último frame.
     */
    virtual void attack(std::vector<Enemy*>& enemies, std::vector<Projectile*>& projectiles, float deltaTime) = 0;

    /**
     * @brief Realiza a melhoria (upgrade) da torre, aumentando seus atributos.
     */
    virtual void upgrade();

    // Getters públicos (Encapsulamento)
    float getRange() const;
    float getDamage() const;
    int getCost() const;
    int getLevel() const;
};

#endif // TOWER_HPP