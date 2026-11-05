#include <doctest/doctest.h>

#include <stdexcept>
#include <string>

#include "GameException.hpp"

TEST_SUITE("GameException") {

TEST_CASE("preserva a mensagem do erro") {
    GameException error("Nao foi possivel iniciar a partida.");

    CHECK(std::string(error.what()) == "Nao foi possivel iniciar a partida.");
}

TEST_CASE("pode ser capturada como runtime_error") {
    CHECK_THROWS_AS(throw GameException("Erro do jogo."), std::runtime_error);
}

} // TEST_SUITE("GameException")
