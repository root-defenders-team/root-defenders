#include <doctest/doctest.h>

#include <cstdio>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Cell.hpp"
#include "CellType.hpp"
#include "EnemyType.hpp"
#include "GameManager.hpp"
#include "Map.hpp"
#include "ScoreBoard.hpp"
#include "ScoreEntry.hpp"
#include "Shell.hpp"
#include "SpawnGroup.hpp"
#include "TargetPriority.hpp"
#include "Tower.hpp"
#include "Wave.hpp"

namespace {

const char* RANKING_PATH = "build/test_shell_ranking.txt"; ///< Apagado no fim de cada teste.

/// Mapa 5x3 com o caminho na linha do meio e um obstáculo em (4, 0).
Map makeTestMap() {
    Map map(5, 3);
    for (int x = 0; x < 5; ++x) {
        Cell cell = {x, 1};
        map.setCellType(cell, CellType::Path);
        map.addWaypoint(cell);
    }
    Cell rock = {4, 0};
    map.setCellType(rock, CellType::Blocked);
    return map;
}

/// Uma única onda com um único Virus.
std::vector<Wave> makeTestWaves() {
    std::vector<SpawnGroup> groups;
    SpawnGroup group = {EnemyType::Virus, 1, 0.0f};
    groups.push_back(group);
    return std::vector<Wave>(1, Wave(groups));
}

/// Diz se o texto aparece em algum lugar da saída do Shell.
bool contains(const std::ostringstream& output, const std::string& text) {
    return output.str().find(text) != std::string::npos;
}

/// Monta uma partida, um ranking, a entrada, a saída e o Shell de cada teste.
class ShellTest {
protected:
    ShellTest()
        : game(makeTestMap(), makeTestWaves()),
          scoreBoard(RANKING_PATH),
          shell(game, scoreBoard, input, output) {
        std::remove(RANKING_PATH);
        shell.setFrameDelay(0);
    }

    ~ShellTest() {
        std::remove(RANKING_PATH);
    }

    GameManager game;
    ScoreBoard scoreBoard;
    std::istringstream input;
    std::ostringstream output;
    Shell shell;
};

} // namespace

TEST_SUITE("Shell") {

TEST_CASE_FIXTURE(ShellTest, "an empty line does nothing") {
    CHECK(shell.execute(""));
    CHECK(shell.execute("    "));
    CHECK(output.str().empty());
}

TEST_CASE_FIXTURE(ShellTest, "exit ends the shell") {
    CHECK_FALSE(shell.execute("exit"));
}

TEST_CASE_FIXTURE(ShellTest, "an invalid command shows an error and keeps going") {
    CHECK(shell.execute("hack 1 2"));
    CHECK(contains(output, "erro:"));

    CHECK(shell.execute("install firewall 2"));
    CHECK(game.getTowers().empty());
}

TEST_CASE_FIXTURE(ShellTest, "install builds a tower and charges its cost") {
    Cell cell = {2, 0};

    CHECK(shell.execute("install firewall 2 0"));

    REQUIRE(game.getTowerAt(cell) != nullptr);
    CHECK(game.getGold() == GameManager::INITIAL_GOLD - game.getTowerAt(cell)->getCost());
    CHECK(contains(output, "instalado em (2, 0)"));
}

TEST_CASE_FIXTURE(ShellTest, "install accepts a defense name with only its first letter capitalized") {
    Cell cell = {2, 0};

    CHECK(shell.execute("install Firewall 2 0"));

    REQUIRE(game.getTowerAt(cell) != nullptr);
    CHECK(game.getTowerAt(cell)->getSymbol() == 'F');
    CHECK(contains(output, "instalado em (2, 0)"));
}

TEST_CASE_FIXTURE(ShellTest, "install refuses the path, obstacles, outside and occupied cells") {
    shell.execute("install firewall 2 1");
    CHECK(contains(output, "caminho"));

    shell.execute("install firewall 4 0");
    CHECK(contains(output, "obstáculo"));

    shell.execute("install firewall 9 9");
    CHECK(contains(output, "fora do mapa"));

    REQUIRE(shell.execute("install firewall 2 0"));
    int goldAfterFirst = game.getGold();
    shell.execute("install scanner 2 0");
    CHECK(contains(output, "já tem uma defesa"));

    CHECK(game.getTowers().size() == 1);
    CHECK(game.getGold() == goldAfterFirst);
}

TEST_CASE_FIXTURE(ShellTest, "install rejects an unknown type and a bad coordinate") {
    shell.execute("install laser 2 0");
    CHECK(contains(output, "tipo de defesa desconhecido"));

    shell.execute("install firewall a 0");
    CHECK(contains(output, "erro:"));

    CHECK(game.getTowers().empty());
    CHECK(game.getGold() == GameManager::INITIAL_GOLD);
}

TEST_CASE_FIXTURE(ShellTest, "install without enough gold shows an error") {
    REQUIRE(game.spendGold(game.getGold()));

    shell.execute("install firewall 2 0");

    CHECK(contains(output, "CPU insuficiente"));
    CHECK(game.getTowers().empty());
}

TEST_CASE_FIXTURE(ShellTest, "upgrade evolves the tower") {
    Cell cell = {2, 0};
    REQUIRE(shell.execute("install firewall 2 0"));

    shell.execute("upgrade 2 0");

    CHECK(game.getTowerAt(cell)->getLevel() == 2);
    CHECK(contains(output, "nível 2"));
}

TEST_CASE_FIXTURE(ShellTest, "upgrade and rm without a tower show an error") {
    shell.execute("upgrade 3 0");
    CHECK(contains(output, "não há defesa em (3, 0)"));

    output.str("");
    shell.execute("rm 3 0");
    CHECK(contains(output, "não há defesa em (3, 0)"));
}

TEST_CASE_FIXTURE(ShellTest, "upgrade stops at the maximum level") {
    Cell cell = {2, 0};
    REQUIRE(shell.execute("install firewall 2 0"));
    game.addGold(10000);
    for (int level = 1; level < Tower::MAX_LEVEL; ++level) {
        shell.execute("upgrade 2 0");
    }

    shell.execute("upgrade 2 0");

    CHECK(contains(output, "nível máximo"));
    CHECK(game.getTowerAt(cell)->getLevel() == Tower::MAX_LEVEL);
}

TEST_CASE_FIXTURE(ShellTest, "rm removes the tower and refunds part of the cost") {
    Cell cell = {2, 0};
    REQUIRE(shell.execute("install firewall 2 0"));
    int goldBefore = game.getGold();
    int refund = game.getTowerAt(cell)->getSellValue();

    shell.execute("rm 2 0");

    CHECK(game.getTowerAt(cell) == nullptr);
    CHECK(game.getGold() == goldBefore + refund);
    CHECK(contains(output, "removida"));
}

TEST_CASE_FIXTURE(ShellTest, "target changes the priority and rejects an unknown one") {
    Cell cell = {2, 0};
    REQUIRE(shell.execute("install firewall 2 0"));

    shell.execute("target 2 0 strong");
    CHECK(game.getTowerAt(cell)->getTargetPriority() == TargetPriority::Strongest);

    shell.execute("target 2 0 fastest");
    CHECK(contains(output, "erro:"));
    CHECK(game.getTowerAt(cell)->getTargetPriority() == TargetPriority::Strongest);
}

TEST_CASE_FIXTURE(ShellTest, "help, ls, man and ranking print information") {
    shell.execute("help");
    CHECK(contains(output, "install"));

    output.str("");
    shell.execute("ls");
    CHECK_FALSE(output.str().empty());

    output.str("");
    shell.execute("man firewall");
    CHECK(contains(output, "Custo"));

    output.str("");
    shell.execute("man rootkit");
    CHECK(contains(output, "Vida"));

    output.str("");
    shell.execute("man dragon");
    CHECK(contains(output, "erro:"));

    output.str("");
    shell.execute("ranking");
    CHECK(contains(output, "=== RANKING ==="));

    output.str("");
    shell.execute("rank");
    CHECK(contains(output, "=== RANKING ==="));

    output.str("");
    shell.execute("status");
    CHECK(contains(output, "CPU: 200"));

    output.str("");
    shell.execute("map");
    CHECK(contains(output, "Legenda:"));

    output.str("");
    shell.execute("status");
    CHECK(contains(output, "CPU: 200"));

    output.str("");
    shell.execute("map");
    CHECK(contains(output, "Legenda:"));
}

TEST_CASE_FIXTURE(ShellTest, "run plays the whole wave") {
    CHECK(shell.execute("run"));

    CHECK(contains(output, "Onda 1 de 1"));
    CHECK(game.getState() == GameManager::GameState::Victory);
}

TEST_CASE_FIXTURE(ShellTest, "the screen explains how to play and what happens in a wave") {
    input.str("run\n\n");

    shell.run();

    CHECK(contains(output, "Como jogar"));
    CHECK(contains(output, "Onda 1 de 1 em andamento"));
    CHECK(contains(output, "Aguarde o fim da onda"));
    CHECK(contains(output, "Legenda:"));
}

TEST_CASE_FIXTURE(ShellTest, "run without a wave left shows an error") {
    REQUIRE(shell.execute("run"));
    output.str("");

    shell.execute("run");

    CHECK(contains(output, "erro:"));
}

TEST_CASE_FIXTURE(ShellTest, "the end of the input works as exit") {
    input.str("help\n");

    shell.run();

    CHECK(contains(output, "root@server:~# "));
    CHECK(game.getState() == GameManager::GameState::Preparing);
}

TEST_CASE_FIXTURE(ShellTest, "a whole game ends with the score saved in the ranking") {
    input.str("run\n  Thi;ago  \n");

    shell.run();

    CHECK(contains(output, "Vitória"));
    REQUIRE(scoreBoard.getEntries().size() == 1);
    CHECK(scoreBoard.getEntries()[0].name == "Thiago");
    CHECK(scoreBoard.getEntries()[0].score == game.getScore());

    ScoreBoard saved(RANKING_PATH);
    saved.load();
    CHECK(saved.getEntries().size() == 1);
}

TEST_CASE_FIXTURE(ShellTest, "an empty name is saved as anonimo") {
    input.str("run\n\n");

    shell.run();

    REQUIRE(scoreBoard.getEntries().size() == 1);
    CHECK(scoreBoard.getEntries()[0].name == "anonimo");
}

TEST_CASE_FIXTURE(ShellTest, "setFrameDelay rejects a negative delay") {
    CHECK_THROWS_AS(shell.setFrameDelay(-1), std::invalid_argument);
    CHECK_NOTHROW(shell.setFrameDelay(100));
}

} // TEST_SUITE("Shell")
