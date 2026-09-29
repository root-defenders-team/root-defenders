#ifndef MAP_HPP
#define MAP_HPP

#include <cstddef>
#include <stdexcept>
#include <vector>

/**
 * @brief Representa o cenário jogável e a rota dos inimigos.
 *
 * Map não é uma Entity: ele organiza o espaço do jogo, informa quais células
 * podem receber torres e fornece os pontos que os inimigos devem percorrer.
 * A implementação dos métodos será definida posteriormente no arquivo .cpp.
 */
class Map {
public:
    /**
     * @brief Ponto inteiro utilizado para posições e waypoints do mapa.
     */
    struct Point {
        /** @brief Coordenada horizontal do ponto. */
        int x;

        /** @brief Coordenada vertical do ponto. */
        int y;
    };

    /**
     * @brief Cria um mapa com a quantidade especificada de colunas e linhas.
     *
     * @param width Quantidade de colunas do mapa.
     * @param height Quantidade de linhas do mapa.
     * @throws std::invalid_argument caso width ou height seja zero.
     */
    Map(std::size_t width, std::size_t height);

    /** @brief Retorna a quantidade de colunas do mapa. */
    [[nodiscard]] std::size_t width() const noexcept;

    /** @brief Retorna a quantidade de linhas do mapa. */
    [[nodiscard]] std::size_t height() const noexcept;

    /**
     * @brief Verifica se uma coordenada pertence aos limites do mapa.
     * @param x Coordenada horizontal.
     * @param y Coordenada vertical.
     * @return true se a coordenada estiver dentro do mapa.
     */
    [[nodiscard]] bool isInside(int x, int y) const noexcept;

    /**
     * @brief Verifica se uma célula pode ser atravessada por inimigos.
     *
     * Uma célula transitável pertence ao mapa e não está bloqueada pelo
     * terreno. A ocupação por uma torre é uma informação independente.
     *
     * @return true quando a célula pode fazer parte da rota dos inimigos.
     */
    [[nodiscard]] bool isTraversable(int x, int y) const noexcept;

    /**
     * @brief Verifica se uma célula está bloqueada para a movimentação.
     * @return true quando a célula está fora do mapa ou não é transitável.
     */
    [[nodiscard]] bool isBlocked(int x, int y) const noexcept;

    /**
     * @brief Define se uma célula faz parte do terreno transitável.
     *
     * @param x Coordenada horizontal.
     * @param y Coordenada vertical.
     * @param traversable true para liberar a passagem; false para bloquear.
     * @return false quando a coordenada é inválida ou quando a célula ocupada
     *         por uma torre não pode ser bloqueada.
     */
    bool setTraversable(int x, int y, bool traversable) noexcept;

    /**
     * @brief Verifica se uma célula válida está livre para construção.
     * @return true quando a célula está dentro do mapa e não está ocupada.
     */
    [[nodiscard]] bool isCellFree(int x, int y) const noexcept;

    /**
     * @brief Marca uma célula como ocupada.
     * @return true se a célula foi ocupada; false se for inválida ou já estiver ocupada.
     */
    bool occupyCell(int x, int y) noexcept;

    /**
     * @brief Libera uma célula previamente ocupada.
     * @return true se a célula foi liberada; false se a coordenada for inválida ou já estiver livre.
     */
    bool releaseCell(int x, int y) noexcept;

    /**
     * @brief Adiciona um waypoint ao final da rota dos inimigos.
     * @param waypoint Ponto que será visitado pela rota.
     * @throws std::out_of_range caso o waypoint esteja fora do mapa.
     */
    void addWaypoint(Point waypoint);

    /**
     * @brief Retorna os waypoints na ordem em que devem ser percorridos.
     * @return Referência somente para leitura da rota do mapa.
     */
    [[nodiscard]] const std::vector<Point>& waypoints() const noexcept;

private:
    std::size_t width_;
    std::size_t height_;
    std::vector<std::vector<bool>> traversableCells_;
    std::vector<std::vector<bool>> occupiedCells_;
    std::vector<Point> waypoints_;
};

#endif // TOWER_DEFENSE_MAP_HPP
