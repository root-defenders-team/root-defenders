#include <doctest/doctest.h>

#include <string>

#include "GameException.hpp"
#include "InvalidCommandException.hpp"

TEST_SUITE("InvalidCommandException") {

TEST_CASE("preserva a explicacao do comando invalido") {
    InvalidCommandException error("O comando install precisa de tipo, x e y.");

    CHECK(std::string(error.what()) == "O comando install precisa de tipo, x e y.");
}

TEST_CASE("pode ser capturada pela excecao-base do jogo") {
    CHECK_THROWS_AS(throw InvalidCommandException("Comando desconhecido."), GameException);
}

} // TEST_SUITE("InvalidCommandException")
