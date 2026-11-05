#include "GameException.hpp"

GameException::GameException(const std::string& message)
    : std::runtime_error(message) {
}
