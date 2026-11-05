#ifndef MAP_HPP
#define MAP_HPP

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "Cell.hpp"
#include "CellType.hpp"

/**
 * @brief Representa a grade do cenário e o caminho dos inimigos.
 *
 * Cada posição do mapa possui um tipo: livre para construção, caminho ou
 * obstáculo. O mapa também guarda quais células já receberam uma torre e os
 * waypoints que formam a rota dos inimigos.
 */
class Map {
public:
    /**
     * @brief Cria um mapa com a quantidade indicada de colunas e linhas.
     * @param width Quantidade de colunas.
     * @param height Quantidade de linhas.
     * @throws std::invalid_argument se width ou height for zero.
     */
    Map(std::size_t width, std::size_t height);

    /** @brief Retorna a quantidade de colunas do mapa. */
    std::size_t getWidth() const;

    /** @brief Retorna a quantidade de linhas do mapa. */
    std::size_t getHeight() const;

    /**
     * @brief Verifica se uma célula pertence aos limites do mapa.
     * @param cell Célula a verificar.
     * @return true se a célula estiver dentro do mapa.
     */
    bool isInside(const Cell& cell) const;

    /**
     * @brief Verifica se uma célula faz parte do caminho dos inimigos.
     * @param cell Célula a verificar.
     * @return true se o tipo da célula for Path.
     */
    bool isPath(const Cell& cell) const;

    /**
     * @brief Verifica se uma célula é um obstáculo ou está fora do mapa.
     * @param cell Célula a verificar.
     * @return true se a célula estiver bloqueada.
     */
    bool isBlocked(const Cell& cell) const;

    /**
     * @brief Verifica se uma célula pode receber uma torre.
     * @param cell Célula a verificar.
     * @return true se ela for Buildable, estiver dentro do mapa e livre.
     */
    bool isBuildable(const Cell& cell) const;

    /**
     * @brief Retorna o tipo de uma célula.
     * @param cell Célula consultada.
     * @return Tipo armazenado na célula.
     * @throws std::out_of_range se a célula estiver fora do mapa.
     */
    CellType getCellType(const Cell& cell) const;

    /**
     * @brief Altera o tipo de uma célula.
     * @param cell Célula a alterar.
     * @param type Novo tipo da célula.
     * @throws std::out_of_range se a célula estiver fora do mapa.
     */
    void setCellType(const Cell& cell, CellType type);

    /**
     * @brief Marca uma célula construível como ocupada por uma torre.
     * @param cell Célula a ocupar.
     * @return true se a ocupação foi realizada.
     */
    bool occupyCell(const Cell& cell);

    /**
     * @brief Libera uma célula que estava ocupada.
     * @param cell Célula a liberar.
     * @return true se a célula estava ocupada e foi liberada.
     */
    bool releaseCell(const Cell& cell);

    /**
     * @brief Adiciona uma célula ao final da rota dos inimigos.
     * @param waypoint Célula a adicionar à rota.
     * @throws std::out_of_range se o waypoint estiver fora do mapa.
     */
    void addWaypoint(const Cell& waypoint);

    /**
     * @brief Retorna os waypoints na ordem da rota.
     * @return Referência constante para os waypoints.
     */
    const std::vector<Cell>& getWaypoints() const;

    /**
     * @brief Retorna a rota dos inimigos em coordenadas contínuas.
     *
     * Cada waypoint adicionado também é armazenado como Vector2. A referência
     * permanece válida enquanto este Map existir e não for substituído.
     * @return Referência constante para a rota na ordem dos waypoints.
     */
    const std::vector<Vector2>& getPath() const;

    /**
     * @brief Informa se a rota possui origem e destino.
     * @return true quando existem pelo menos dois waypoints.
     */
    bool hasPath() const;

private:
    std::size_t width_;
    std::size_t height_;
    std::vector<std::vector<CellType> > cells_;
    std::vector<std::vector<bool> > occupiedCells_;
    std::vector<Cell> waypoints_;
    std::vector<Vector2> path_;
};

#endif // MAP_HPP
