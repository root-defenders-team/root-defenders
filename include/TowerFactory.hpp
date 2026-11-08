#ifndef TOWER_FACTORY_HPP
#define TOWER_FACTORY_HPP

#include <memory>
#include <string>

#include "Cell.hpp"
#include "Tower.hpp"
#include "TowerType.hpp"

/**
 * @brief Fábrica utilitária para criação de defesas a partir do tipo ou nome.
 *
 * Centraliza o conhecimento das subclasses concretas de Tower (FirewallTower,
 * AntivirusTower e ScannerTower).
 */
class TowerFactory {
public:
    /**
     * @brief Cria a subclasse de defesa correspondente ao tipo informado.
     * @param type Tipo da defesa a ser criada.
     * @param cell Célula onde a defesa será posicionada.
     * @return Ponteiro inteligente dono da nova defesa.
     * @throws std::invalid_argument se o tipo informado for inválido.
     */
    static std::unique_ptr<Tower> create(TowerType type, const Cell& cell);

    /**
     * @brief Converte o nome textual da defesa para o valor de TowerType.
     * @param name Nome em minúsculas ("firewall", "antivirus" ou "scanner").
     * @return Valor correspondente do enum TowerType.
     * @throws std::invalid_argument se o nome for desconhecido.
     */
    static TowerType parseType(const std::string& name);
};

#endif // TOWER_FACTORY_HPP
