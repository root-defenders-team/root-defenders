#include <doctest/doctest.h>

#include <cstddef>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

#include "CellType.hpp"
#include "FileFormatException.hpp"
#include "MapLoader.hpp"

namespace {

// Todos os erros devem informar a origem, a linha e o motivo do problema.
void checkMapError(std::istream& input, int expectedLine, const std::string& reason) {
    try {
        MapLoader::parse(input, "fase_teste.txt");
        FAIL("O mapa invalido deveria lancar FileFormatException.");
    } catch (const FileFormatException& error) {
        CHECK(error.getFileName() == "fase_teste.txt");
        CHECK(error.getLineNumber() == expectedLine);
        CHECK(std::string(error.what()) ==
              "fase_teste.txt, linha " + std::to_string(expectedLine) + ": " + reason);
    }
}

} // namespace

TEST_SUITE("MapLoader") {

TEST_CASE("creates a map from a valid text input") {
    std::stringstream input("E#@\n...\nX..\n");

    Map map = MapLoader::parse(input, "mapa_teste");

    CHECK(map.getWidth() == 3);
    CHECK(map.getHeight() == 3);
    CHECK(map.getCellType({0, 0}) == CellType::Path);
    CHECK(map.getCellType({1, 0}) == CellType::Path);
    CHECK(map.getCellType({1, 1}) == CellType::Buildable);
    CHECK(map.getCellType({0, 2}) == CellType::Blocked);
    REQUIRE(map.getWaypoints().size() == 3);
    CHECK(map.getWaypoints()[0] == Cell{0, 0});
    CHECK(map.getWaypoints()[2] == Cell{2, 0});
    CHECK(map.hasPath());
}

TEST_CASE("rejects maps with inconsistent line sizes") {
    std::stringstream input("E#@\n..\n");

    checkMapError(input, 2, "todas as linhas devem ter o mesmo tamanho.");
}

TEST_CASE("rejeita caracteres desconhecidos e espacos") {
    std::stringstream invalidCharacter("E#@\n.?X\n");
    std::stringstream space("E @\n");

    checkMapError(invalidCharacter, 2, "caractere desconhecido.");
    checkMapError(space, 1, "caractere desconhecido.");
}

TEST_CASE("rejects interrupted or forked paths") {
    std::stringstream interruptedPath("E.X\n..@\n");
    std::stringstream forkedPath("E#@\n.#.\n");

    checkMapError(interruptedPath, 1, "caminho interrompido.");
    checkMapError(forkedPath, 1, "caminho com bifurcacao.");
}

TEST_CASE("rejeita arquivo vazio e linha vazia") {
    std::stringstream empty;
    std::stringstream blankLine("E#@\n\n");

    checkMapError(empty, 0, "arquivo de mapa vazio.");
    checkMapError(blankLine, 2, "linha vazia.");
}

TEST_CASE("rejeita mapas sem entrada ou sem destino") {
    std::stringstream missingEntry(".#@\n");
    std::stringstream missingDestination("E#.\n");

    checkMapError(missingEntry, 0, "o mapa deve possuir uma entrada.");
    checkMapError(missingDestination, 0, "o mapa deve possuir um destino.");
}

TEST_CASE("informa a linha da segunda entrada") {
    std::stringstream input("E#@\nE..\n");

    checkMapError(input, 2, "o mapa deve possuir uma unica entrada.");
}

TEST_CASE("informa a linha do segundo destino") {
    std::stringstream input("E#@\n..@\n");

    checkMapError(input, 2, "o mapa deve possuir um unico destino.");
}

TEST_CASE("rejeita trecho de caminho desconectado da rota") {
    std::stringstream input("E#@..\n.....\n....#\n");

    checkMapError(input, 3, "caminho desconectado.");
}

TEST_CASE("detecta falha de leitura sem confundir com arquivo vazio") {
    std::stringstream input("E#@\n");
    input.setstate(std::ios::badbit);

    checkMapError(input, 1, "falha ao ler o mapa.");
}

TEST_CASE("rejeita entrada em estado de falha sem fim de arquivo") {
    std::stringstream input("E#@\n");
    input.setstate(std::ios::failbit);

    checkMapError(input, 1, "falha ao ler o mapa.");
}

TEST_CASE("aceita arquivo do Windows e ultima linha sem quebra") {
    std::stringstream windowsInput("E#@\r\n...\r\n");
    std::stringstream noFinalNewline("E#@");

    Map windowsMap = MapLoader::parse(windowsInput, "windows.txt");
    Map noNewlineMap = MapLoader::parse(noFinalNewline, "sem_quebra.txt");

    CHECK(windowsMap.getHeight() == 2);
    CHECK(windowsMap.getWaypoints().size() == 3);
    CHECK(noNewlineMap.getHeight() == 1);
    CHECK(noNewlineMap.getWaypoints().size() == 3);
}

TEST_CASE("monta waypoints de um caminho com curvas na ordem correta") {
    std::stringstream input("E#..\n.#..\n.##@\n");

    Map map = MapLoader::parse(input, "curvas.txt");

    REQUIRE(map.getWaypoints().size() == 6);
    CHECK(map.getWaypoints()[0] == Cell{0, 0});
    CHECK(map.getWaypoints()[1] == Cell{1, 0});
    CHECK(map.getWaypoints()[2] == Cell{1, 1});
    CHECK(map.getWaypoints()[3] == Cell{1, 2});
    CHECK(map.getWaypoints()[4] == Cell{2, 2});
    CHECK(map.getWaypoints()[5] == Cell{3, 2});
}

TEST_CASE("aceita caminho para a esquerda e caminho vertical") {
    std::stringstream left("@#E\n");
    std::stringstream vertical("E\n#\n@\n");

    Map leftMap = MapLoader::parse(left, "esquerda.txt");
    Map verticalMap = MapLoader::parse(vertical, "vertical.txt");

    REQUIRE(leftMap.getWaypoints().size() == 3);
    REQUIRE(verticalMap.getWaypoints().size() == 3);
    CHECK(leftMap.getWaypoints().front() == Cell{2, 0});
    CHECK(leftMap.getWaypoints().back() == Cell{0, 0});
    CHECK(verticalMap.getWaypoints().front() == Cell{0, 0});
    CHECK(verticalMap.getWaypoints().back() == Cell{0, 2});
}

TEST_CASE("aceita entrada ao lado do destino") {
    std::stringstream input("E@");

    Map map = MapLoader::parse(input, "curto.txt");

    CHECK(map.hasPath());
    CHECK(map.getWaypoints().size() == 2);
}

TEST_CASE("carrega mapa de um arquivo real") {
    const std::string filePath = "build/test_map_loader_c9.txt";
    // Impede que o teste sobrescreva um arquivo que ja exista nesse caminho.
    std::ifstream existing(filePath.c_str());
    REQUIRE_FALSE(existing.is_open());

    std::ofstream file(filePath.c_str());
    REQUIRE(file.is_open());
    file << "E#@\n.X.\n";
    file.close();

    Map map = MapLoader::load(filePath);
    CHECK(std::remove(filePath.c_str()) == 0);

    CHECK(map.getWidth() == 3);
    CHECK(map.getHeight() == 2);
    CHECK(map.isBlocked({1, 1}));
    CHECK(map.getWaypoints().size() == 3);
}

TEST_CASE("carrega os cinco mapas da fase") {
    const char* paths[] = {
        "assets/maps/level1.txt",
        "assets/maps/level2.txt",
        "assets/maps/level3.txt",
        "assets/maps/level4.txt",
        "assets/maps/level5.txt"
    };
    const std::size_t waypointCounts[] = {37, 33, 28, 24, 20};
    const Cell entries[] = {{0, 0}, {0, 1}, {0, 0}, {0, 2}, {0, 4}};
    const Cell destinations[] = {{16, 8}, {17, 8}, {19, 8}, {19, 6}, {19, 4}};

    for (int i = 0; i < 5; ++i) {
        Map map = MapLoader::load(paths[i]);

        CHECK(map.getWidth() == 20);
        CHECK(map.getHeight() == 10);
        REQUIRE(map.getWaypoints().size() == waypointCounts[i]);
        CHECK(map.getWaypoints().front() == entries[i]);
        CHECK(map.getWaypoints().back() == destinations[i]);
    }
}

TEST_CASE("informa arquivo e linha zero quando nao consegue abrir") {
    // Um componente intermediario inexistente garante que o caminho nao abra.
    const std::string filePath = "include/Map.hpp/mapa.txt";

    try {
        MapLoader::load(filePath);
        FAIL("O arquivo inexistente deveria lancar FileFormatException.");
    } catch (const FileFormatException& error) {
        CHECK(error.getFileName() == filePath);
        CHECK(error.getLineNumber() == 0);
        CHECK(std::string(error.what()) ==
              filePath + ", linha 0: nao foi possivel abrir o arquivo.");
    }
}

} // TEST_SUITE("MapLoader")
