#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <vector>

/**
 * @brief Comando digitado pelo jogador, já separado em nome e argumentos.
 *
 * É criado pelo CommandParser, que garante que o nome existe e que a quantidade
 * de argumentos está certa. Os argumentos continuam como texto; o Shell converte
 * cada um quando precisa (por exemplo, com CommandParser::parseCell).
 */
struct Command {
    std::string name;              ///< Nome do comando, como "install" ou "run".
    std::vector<std::string> args; ///< Argumentos, na ordem em que foram digitados.
};

#endif // COMMAND_HPP
