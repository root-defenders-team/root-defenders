#ifndef CELL_HPP
#define CELL_HPP

#include "Vector2.hpp"

/**
 * @brief Coordenada inteira de uma célula da grade do mapa.
 *
 * x é a coluna e y é a linha, ambas começando em 0. É um agregado:
 * aceita inicialização como Cell c = {3, 4};.
 */
struct Cell {
    int x; ///< Coluna da célula.
    int y; ///< Linha da célula.

    /**
     * @brief Compara duas células.
     * @param other Célula comparada.
     * @return true se as duas tiverem a mesma coluna e a mesma linha.
     */
    bool operator==(const Cell& other) const;

    /**
     * @brief Converte a célula para posição contínua no mapa.
     * @return Vector2 com as mesmas coordenadas, em float.
     */
    Vector2 toVector2() const;
};

#endif // CELL_HPP
