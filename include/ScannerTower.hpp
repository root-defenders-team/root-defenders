#ifndef SCANNER_TOWER_HPP
#define SCANNER_TOWER_HPP

#include <memory>
#include <vector>

#include "Cell.hpp"
#include "Tower.hpp"

class Enemy;
class Projectile;

/**
 * @brief Defesa do tipo Scanner: causa dano em área sem criar projéteis.
 *
 * Ataca simultaneamente todos os inimigos dentro do alcance e aumenta o seu
 * raio de alcance a cada evolução de nível.
 */
class ScannerTower : public Tower {
public:
    /**
     * @brief Constrói um Scanner na célula informada.
     * @param cell Célula onde a defesa é instalada.
     */
    explicit ScannerTower(const Cell& cell);

    /**
     * @brief Retorna o símbolo do Scanner no mapa.
     * @return O caractere 'S'.
     */
    char getSymbol() const override;

protected:
    /**
     * @brief Aplica dano direto em todos os inimigos ativos dentro do alcance.
     * @param target Alvo que disparou a ativação do pulso.
     * @param enemies Lista de inimigos ativos no mapa.
     * @param projectiles Lista de projéteis (não modificada pelo Scanner).
     */
    void fire(Enemy& target, const std::vector<Enemy*>& enemies,
              std::vector<std::unique_ptr<Projectile> >& projectiles) override;

    /**
     * @brief Aumenta o alcance em 0,5 célula a cada upgrade.
     */
    void applyUpgrade() override;
};

#endif // SCANNER_TOWER_HPP
