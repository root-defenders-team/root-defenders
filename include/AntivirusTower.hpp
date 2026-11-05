#ifndef ANTIVIRUS_TOWER_HPP
#define ANTIVIRUS_TOWER_HPP

#include <memory>
#include <vector>

#include "Cell.hpp"
#include "Tower.hpp"

class Enemy;
class Projectile;

/**
 * @brief Defesa do tipo Antivírus: alto dano e longo alcance.
 *
 * Ataca um único inimigo lançando projéteis pesados e aumenta o seu dano
 * a cada evolução de nível.
 */
class AntivirusTower : public Tower {
public:
    /**
     * @brief Constrói um Antivírus na célula informada.
     * @param cell Célula onde a defesa é instalada.
     */
    explicit AntivirusTower(const Cell& cell);

    /**
     * @brief Retorna o símbolo do Antivírus no mapa.
     * @return O caractere 'A'.
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
     * @brief Aumenta o dano em 15 pontos a cada upgrade.
     */
    void applyUpgrade() override;
};

#endif // ANTIVIRUS_TOWER_HPP
