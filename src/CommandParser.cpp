#include "CommandParser.hpp"

#include <cstddef>
#include <sstream>

#include "InvalidCommandException.hpp"

namespace {

/// Um comando do jogo: nome, quantidade de argumentos e forma de uso.
struct CommandRule {
    const char* name;           ///< Nome digitado pelo jogador.
    std::size_t argumentCount;  ///< Quantidade exata de argumentos.
    const char* usage;          ///< Forma de uso, mostrada quando o jogador erra.
};

const CommandRule RULES[] = {
    {"help", 0, "help"},
    {"install", 3, "install <tipo> <x> <y>"},
    {"upgrade", 2, "upgrade <x> <y>"},
    {"rm", 2, "rm <x> <y>"},
    {"target", 3, "target <x> <y> <prioridade>"},
    {"ls", 0, "ls"},
    {"man", 1, "man <nome>"},
    {"run", 0, "run"},
    {"ranking", 0, "ranking"},
    {"rank", 0, "rank"},
    {"exit", 0, "exit"}
};

const std::size_t RULE_COUNT = sizeof(RULES) / sizeof(RULES[0]);

} // namespace

Command CommandParser::parse(const std::string& line) {
    std::istringstream words(line);
    Command command;
    if (!(words >> command.name)) {
        throw InvalidCommandException("comando vazio (digite help para ver os comandos)");
    }
    std::string word;
    while (words >> word) {
        command.args.push_back(word);
    }

    for (std::size_t i = 0; i < RULE_COUNT; ++i) {
        if (command.name == RULES[i].name) {
            if (command.args.size() != RULES[i].argumentCount) {
                throw InvalidCommandException(std::string("uso: ") + RULES[i].usage);
            }
            return command;
        }
    }
    throw InvalidCommandException("comando desconhecido: " + command.name +
                                  " (digite help para ver os comandos)");
}

int CommandParser::parseInt(const std::string& text) {
    std::istringstream input(text);
    int value = 0;
    char extra = '\0';
    // Falha se não começar com número, se sobrar algo depois dele ou se não couber num int.
    if (!(input >> value) || (input >> extra)) {
        throw InvalidCommandException("'" + text + "' não é um número inteiro");
    }
    return value;
}

Cell CommandParser::parseCell(const std::string& xText, const std::string& yText) {
    Cell cell = {parseInt(xText), parseInt(yText)};
    return cell;
}

TargetPriority CommandParser::parsePriority(const std::string& text) {
    if (text == "first") {
        return TargetPriority::First;
    }
    if (text == "strong") {
        return TargetPriority::Strongest;
    }
    if (text == "weak") {
        return TargetPriority::Weakest;
    }
    if (text == "close") {
        return TargetPriority::Closest;
    }
    throw InvalidCommandException("prioridade desconhecida: " + text +
                                  " (use first, strong, weak ou close)");
}
