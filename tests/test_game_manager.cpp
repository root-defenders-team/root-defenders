#include <doctest/doctest.h>

#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "Cell.hpp"
#include "CellType.hpp"
#include "Enemy.hpp"
#include "EnemyType.hpp"
#include "GameManager.hpp"
#include "MakeUnique.hpp"
#include "Map.hpp"
#include "Projectile.hpp"
#include "SpawnGroup.hpp"
#include "Tower.hpp"
#include "Wave.hpp"

namespace {

/// Defesa falsa: não atira e não melhora nada; serve só para testar as regras do jogo.
class FakeTower : public Tower {
public:
    FakeTower(const Cell& cell, int cost) : Tower(cell, 2.0f, 10.0f, 1.0f, cost) {}

    char getSymbol() const override {
        return 'T';
    }

protected:
    void fire(Enemy&, const std::vector<Enemy*>&,
              std::vector<std::unique_ptr<Projectile> >&) override {}

    void applyUpgrade() override {}
};

/// Cria uma defesa falsa na célula (x, y).
std::unique_ptr<Tower> makeTower(int x, int y, int cost) {
    Cell cell = {x, y};
    return util::makeUnique<FakeTower>(cell, cost);
}

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

TEST_CASE("builds a tower on a free cell") {
    GameManager game(makeTestMap(), makeTestWaves(1));
    Cell cell = {2, 0};

    CHECK(game.buildTower(makeTower(2, 0, 50)));
    CHECK(game.getGold() == GameManager::INITIAL_GOLD - 50);
    CHECK(game.getTowers().size() == 1);
    REQUIRE(game.getTowerAt(cell) != nullptr);
    CHECK(game.getTowerAt(cell)->getCost() == 50);
    CHECK_FALSE(game.getMap().isBuildable(cell));
}

TEST_CASE("does not build on the path or outside the map") {
    GameManager game(makeTestMap(), makeTestWaves(1));

    CHECK_FALSE(game.buildTower(makeTower(2, 1, 50)));
    CHECK_FALSE(game.buildTower(makeTower(9, 9, 50)));
    CHECK(game.getTowers().empty());
    CHECK(game.getGold() == GameManager::INITIAL_GOLD);
}

TEST_CASE("does not build on an occupied cell") {
    GameManager game(makeTestMap(), makeTestWaves(1));

    REQUIRE(game.buildTower(makeTower(2, 0, 50)));
    CHECK_FALSE(game.buildTower(makeTower(2, 0, 50)));
    CHECK(game.getTowers().size() == 1);
    CHECK(game.getGold() == GameManager::INITIAL_GOLD - 50);
}

TEST_CASE("does not build without enough gold") {
    GameManager game(makeTestMap(), makeTestWaves(1));
    Cell cell = {2, 0};

    CHECK_FALSE(game.buildTower(makeTower(2, 0, GameManager::INITIAL_GOLD + 1)));
    CHECK(game.getTowers().empty());
    CHECK(game.getGold() == GameManager::INITIAL_GOLD);
    CHECK(game.getMap().isBuildable(cell));
}

TEST_CASE("rejects a null tower") {
    GameManager game(makeTestMap(), makeTestWaves(1));
    std::unique_ptr<Tower> noTower;

    CHECK_FALSE(game.buildTower(std::move(noTower)));
    CHECK(game.getTowers().empty());
}

TEST_CASE("startNewGame removes the towers and frees their cells") {
    GameManager game(makeTestMap(), makeTestWaves(1));
    Cell cell = {2, 0};
    REQUIRE(game.buildTower(makeTower(2, 0, 50)));

    game.startNewGame();

    CHECK(game.getTowers().empty());
    CHECK(game.getTowerAt(cell) == nullptr);
    CHECK(game.getMap().isBuildable(cell));
}

TEST_CASE("upgrades a tower and charges the upgrade cost") {
    GameManager game(makeTestMap(), makeTestWaves(1));
    Cell cell = {2, 0};
    REQUIRE(game.buildTower(makeTower(2, 0, 100)));
    int upgradeCost = game.getTowerAt(cell)->getUpgradeCost();

    CHECK(game.upgradeTower(cell));
    CHECK(game.getTowerAt(cell)->getLevel() == 2);
    CHECK(game.getGold() == GameManager::INITIAL_GOLD - 100 - upgradeCost);
}

TEST_CASE("does not upgrade past the maximum level") {
    GameManager game(makeTestMap(), makeTestWaves(1));
    Cell cell = {2, 0};
    REQUIRE(game.buildTower(makeTower(2, 0, 10)));
    game.addGold(10000);
    for (int level = 1; level < Tower::MAX_LEVEL; ++level) {
        REQUIRE(game.upgradeTower(cell));
    }
    int goldAtMaxLevel = game.getGold();

    CHECK_FALSE(game.upgradeTower(cell));
    CHECK(game.getTowerAt(cell)->getLevel() == Tower::MAX_LEVEL);
    CHECK(game.getGold() == goldAtMaxLevel);
}

TEST_CASE("does not upgrade without enough gold") {
    GameManager game(makeTestMap(), makeTestWaves(1));
    Cell cell = {2, 0};
    REQUIRE(game.buildTower(makeTower(2, 0, GameManager::INITIAL_GOLD)));

    CHECK_FALSE(game.upgradeTower(cell));
    CHECK(game.getTowerAt(cell)->getLevel() == 1);
    CHECK(game.getGold() == 0);
}

TEST_CASE("sells a tower, refunds half and frees the cell") {
    GameManager game(makeTestMap(), makeTestWaves(1));
    Cell cell = {2, 0};
    REQUIRE(game.buildTower(makeTower(2, 0, 100)));

    CHECK(game.sellTower(cell));
    CHECK(game.getGold() == GameManager::INITIAL_GOLD - 100 + 50);
    CHECK(game.getTowerAt(cell) == nullptr);
    CHECK(game.getTowers().empty());
    CHECK(game.getMap().isBuildable(cell));
}

TEST_CASE("changes the target priority of a tower") {
    GameManager game(makeTestMap(), makeTestWaves(1));
    Cell cell = {2, 0};
    REQUIRE(game.buildTower(makeTower(2, 0, 50)));

    CHECK(game.setTargetPriority(cell, TargetPriority::Strongest));
    CHECK(game.getTowerAt(cell)->getTargetPriority() == TargetPriority::Strongest);
}

} // TEST_SUITE("GameManager")
