#ifndef FIREWALL_TOWER_HPP
#define FIREWALL_TOWER_HPP

#include <memory>
#include <vector>

#include "Cell.hpp"
#include "Tower.hpp"

class Enemy;
class Projectile;

/**
 * @brief Defesa do tipo Firewall: barata e com disparo rápido.
 *
 * Ataca um único inimigo lançando projéteis rápidos e reduz o seu intervalo
 * de disparo a cada evolução de nível.
 */
class FirewallTower : public Tower {
public:
    /**
     * @brief Constrói um Firewall na célula informada.
     * @param cell Célula onde a defesa é instalada.
     */
    explicit FirewallTower(const Cell& cell);

    /**
     * @brief Retorna o símbolo do Firewall no mapa.
     * @return O caractere 'F'.
     */
    char getSymbol() const override;

protected:
    /**
     * @brief Cria um projétil direcionado ao inimigo alvo.
     * @param target Inimigo escolhido para receber o disparo.
     * @param enemies Lista de inimigos ativos no mapa.
     * @param projectiles Lista onde o novo projétil é adicionado.
     */
    void fire(Enemy& target, const std::vector<Enemy*>& enemies,
              std::vector<std::unique_ptr<Projectile> >& projectiles) override;

    /**
     * @brief Reduz o intervalo de disparo em 0,1 s a cada upgrade.
     */
    void applyUpgrade() override;
};

#endif // FIREWALL_TOWER_HPP
