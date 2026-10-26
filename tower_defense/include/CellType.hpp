/**
 * @file CellType.hpp
 * @brief Enum com os tipos de célula da grade do mapa.
 */

#ifndef CELL_TYPE_HPP
#define CELL_TYPE_HPP

/**
 * @brief Tipo de uma célula da grade do mapa.
 */
enum class CellType {
    Buildable, ///< Célula livre, aceita defesa.
    Path,      ///< Caminho dos malwares.
    Blocked    ///< Obstáculo: não é caminho e não aceita defesa.
};

#endif // CELL_TYPE_HPP
