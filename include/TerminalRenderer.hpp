#ifndef TERMINAL_RENDERER_HPP
#define TERMINAL_RENDERER_HPP

#include <ostream>

class GameManager;
class ScoreBoard;

/**
 * @brief Renderizador da interface de terminal em modo texto (ASCII).
 *
 * Responsável por desenhar o cabeçalho de status da partida, a grade do mapa,
 * a listagem de defesas e o ranking de pontuações.
 */
class TerminalRenderer {
public:
    /**
     * @brief Constrói o renderizador de terminal.
     * @param useColors Define se códigos ANSI de cor estão habilitados.
     */
    explicit TerminalRenderer(bool useColors = false);

    /**
     * @brief Limpa a tela e desenha o cabeçalho, o mapa e a legenda dos símbolos.
     * @param game Partida cujos dados serão desenhados.
     * @param out Fluxo de saída onde o quadro será escrito.
     */
    void drawFrame(const GameManager& game, std::ostream& out) const;

    /**
     * @brief Desenha a linha de status com CPU, integridade, onda e pontos.
     * @param game Partida cujos dados serão desenhados.
     * @param out Fluxo de saída onde o cabeçalho será escrito.
     */
    void drawHeader(const GameManager& game, std::ostream& out) const;

    /**
     * @brief Desenha a grade ASCII do mapa com terreno e entidades ativas.
     *
     * As colunas (x) aparecem numeradas em cima e as linhas (y) à esquerda, para o
     * jogador saber as coordenadas dos comandos. Com cores ligadas, cada símbolo
     * ganha uma cor ANSI.
     *
     * @param game Partida cujos dados serão desenhados.
     * @param out Fluxo de saída onde o mapa será escrito.
     */
    void drawMap(const GameManager& game, std::ostream& out) const;

    /**
     * @brief Desenha a lista de defesas instaladas na partida (comando ls).
     * @param game Partida cujas defesas serão listadas.
     * @param out Fluxo de saída onde a lista será escrita.
     */
    void drawTowerList(const GameManager& game, std::ostream& out) const;

    /**
     * @brief Desenha o ranking com as melhores pontuações.
     * @param board Quadro de recordes a ser exibido.
     * @param out Fluxo de saída onde o ranking será escrito.
     */
    void drawRanking(const ScoreBoard& board, std::ostream& out) const;

private:
    bool useColors; ///< Se true, desenha com cores ANSI.
};

#endif // TERMINAL_RENDERER_HPP
