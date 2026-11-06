#ifndef SHELL_HPP
#define SHELL_HPP

#include <istream>
#include <ostream>
#include <string>

#include "Command.hpp"
#include "TerminalRenderer.hpp"

class GameManager;
class ScoreBoard;

/**
 * @brief Laço principal do ROOT DEFENSE: conversa com o jogador pelo terminal.
 *
 * Mostra o prompt, lê cada linha digitada, transforma em comando pelo CommandParser
 * e pede ao GameManager a ação correspondente. Durante uma onda, anima o mapa até
 * ela acabar; no fim da partida, mostra o resultado e grava o ranking.
 *
 * Toda entrada errada vira uma mensagem "erro: motivo" e o prompt aparece de novo:
 * nenhum comando derruba o jogo. Lê de um std::istream e escreve num std::ostream,
 * por isso os testes usam std::stringstream no lugar do teclado e da tela.
 */
class Shell {
public:
    /**
     * @brief Cria o Shell de uma partida.
     *
     * @param game Partida controlada pelo Shell; não é copiada.
     * @param scoreBoard Ranking já carregado; recebe a pontuação no fim da partida.
     * @param input De onde os comandos são lidos (std::cin no jogo).
     * @param output Onde as mensagens e o mapa são escritos (std::cout no jogo).
     */
    Shell(GameManager& game, ScoreBoard& scoreBoard, std::istream& input,
          std::ostream& output);

    /**
     * @brief Lê e executa comandos até exit, fim da entrada, vitória ou derrota.
     *
     * Antes do primeiro comando, avisa se o ranking tinha linhas corrompidas. Se a
     * partida terminar em vitória ou derrota, mostra o resultado e o ranking.
     */
    void run();

    /**
     * @brief Executa um único comando digitado.
     *
     * Linha vazia não faz nada. Comando mal escrito ou ação impossível mostram
     * "erro: motivo" e não lançam exceção.
     *
     * @param line Linha digitada pelo jogador.
     * @return false se o comando foi exit; true nos outros casos.
     */
    bool execute(const std::string& line);

    /**
     * @brief Define a pausa entre dois quadros da animação da onda.
     * @param milliseconds Pausa em milissegundos: 100 no jogo, 0 nos testes.
     * @throws std::invalid_argument se milliseconds for negativo.
     */
    void setFrameDelay(int milliseconds);

private:
    /**
     * @brief Comando install: confere a célula e a CPU e instala a defesa.
     * @param command Comando com o tipo e a célula da defesa.
     */
    void handleInstall(const Command& command);

    /**
     * @brief Comando upgrade: evolui a defesa da célula em um nível.
     * @param command Comando com a célula da defesa.
     */
    void handleUpgrade(const Command& command);

    /**
     * @brief Comando rm: remove a defesa da célula e devolve parte da CPU.
     * @param command Comando com a célula da defesa.
     */
    void handleRemove(const Command& command);

    /**
     * @brief Comando target: muda a prioridade de alvo da defesa da célula.
     * @param command Comando com a célula e a prioridade.
     */
    void handleTarget(const Command& command);

    /**
     * @brief Comando man: mostra a descrição e os atributos de uma defesa ou malware.
     * @param command Comando com o nome da defesa ou do malware.
     */
    void handleManual(const Command& command);

    /**
     * @brief Avança a onda atual tick a tick, desenhando o mapa, até ela acabar.
     */
    void runWave();

    /**
     * @brief Mostra vitória ou derrota, registra a pontuação e mostra o ranking.
     *
     * Se a pontuação entrar no top 5, pede o nome do jogador. Uma falha ao gravar
     * o ranking vira aviso, sem interromper o fim da partida.
     */
    void finishGame();

    GameManager& game;          ///< Partida controlada pelo Shell.
    ScoreBoard& scoreBoard;     ///< Ranking da partida.
    std::istream& input;        ///< Entrada dos comandos.
    std::ostream& output;       ///< Saída das mensagens e do mapa.
    TerminalRenderer renderer;  ///< Desenha o cabeçalho, o mapa, as defesas e o ranking.
    int frameDelay;             ///< Pausa entre quadros da onda, em milissegundos.
};

#endif // SHELL_HPP
