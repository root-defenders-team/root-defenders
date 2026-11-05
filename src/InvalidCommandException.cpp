#include "InvalidCommandException.hpp"

InvalidCommandException::InvalidCommandException(const std::string& message)
    : GameException(message) {
}
