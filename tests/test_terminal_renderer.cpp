#include <doctest/doctest.h>

#include <sstream>
#include <string>
#include <vector>

#include "Cell.hpp"
#include "CellType.hpp"
#include "EnemyType.hpp"
#include "FirewallTower.hpp"
#include "GameManager.hpp"
#include "MakeUnique.hpp"
#include "Map.hpp"
#include "ScoreBoard.hpp"
#include "ScoreEntry.hpp"
#include "SpawnGroup.hpp"
#include "TerminalRenderer.hpp"
#include "VirusEnemy.hpp"
#include "Wave.hpp"

namespace {

Map makeRendererTestMap() {
    Map map(4, 3);
    for (int x = 0; x < 4; ++x) {
        Cell cell = {x, 1};
        map.setCellType(cell, CellType::Path);
        map.addWaypoint(cell);
    }
    Cell blocked = {2, 0};
    map.setCellType(blocked, CellType::Blocked);
    return map;
}

std::vector<Wave> makeRendererTestWaves() {
    std::vector<SpawnGroup> groups;
    SpawnGroup group = {EnemyType::Virus, 1, 0.0f};
    groups.push_back(group);
    std::vector<Wave> waves;
    waves.push_back(Wave(groups));
    return waves;
}

} // namespace

TEST_SUITE("TerminalRenderer") {

TEST_CASE("drawHeader exibe CPU, integridade, onda e pontos") {
    GameManager game(makeRendererTestMap(), makeRendererTestWaves());
    TerminalRenderer renderer(false);
    std::ostringstream out;

    renderer.drawHeader(game, out);
    std::string header = out.str();

    CHECK(header.find("CPU: 200") != std::string::npos);
    CHECK(header.find("Integridade: 100") != std::string::npos);
    CHECK(header.find("Onda: 0/1") != std::string::npos);
    CHECK(header.find("Pontos: 0") != std::string::npos);
}

TEST_CASE("drawMap desenha terreno, entrada, root, defesas e malwares nas posicoes certas") {
    GameManager game(makeRendererTestMap(), makeRendererTestWaves());
    Cell towerCell = {1, 0};
    REQUIRE(game.buildTower(util::makeUnique<FirewallTower>(towerCell)));
    game.spawnEnemy(util::makeUnique<VirusEnemy>(game.getMap().getPath()));

    TerminalRenderer renderer(false);
    std::ostringstream out;
    renderer.drawMap(game, out);

    std::string expected =
        "   0123\n"
        " 0 .FX.\n"
        " 1 v##@\n"
        " 2 ....\n";
    CHECK(out.str() == expected);
}

TEST_CASE("drawMap numera as dezenas das colunas em mapas largos") {
    Map map(12, 2);
    for (int x = 0; x < 12; ++x) {
        Cell cell = {x, 0};
        map.setCellType(cell, CellType::Path);
        map.addWaypoint(cell);
    }
    GameManager game(map, makeRendererTestWaves());

    TerminalRenderer renderer(false);
    std::ostringstream out;
    renderer.drawMap(game, out);

    CHECK(out.str().find("             11\n") == 0);
    CHECK(out.str().find("   012345678901\n") != std::string::npos);
}

TEST_CASE("drawMap pinta os simbolos quando as cores estao ligadas") {
    GameManager game(makeRendererTestMap(), makeRendererTestWaves());
    Cell towerCell = {1, 0};
    REQUIRE(game.buildTower(util::makeUnique<FirewallTower>(towerCell)));
    game.spawnEnemy(util::makeUnique<VirusEnemy>(game.getMap().getPath()));

    TerminalRenderer renderer(true);
    std::ostringstream out;
    renderer.drawMap(game, out);

    CHECK(out.str().find("\033[1;32mF\033[0m") != std::string::npos);
    CHECK(out.str().find("\033[1;31mv\033[0m") != std::string::npos);
    CHECK(out.str().find("\033[1;35m@\033[0m") != std::string::npos);
}

TEST_CASE("drawFrame limpa a tela e desenha cabecalho e mapa") {
    GameManager game(makeRendererTestMap(), makeRendererTestWaves());
    TerminalRenderer renderer(false);
    std::ostringstream out;

    renderer.drawFrame(game, out);
    std::string frame = out.str();

    CHECK(frame.find("\033[2J\033[H") == 0);
    CHECK(frame.find("CPU: 200") != std::string::npos);
    CHECK(frame.find("E##@") != std::string::npos);
    CHECK(frame.find("Legenda:") != std::string::npos);
}

TEST_CASE("drawTowerList e drawRanking exibem informacoes formatadas") {
    GameManager game(makeRendererTestMap(), makeRendererTestWaves());
    TerminalRenderer renderer(false);
    std::ostringstream out;

    renderer.drawTowerList(game, out);
    CHECK(out.str().find("Nenhuma defesa") != std::string::npos);

    out.str("");
    Cell towerCell = {1, 0};
    REQUIRE(game.buildTower(util::makeUnique<FirewallTower>(towerCell)));
    renderer.drawTowerList(game, out);
    CHECK(out.str().find("(1, 0)") != std::string::npos);

    out.str("");
    ScoreBoard board("build/test_renderer_ranking.txt");
    ScoreEntry entry = {"Vitor", 500, 3};
    board.addEntry(entry);
    renderer.drawRanking(board, out);
    CHECK(out.str().find("Vitor") != std::string::npos);
    CHECK(out.str().find("500") != std::string::npos);
}

} // TEST_SUITE("TerminalRenderer")
