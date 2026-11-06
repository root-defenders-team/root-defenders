#ifndef COMMAND_PARSER_HPP
#define COMMAND_PARSER_HPP

#include <string>

#include "Cell.hpp"
#include "Command.hpp"
#include "TargetPriority.hpp"

/**
 * @brief Transforma o texto digitado pelo jogador em comandos validados.
 *
 * É o único lugar que conhece a lista de comandos do jogo e quantos argumentos
 * cada um recebe. Qualquer erro de digitação vira InvalidCommandException, com uma
 * mensagem que o Shell mostra direto ao jogador.
 */
class CommandParser {
public:
    /**
     * @brief Separa uma linha em nome e argumentos e confere o comando.
     *
     * Espaços extras entre as palavras são ignorados.
     *
     * @param line Linha digitada pelo jogador.
     * @return O comando com o nome e os argumentos.
     * @throws InvalidCommandException se a linha estiver vazia, o comando não existir
     *         ou a quantidade de argumentos estiver errada.
     */
    static Command parse(const std::string& line);

    /**
     * @brief Converte um texto em número inteiro.
     * @param text Texto com o número, como "5" ou "-2".
     * @return O número lido.
     * @throws InvalidCommandException se o texto não for um inteiro válido.
     */
    static int parseInt(const std::string& text);

    /**
     * @brief Converte a coluna e a linha digitadas em uma célula.
     *
     * Não confere se a célula está dentro do mapa; isso é papel do Shell.
     *
     * @param xText Coluna, em texto.
     * @param yText Linha, em texto.
     * @return A célula (x, y).
     * @throws InvalidCommandException se a coluna ou a linha não forem inteiros.
     */
    static Cell parseCell(const std::string& xText, const std::string& yText);

    /**
     * @brief Converte o nome de uma prioridade de alvo.
     * @param text "first", "strong", "weak" ou "close".
     * @return A prioridade correspondente.
     * @throws InvalidCommandException se o nome não for de nenhuma prioridade.
     */
    static TargetPriority parsePriority(const std::string& text);
};

#endif // COMMAND_PARSER_HPP
