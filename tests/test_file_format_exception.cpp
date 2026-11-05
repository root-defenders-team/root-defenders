#include <doctest/doctest.h>

#include <string>

#include "FileFormatException.hpp"
#include "GameException.hpp"

TEST_SUITE("FileFormatException") {

TEST_CASE("informa arquivo linha e motivo do erro") {
    FileFormatException error("fase.txt", 3, "caractere desconhecido.");

    CHECK(error.getFileName() == "fase.txt");
    CHECK(error.getLineNumber() == 3);
    CHECK(std::string(error.what()) == "fase.txt, linha 3: caractere desconhecido.");
}

TEST_CASE("usa linha zero para problemas gerais do arquivo") {
    FileFormatException error("fase.txt", 0, "arquivo vazio.");

    CHECK(error.getLineNumber() == 0);
    CHECK(std::string(error.what()) == "fase.txt, linha 0: arquivo vazio.");
}

TEST_CASE("pode ser capturada pela excecao-base do jogo") {
    CHECK_THROWS_AS(throw FileFormatException("fase.txt", 2, "linha vazia."), GameException);
}

} // TEST_SUITE("FileFormatException")
