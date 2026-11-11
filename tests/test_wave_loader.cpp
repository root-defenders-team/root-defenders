#include <doctest/doctest.h>

#include <cstdio>
#include <fstream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include "EnemyType.hpp"
#include "FileFormatException.hpp"
#include "WaveLoader.hpp"

namespace {

void checkWaveError(std::istream& input, int expectedLine, const std::string& reason) {
    try {
        WaveLoader::parse(input, "ondas_teste.txt");
        FAIL("O arquivo invalido deveria lancar FileFormatException.");
    } catch (const FileFormatException& error) {
        CHECK(error.getFileName() == "ondas_teste.txt");
        CHECK(error.getLineNumber() == expectedLine);
        CHECK(std::string(error.what()) ==
              "ondas_teste.txt, linha " + std::to_string(expectedLine) + ": " + reason);
    }
}

} // namespace

TEST_SUITE("WaveLoader") {

TEST_CASE("carrega grupos em ordem e ignora comentarios e linhas vazias") {
    std::stringstream input("// comentario\n\n1 virus 2 1.0\n1 worm 1 0\n"
                            "  // outro comentario\n2 rootkit 1 0\n");

    std::vector<Wave> waves = WaveLoader::parse(input, "ondas_teste.txt");

    REQUIRE(waves.size() == 2);
    CHECK(waves[0].getTotalEnemies() == 3);
    CHECK(waves[1].getTotalEnemies() == 1);
    CHECK(waves[0].update(0.0f) == std::vector<EnemyType>{EnemyType::Virus});
    CHECK(waves[0].update(1.0f) == std::vector<EnemyType>{EnemyType::Virus});
    CHECK(waves[0].update(0.0f) == std::vector<EnemyType>{EnemyType::Worm});
    CHECK(waves[1].update(0.0f) == std::vector<EnemyType>{EnemyType::Rootkit});
}

TEST_CASE("aceita fim de linha Windows e arquivo sem quebra final") {
    std::stringstream input("1 virus 1 0\r\n2 worm 1 0");

    std::vector<Wave> waves = WaveLoader::parse(input, "windows.txt");

    REQUIRE(waves.size() == 2);
    CHECK(waves[0].getTotalEnemies() == 1);
    CHECK(waves[1].getTotalEnemies() == 1);
}

TEST_CASE("rejeita arquivo sem grupos") {
    std::stringstream empty;
    std::stringstream comments("// apenas comentario\n  \n");

    checkWaveError(empty, 0, "arquivo de ondas vazio.");
    checkWaveError(comments, 0, "arquivo de ondas vazio.");
}

TEST_CASE("exige exatamente quatro campos por grupo") {
    std::stringstream missing("1 virus 2\n");
    std::stringstream extra("1 virus 2 1.0 extra\n");

    checkWaveError(missing, 1, "cada grupo deve ter quatro campos.");
    checkWaveError(extra, 1, "cada grupo deve ter quatro campos.");
}

TEST_CASE("valida numero de onda e ordem das ondas") {
    std::stringstream text("um virus 1 0\n");
    std::stringstream zero("0 virus 1 0\n");
    std::stringstream startAtTwo("2 virus 1 0\n");
    std::stringstream gap("1 virus 1 0\n3 worm 1 0\n");
    std::stringstream backwards("1 virus 1 0\n2 worm 1 0\n1 virus 1 0\n");

    checkWaveError(text, 1, "numero de onda invalido.");
    checkWaveError(zero, 1, "numero de onda invalido.");
    checkWaveError(startAtTwo, 1, "ondas fora de ordem.");
    checkWaveError(gap, 2, "ondas fora de ordem.");
    checkWaveError(backwards, 3, "ondas fora de ordem.");
}

TEST_CASE("rejeita tipo de inimigo desconhecido") {
    std::stringstream input("1 fantasma 1 0\n");

    checkWaveError(input, 1, "tipo de inimigo desconhecido.");
}

TEST_CASE("valida quantidade de inimigos") {
    std::stringstream zero("1 virus 0 1.0\n");
    std::stringstream negative("1 virus -2 1.0\n");
    std::stringstream text("1 virus dois 1.0\n");
    std::stringstream fraction("1 virus 2.5 1.0\n");

    checkWaveError(zero, 1, "quantidade de inimigos invalida.");
    checkWaveError(negative, 1, "quantidade de inimigos invalida.");
    checkWaveError(text, 1, "quantidade de inimigos invalida.");
    checkWaveError(fraction, 1, "quantidade de inimigos invalida.");
}

TEST_CASE("valida intervalo de nascimento") {
    std::stringstream negative("1 virus 1 -0.1\n");
    std::stringstream text("1 virus 1 rapido\n");
    std::stringstream suffix("1 virus 1 0.5s\n");
    std::stringstream infinity("1 virus 1 1e999\n");

    checkWaveError(negative, 1, "intervalo invalido.");
    checkWaveError(text, 1, "intervalo invalido.");
    checkWaveError(suffix, 1, "intervalo invalido.");
    checkWaveError(infinity, 1, "intervalo invalido.");
}

TEST_CASE("rejeita soma de quantidades maior que o limite") {
    std::stringstream input("1 virus " +
                            std::to_string(std::numeric_limits<int>::max()) +
                            " 0\n1 worm 1 0\n");

    checkWaveError(input, 2, "quantidade total da onda excede o limite.");
}

TEST_CASE("informa falha ao ler a entrada") {
    std::stringstream input("1 virus 1 0\n");
    input.setstate(std::ios::badbit);

    checkWaveError(input, 1, "falha ao ler as ondas.");
}

TEST_CASE("abre um arquivo real de ondas") {
    const std::string filePath = "build/test_wave_loader.txt";
    std::ifstream existing(filePath.c_str());
    REQUIRE_FALSE(existing.is_open());

    std::ofstream file(filePath.c_str());
    REQUIRE(file.is_open());
    file << "1 virus 2 0.5\n";
    file.close();

    std::vector<Wave> waves = WaveLoader::load(filePath);
    CHECK(std::remove(filePath.c_str()) == 0);

    REQUIRE(waves.size() == 1);
    CHECK(waves[0].getTotalEnemies() == 2);
}

TEST_CASE("carrega as oito ondas da fase") {
    std::vector<Wave> waves = WaveLoader::load("assets/waves.txt");
    const int enemyCounts[] = {3, 6, 5, 11, 14, 18, 23, 28};

    REQUIRE(waves.size() == 8);
    for (int i = 0; i < 8; ++i) {
        CHECK(waves[i].getTotalEnemies() == enemyCounts[i]);
    }
}

TEST_CASE("informa arquivo e linha zero quando nao consegue abrir") {
    const std::string filePath = "include/Wave.hpp/ondas.txt";

    try {
        WaveLoader::load(filePath);
        FAIL("O arquivo inexistente deveria lancar FileFormatException.");
    } catch (const FileFormatException& error) {
        CHECK(error.getFileName() == filePath);
        CHECK(error.getLineNumber() == 0);
        CHECK(std::string(error.what()) ==
              filePath + ", linha 0: nao foi possivel abrir o arquivo.");
    }
}

} // TEST_SUITE("WaveLoader")
