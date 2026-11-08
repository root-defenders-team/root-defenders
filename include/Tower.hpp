#ifndef TOWER_HPP
#define TOWER_HPP

#include <memory>
#include <vector>

#include "Cell.hpp"
#include "Entity.hpp"
#include "TargetPriority.hpp"

class Enemy;
class Projectile;

/**
 * @brief Classe abstrata base para todas as defesas (torres) do jogo.
 *
 * Herda de Entity e implementa a lógica comum de alcance, dano, intervalo de disparo,
 * recarga (cooldown), evolução (upgrade), valor de venda e mira (TargetPriority).
 */
class Tower : public Entity {
public:
    static const int MAX_LEVEL = 3; ///< Nível máximo que uma defesa pode atingir.

    /**
     * @brief Constrói uma defesa na célula indicada.
     * @param cell Célula da grade onde a torre está instalada.
     * @param range Alcance de ataque em unidades de distância.
     * @param damage Dano base por disparo.
     * @param fireInterval Intervalo mínimo entre disparos (em segundos).
     * @param cost Custo de compra da defesa em CPU.
     * @throws std::invalid_argument se range <= 0, damage <= 0, fireInterval <= 0 ou cost <= 0.
     */
    Tower(const Cell& cell, float range, float damage, float fireInterval, int cost);

    virtual ~Tower() = default;

    /**
     * @brief Atualiza a defesa a cada tick, reduzindo o cooldown de disparo.
     * @param deltaTime Tempo decorrido em segundos.
     */
    void update(float deltaTime) override;

    /**
     * @brief Tenta atacar os inimigos seguindo o padrão Template Method.
     *
     * 1. Se cooldown > 0, não faz nada.
     * 2. selectTarget escolhe o alvo no alcance conforme a TargetPriority.
     * 3. Se não houver alvo no alcance, retorna sem reiniciar o cooldown.
     * 4. Se houver alvo, chama o método protegido fire(...) e faz cooldown = fireInterval.
     *
     * @param enemies Lista de inimigos ativos no mapa.
     * @param projectiles Lista de projéteis ativos onde novos disparos são inseridos.
     */
    void tryAttack(const std::vector<Enemy*>& enemies,
                   std::vector<std::unique_ptr<Projectile> >& projectiles);

    /**
     * @brief Verifica se um inimigo está dentro do raio de alcance da defesa.
     * @param enemy Inimigo a verificar.
     * @return true se a distância for menor ou igual ao alcance.
     */
    bool isInRange(const Enemy& enemy) const;

    /**
     * @brief Verifica se a defesa pode receber evolução (nível < MAX_LEVEL).
     * @return true se puder evoluir.
     */
    bool canUpgrade() const;

    /**
     * @brief Realiza a evolução da defesa para o próximo nível.
     * @throws std::logic_error se a defesa já estiver no nível máximo.
     */
    void upgrade();

    /**
     * @brief Retorna o custo para evoluir do nível atual para o próximo.
     * Fórmula: (cost * level * 3) / 4.
     * @return Custo de evolução em CPU.
     */
    int getUpgradeCost() const;

    /**
     * @brief Retorna o valor de reembolso ao vender a defesa (50% de tudo investido).
     * @return Valor de venda em CPU.
     */
    int getSellValue() const;

    Cell getCell() const;
    float getRange() const;
    float getDamage() const;
    float getFireInterval() const;
    float getCooldown() const;
    int getCost() const;
    int getLevel() const;
    TargetPriority getTargetPriority() const;
    void setTargetPriority(TargetPriority newPriority);

protected:
    /**
     * @brief Método virtual puro chamado por tryAttack para disparar contra o alvo.
     * @param target Inimigo selecionado para ataque.
     * @param enemies Lista de inimigos ativos no mapa.
     * @param projectiles Lista de projéteis onde o novo projétil deve ser adicionado.
     */
    virtual void fire(Enemy& target, const std::vector<Enemy*>& enemies,
                      std::vector<std::unique_ptr<Projectile> >& projectiles) = 0;

    /**
     * @brief Método virtual puro para aplicar bônus específicos da subclasse ao evoluir.
     */
    virtual void applyUpgrade() = 0;

    void setRange(float newRange);
    void setDamage(float newDamage);
    void setFireInterval(float newFireInterval);

private:
    Enemy* selectTarget(const std::vector<Enemy*>& enemies) const;

    Cell cell;
    float range;
    float damage;
    float fireInterval;
    float cooldown;
    int cost;
    int level;
    int investedGold;
    TargetPriority targetPriority;
};

#endif // TOWER_HPP