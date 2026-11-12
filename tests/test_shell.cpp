#include <doctest/doctest.h>

#include <cstdio>
#include <fstream>
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

TEST_CASE_FIXTURE(ShellTest, "install without enough CPU shows the missing amount") {
    REQUIRE(game.spendGold(GameManager::INITIAL_GOLD - 30));

    shell.execute("install antivirus 2 0");

    CHECK(output.str() ==
          "erro: CPU insuficiente: a torre Antivírus custa 100 e você possui apenas 30; "
          "faltam 70 CPU. Derrote malwares e avance para conseguir mais.\n");
    CHECK(game.getTowers().empty());
    CHECK(game.getGold() == 30);
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

TEST_CASE_FIXTURE(ShellTest, "man accepts a malware name with only its first letter capitalized") {
    CHECK(shell.execute("man Rootkit"));

    CHECK(contains(output, "Rootkit"));
    CHECK(contains(output, "Vida"));
    CHECK_FALSE(contains(output, "erro:"));
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

TEST_CASE_FIXTURE(ShellTest, "negative coordinates are rejected across defense commands") {
    CHECK(shell.execute("install firewall -1 0"));
    CHECK(contains(output, "fora do mapa"));
    CHECK(game.getTowers().empty());
    CHECK(game.getGold() == GameManager::INITIAL_GOLD);

    output.str("");
    CHECK(shell.execute("install firewall 0 -2"));
    CHECK(contains(output, "fora do mapa"));
    CHECK(game.getTowers().empty());

    output.str("");
    CHECK(shell.execute("upgrade -1 0"));
    CHECK(contains(output, "não há defesa em (-1, 0)"));

    output.str("");
    CHECK(shell.execute("rm 0 -1"));
    CHECK(contains(output, "não há defesa em (0, -1)"));

    output.str("");
    CHECK(shell.execute("target -2 -3 strong"));
    CHECK(contains(output, "não há defesa em (-2, -3)"));
}

TEST_CASE_FIXTURE(ShellTest, "upgrade without enough CPU shows an error and keeps level") {
    Cell cell = {2, 0};
    REQUIRE(shell.execute("install firewall 2 0"));
    REQUIRE(game.spendGold(game.getGold() - 20));
    output.str("");

    shell.execute("upgrade 2 0");

    CHECK(contains(output, "CPU insuficiente"));
    CHECK(game.getTowerAt(cell)->getLevel() == 1);
    CHECK(game.getGold() == 20);
}

TEST_CASE_FIXTURE(ShellTest, "rm after upgrade refunds half of total invested CPU") {
    Cell cell = {2, 0};
    REQUIRE(shell.execute("install firewall 2 0"));
    REQUIRE(shell.execute("upgrade 2 0"));
    int goldBefore = game.getGold();
    int expectedRefund = (50 + 37) / 2;
    output.str("");

    shell.execute("rm 2 0");

    CHECK(game.getTowerAt(cell) == nullptr);
    CHECK(game.getGold() == goldBefore + expectedRefund);
    CHECK(contains(output, "CPU devolvida: 43"));
}

TEST_CASE_FIXTURE(ShellTest, "target rejects missing tower and accepts all four priorities") {
    Cell cell = {2, 0};
    shell.execute("target 2 0 first");
    CHECK(contains(output, "não há defesa em (2, 0)"));

    REQUIRE(shell.execute("install firewall 2 0"));
    shell.execute("target 2 0 strong");
    CHECK(game.getTowerAt(cell)->getTargetPriority() == TargetPriority::Strongest);
    shell.execute("target 2 0 weak");
    CHECK(game.getTowerAt(cell)->getTargetPriority() == TargetPriority::Weakest);
    shell.execute("target 2 0 close");
    CHECK(game.getTowerAt(cell)->getTargetPriority() == TargetPriority::Closest);
    shell.execute("target 2 0 first");
    CHECK(game.getTowerAt(cell)->getTargetPriority() == TargetPriority::First);
}

TEST_CASE_FIXTURE(ShellTest, "man covers defenses and malwares and bad arg counts show usage") {
    shell.execute("man antivirus");
    CHECK(contains(output, "Antivírus:"));
    output.str("");
    shell.execute("man scanner");
    CHECK(contains(output, "Scanner:"));
    output.str("");
    shell.execute("man Antivirus");
    CHECK(contains(output, "Custo: 100"));
    output.str("");
    shell.execute("man virus");
    CHECK(contains(output, "Vírus:"));
    output.str("");
    shell.execute("man worm");
    CHECK(contains(output, "Dano ao /root: 5"));

    output.str("");
    shell.execute("upgrade 2");
    CHECK(contains(output, "uso: upgrade <x> <y>"));
    output.str("");
    shell.execute("target 2 0");
    CHECK(contains(output, "uso: target <x> <y> <prioridade>"));
    output.str("");
    shell.execute("help extra");
    CHECK(contains(output, "uso: help"));
}

TEST_CASE("run ends in defeat when root integrity reaches zero") {
    std::remove(RANKING_PATH);
    std::vector<SpawnGroup> groups;
    groups.push_back(SpawnGroup{EnemyType::Rootkit, 4, 0.0f});
    std::vector<Wave> lethalWaves(1, Wave(groups));

    GameManager game(makeTestMap(), lethalWaves);
    ScoreBoard scoreBoard(RANKING_PATH);
    std::istringstream input("run\n  Admin;Com_Nome_Muito_Longo  \n");
    std::ostringstream output;
    Shell shell(game, scoreBoard, input, output);
    shell.setFrameDelay(0);

    shell.run();

    CHECK(game.getState() == GameManager::GameState::Defeat);
    CHECK(game.getRootIntegrity() == 0);
    CHECK(contains(output, "Derrota! A integridade do /root chegou a 0."));
    CHECK(contains(output, "Pontuação final: 0"));
    REQUIRE(scoreBoard.getEntries().size() == 1);
    CHECK(scoreBoard.getEntries()[0].name == "AdminCom_Nome_Muito_");
    CHECK(scoreBoard.getEntries()[0].score == 0);
    std::remove(RANKING_PATH);
}

TEST_CASE("defeat with full higher ranking skips name prompt") {
    std::remove(RANKING_PATH);
    std::vector<SpawnGroup> groups;
    groups.push_back(SpawnGroup{EnemyType::Rootkit, 4, 0.0f});
    std::vector<Wave> lethalWaves(1, Wave(groups));

    GameManager game(makeTestMap(), lethalWaves);
    ScoreBoard scoreBoard(RANKING_PATH);
    for (int i = 1; i <= 5; ++i) {
        ScoreEntry entry = {"top", i * 100, 1};
        scoreBoard.addEntry(entry);
    }
    std::istringstream input("run\n");
    std::ostringstream output;
    Shell shell(game, scoreBoard, input, output);
    shell.setFrameDelay(0);

    shell.run();

    CHECK(game.getState() == GameManager::GameState::Defeat);
    CHECK(contains(output, "Derrota! A integridade do /root chegou a 0."));
    CHECK(output.str().find("Digite seu nome:") == std::string::npos);
    CHECK(scoreBoard.getEntries().size() == 5);
    CHECK(scoreBoard.getEntries()[4].score == 100);
    std::remove(RANKING_PATH);
}

TEST_CASE("run warns about ignored ranking lines and ranking save failure") {
    std::remove(RANKING_PATH);
    {
        std::ofstream badRanking(RANKING_PATH);
        badRanking << "200;2;alice\nlinha_corrompida\n;;;\n";
    }
    GameManager firstGame(makeTestMap(), makeTestWaves());
    ScoreBoard loadedBoard(RANKING_PATH);
    loadedBoard.load();
    std::istringstream firstInput("exit\n");
    std::ostringstream firstOutput;
    Shell firstShell(firstGame, loadedBoard, firstInput, firstOutput);
    firstShell.setFrameDelay(0);

    firstShell.run();

    CHECK(contains(firstOutput, "2 linha(s) corrompida(s) do ranking foram ignoradas"));
    std::remove(RANKING_PATH);

    GameManager secondGame(makeTestMap(), makeTestWaves());
    ScoreBoard unwritableBoard("build/pasta_inexistente_shell/ranking.txt");
    std::istringstream secondInput("run\nvitor\n");
    std::ostringstream secondOutput;
    Shell secondShell(secondGame, unwritableBoard, secondInput, secondOutput);
    secondShell.setFrameDelay(0);

    secondShell.run();

    CHECK(contains(secondOutput, "aviso: o ranking não foi salvo:"));
}

} // TEST_SUITE("Shell")
