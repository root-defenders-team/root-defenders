#include <doctest/doctest.h>

#include <stdexcept>
#include <vector>

#include "Cell.hpp"
#include "CellType.hpp"
#include "EnemyType.hpp"
#include "GameManager.hpp"
#include "Map.hpp"
#include "SpawnGroup.hpp"
#include "Wave.hpp"

namespace {

/// Mapa 5x3 com o caminho na linha do meio, da coluna 0 até a coluna 4.
Map makeTestMap() {
    Map map(5, 3);
    for (int x = 0; x < 5; ++x) {
        Cell cell = {x, 1};
        map.setCellType(cell, CellType::Path);
        map.addWaypoint(cell);
    }
    return map;
}

/// Cria a quantidade pedida de ondas, cada uma com um único Virus.
std::vector<Wave> makeTestWaves(int count) {
    std::vector<Wave> waves;
    for (int i = 0; i < count; ++i) {
        std::vector<SpawnGroup> groups;
        SpawnGroup group = {EnemyType::Virus, 1, 0.0f};
        groups.push_back(group);
        waves.push_back(Wave(groups));
    }
    return waves;
}

} // namespace

TEST_SUITE("GameManager") {

TEST_CASE("rejects a map without path") {
    Map emptyMap(5, 3);
    CHECK_THROWS_AS(GameManager(emptyMap, makeTestWaves(1)), std::invalid_argument);

    Map onePointMap(5, 3);
    Cell cell = {0, 1};
    onePointMap.addWaypoint(cell);
    CHECK_THROWS_AS(GameManager(onePointMap, makeTestWaves(1)), std::invalid_argument);
}

TEST_CASE("rejects an empty list of waves") {
    std::vector<Wave> noWaves;
    CHECK_THROWS_AS(GameManager(makeTestMap(), noWaves), std::invalid_argument);
}

TEST_CASE("accepts a valid map and waves") {
    CHECK_NOTHROW(GameManager(makeTestMap(), makeTestWaves(1)));
}

TEST_CASE("starts with the initial values") {
    GameManager game(makeTestMap(), makeTestWaves(3));

    CHECK(game.getGold() == GameManager::INITIAL_GOLD);
    CHECK(game.getRootIntegrity() == GameManager::INITIAL_INTEGRITY);
    CHECK(game.getScore() == 0);
    CHECK(game.getCurrentWave() == 0);
    CHECK(game.getTotalWaves() == 3);
    CHECK(game.getState() == GameManager::GameState::Preparing);
    CHECK(game.getTowers().empty());
    CHECK(game.getEnemies().empty());
    CHECK(game.getProjectiles().empty());
}

TEST_CASE("keeps a copy of the map") {
    GameManager game(makeTestMap(), makeTestWaves(1));

    CHECK(game.getMap().getWidth() == 5);
    CHECK(game.getMap().getHeight() == 3);
    CHECK(game.getMap().hasPath());
}

TEST_CASE("adds gold") {
    GameManager game(makeTestMap(), makeTestWaves(1));

    game.addGold(50);
    CHECK(game.getGold() == GameManager::INITIAL_GOLD + 50);

    game.addGold(0);
    CHECK(game.getGold() == GameManager::INITIAL_GOLD + 50);
}

TEST_CASE("rejects adding a negative amount of gold") {
    GameManager game(makeTestMap(), makeTestWaves(1));

    CHECK_THROWS_AS(game.addGold(-1), std::invalid_argument);
    CHECK(game.getGold() == GameManager::INITIAL_GOLD);
}

TEST_CASE("spends gold when there is enough") {
    GameManager game(makeTestMap(), makeTestWaves(1));

    CHECK(game.spendGold(80));
    CHECK(game.getGold() == GameManager::INITIAL_GOLD - 80);

    CHECK(game.spendGold(GameManager::INITIAL_GOLD - 80));
    CHECK(game.getGold() == 0);
}

TEST_CASE("does not spend more gold than available") {
    GameManager game(makeTestMap(), makeTestWaves(1));

    CHECK_FALSE(game.spendGold(GameManager::INITIAL_GOLD + 1));
    CHECK(game.getGold() == GameManager::INITIAL_GOLD);
}

TEST_CASE("does not spend a negative amount of gold") {
    GameManager game(makeTestMap(), makeTestWaves(1));

    CHECK_FALSE(game.spendGold(-10));
    CHECK(game.getGold() == GameManager::INITIAL_GOLD);
}

TEST_CASE("damages the root") {
    GameManager game(makeTestMap(), makeTestWaves(1));

    game.damageRoot(30);
    CHECK(game.getRootIntegrity() == GameManager::INITIAL_INTEGRITY - 30);
}

TEST_CASE("root integrity never goes below zero") {
    GameManager game(makeTestMap(), makeTestWaves(1));

    game.damageRoot(GameManager::INITIAL_INTEGRITY + 50);
    CHECK(game.getRootIntegrity() == 0);
}

TEST_CASE("starts the first wave") {
    GameManager game(makeTestMap(), makeTestWaves(2));

    CHECK(game.startNextWave());
    CHECK(game.getCurrentWave() == 1);
    CHECK(game.getState() == GameManager::GameState::WaveRunning);
}

TEST_CASE("does not start a wave while another is running") {
    GameManager game(makeTestMap(), makeTestWaves(2));

    REQUIRE(game.startNextWave());
    CHECK_FALSE(game.startNextWave());
    CHECK(game.getCurrentWave() == 1);
}

TEST_CASE("no wave is unfinished before the first one starts") {
    GameManager game(makeTestMap(), makeTestWaves(1));

    CHECK(game.isWaveFinished());
}

TEST_CASE("a wave is not finished right after it starts") {
    GameManager game(makeTestMap(), makeTestWaves(1));

    REQUIRE(game.startNextWave());
    CHECK_FALSE(game.isWaveFinished());
}

TEST_CASE("startNewGame lets the first wave start again") {
    GameManager game(makeTestMap(), makeTestWaves(2));
    REQUIRE(game.startNextWave());

    game.startNewGame();

    CHECK(game.startNextWave());
    CHECK(game.getCurrentWave() == 1);
    CHECK(game.getState() == GameManager::GameState::WaveRunning);
}

TEST_CASE("startNewGame restores the initial values") {
    GameManager game(makeTestMap(), makeTestWaves(2));
    game.spendGold(100);
    game.damageRoot(40);
    game.startNextWave();

    game.startNewGame();

    CHECK(game.getGold() == GameManager::INITIAL_GOLD);
    CHECK(game.getRootIntegrity() == GameManager::INITIAL_INTEGRITY);
    CHECK(game.getScore() == 0);
    CHECK(game.getCurrentWave() == 0);
    CHECK(game.getState() == GameManager::GameState::Preparing);
}

TEST_CASE("has no tower on an empty cell") {
    GameManager game(makeTestMap(), makeTestWaves(1));
    Cell cell = {2, 0};

    CHECK(game.getTowerAt(cell) == nullptr);
    CHECK_FALSE(game.upgradeTower(cell));
    CHECK_FALSE(game.sellTower(cell));
    CHECK_FALSE(game.setTargetPriority(cell, TargetPriority::Strongest));
    CHECK(game.getGold() == GameManager::INITIAL_GOLD);
}

} // TEST_SUITE("GameManager")
