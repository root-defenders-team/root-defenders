#ifndef INVALID_COMMAND_EXCEPTION_HPP
#define INVALID_COMMAND_EXCEPTION_HPP

#include <string>

#include "GameException.hpp"

/**
 * @brief Informa um comando desconhecido ou com argumentos inválidos.
 */
class InvalidCommandException : public GameException {
public:
    /**
     * @brief Cria uma exceção para um comando inválido.
     * @param message Mensagem que explica o erro no comando.
     */
    explicit InvalidCommandException(const std::string& message);
};

#endif // INVALID_COMMAND_EXCEPTION_HPP
