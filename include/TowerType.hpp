/**
 * @file TowerType.hpp
 * @brief Enum com os tipos de defesa.
 */

#ifndef TOWER_TYPE_HPP
#define TOWER_TYPE_HPP

/**
 * @brief Tipos de defesa existentes no jogo.
 */
enum class TowerType {
    Firewall,  ///< Disparos rápidos e fracos, alcance curto.
    Antivirus, ///< Disparos fortes e lentos, alcance longo.
    Scanner    ///< Dano em área, sem projétil.
};

#endif // TOWER_TYPE_HPP
