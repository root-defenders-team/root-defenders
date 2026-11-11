#ifndef SCORE_BOARD_HPP
#define SCORE_BOARD_HPP

#include <cstddef>
#include <string>
#include <vector>

#include "ScoreEntry.hpp"

/**
 * @brief Ranking das 5 melhores partidas, guardado num arquivo de texto.
 *
 * Cada linha do arquivo tem o formato pontos;onda alcançada;nome, da maior
 * para a menor pontuação. Arquivo ausente vira ranking vazio, e linha corrompida
 * é pulada e contada, para o Shell avisar o jogador sem interromper o jogo.
 */
class ScoreBoard {
public:
    static const std::size_t MAX_ENTRIES = 5; ///< Quantidade de entradas guardadas.

    /**
     * @brief Cria um ranking vazio ligado a um arquivo.
     *
     * O arquivo só é lido em load() e só é escrito em save().
     *
     * @param filePath Caminho do arquivo do ranking.
     */
    explicit ScoreBoard(const std::string& filePath);

    /**
     * @brief Lê o arquivo, descartando as entradas que estavam na memória.
     *
     * Arquivo ausente deixa o ranking vazio. Linhas corrompidas são puladas e
     * contadas em getIgnoredLines(); linhas vazias são ignoradas sem contar.
     */
    void load();

    /**
     * @brief Grava as entradas no arquivo, uma por linha.
     * @throws GameException se não for possível abrir ou gravar o arquivo.
     */
    void save() const;

    /**
     * @brief Diz se uma pontuação entra no top 5.
     * @param score Pontuação da partida.
     * @return true se ainda há vaga ou se supera a menor pontuação do ranking.
     */
    bool qualifies(int score) const;

    /**
     * @brief Coloca uma entrada na posição certa e mantém só as 5 melhores.
     *
     * Em caso de empate, a entrada mais antiga fica na frente.
     *
     * @param entry Entrada com o nome já limpo pelo Shell.
     * @throws std::invalid_argument se o nome for vazio ou tiver ';', ou se a
     *         pontuação ou a onda forem negativas.
     */
    void addEntry(const ScoreEntry& entry);

    /**
     * @brief Retorna as entradas, da maior para a menor pontuação.
     * @return As entradas, da maior para a menor pontuação.
     */
    const std::vector<ScoreEntry>& getEntries() const;

    /**
     * @brief Retorna a maior pontuação do ranking, ou 0 se ele estiver vazio.
     * @return A maior pontuação do ranking, ou 0 se ele estiver vazio.
     */
    int getHighScore() const;

    /**
     * @brief Retorna quantas linhas corrompidas o último load() pulou.
     * @return Quantas linhas corrompidas o último load() pulou.
     */
    int getIgnoredLines() const;

private:
    std::string filePath;            ///< Caminho do arquivo do ranking.
    std::vector<ScoreEntry> entries; ///< Entradas, da maior para a menor pontuação.
    int ignoredLines;                ///< Linhas corrompidas puladas no último load().
};

#endif // SCORE_BOARD_HPP
