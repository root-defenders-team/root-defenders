#ifndef GAME_EXCEPTION_HPP
#define GAME_EXCEPTION_HPP

#include <stdexcept>
#include <string>

/**
 * @brief Exceção-base para erros identificados pelo jogo.
 *
 * A mensagem pode ser consultada por what(), herdado de std::runtime_error.
 */
class GameException : public std::runtime_error {
public:
    /**
     * @brief Cria uma exceção com a descrição do erro.
     * @param message Mensagem que explica o problema.
     */
    explicit GameException(const std::string& message);
};

#endif // GAME_EXCEPTION_HPP
