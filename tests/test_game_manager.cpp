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

/// Malware falso, com vida e recompensa escolhidas pelo teste.
class FakeEnemy : public Enemy {
public:
    FakeEnemy(const std::vector<Vector2>& path, float hp, int reward)
        : Enemy(path, hp, 1.0f, reward, 10) {}

    char getSymbol() const override {
        return 'e';
    }
};

/// Cria um malware falso que anda pelo caminho do mapa da partida.
std::unique_ptr<Enemy> makeEnemy(const GameManager& game, float hp, int reward) {
    return util::makeUnique<FakeEnemy>(game.getMap().getPath(), hp, reward);
}

/// Defesa falsa que dispara projéteis muito lentos, para o teste decidir quando o alvo morre.
class SlowShooterTower : public Tower {
public:
    explicit SlowShooterTower(const Cell& cell) : Tower(cell, 2.0f, 1.0f, 1.0f, 10) {}

    char getSymbol() const override {
        return 'S';
    }

protected:
    void fire(Enemy& target, const std::vector<Enemy*>&,
              std::vector<std::unique_ptr<Projectile> >& projectiles) override {
        projectiles.push_back(util::makeUnique<Projectile>(getPosition(), target, 0.001f, 1.0f));
    }

    void applyUpgrade() override {}
};

/// Avança a partida até a onda atual acabar (no máximo 1000 ticks, para o teste nunca travar).
void runUntilWaveEnds(GameManager& game) {
    for (int tick = 0; tick < 1000; ++tick) {
        if (game.getState() != GameManager::GameState::WaveRunning) {
            return;
        }
        game.update(0.1f);
    }
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

TEST_CASE("spawns enemies") {
    GameManager game(makeTestMap(), makeTestWaves(1));

    game.spawnEnemy(makeEnemy(game, 10.0f, 5));
    game.spawnEnemy(makeEnemy(game, 10.0f, 5));

    CHECK(game.getEnemies().size() == 2);
}

TEST_CASE("ignores a null enemy") {
    GameManager game(makeTestMap(), makeTestWaves(1));
    std::unique_ptr<Enemy> noEnemy;

    game.spawnEnemy(std::move(noEnemy));

    CHECK(game.getEnemies().empty());
}

TEST_CASE("removes only the inactive enemies") {
    GameManager game(makeTestMap(), makeTestWaves(1));
    game.spawnEnemy(makeEnemy(game, 10.0f, 5));
    game.spawnEnemy(makeEnemy(game, 20.0f, 5));
    game.getEnemies()[0]->setActive(false);

    game.removeInactiveEntities();

    REQUIRE(game.getEnemies().size() == 1);
    CHECK(game.getEnemies()[0]->getMaxHp() == doctest::Approx(20.0f));
}

TEST_CASE("startNewGame removes the enemies") {
    GameManager game(makeTestMap(), makeTestWaves(1));
    game.spawnEnemy(makeEnemy(game, 10.0f, 5));

    game.startNewGame();

    CHECK(game.getEnemies().empty());
}

TEST_CASE("update does nothing before a wave starts") {
    GameManager game(makeTestMap(), makeTestWaves(1));

    game.update(0.1f);

    CHECK(game.getEnemies().empty());
    CHECK(game.getState() == GameManager::GameState::Preparing);
    CHECK(game.getGold() == GameManager::INITIAL_GOLD);
}

TEST_CASE("a wave spawns its first enemy on the first tick") {
    GameManager game(makeTestMap(), makeTestWaves(1));
    REQUIRE(game.startNextWave());

    game.update(0.1f);

    CHECK(game.getEnemies().size() == 1);
}

TEST_CASE("an enemy that reaches the end damages the root") {
    GameManager game(makeTestMap(), makeTestWaves(2));
    REQUIRE(game.startNextWave());
    game.update(0.1f);
    REQUIRE(game.getEnemies().size() == 1);
    int damage = game.getEnemies()[0]->getDamageToRoot();

    runUntilWaveEnds(game);

    CHECK(game.getRootIntegrity() == GameManager::INITIAL_INTEGRITY - damage);
    CHECK(game.getEnemies().empty());
    CHECK(game.getState() == GameManager::GameState::Preparing);
    CHECK(game.getGold() == GameManager::INITIAL_GOLD + 25);
    CHECK(game.getScore() == 100);
}

TEST_CASE("the last wave ends in victory") {
    GameManager game(makeTestMap(), makeTestWaves(1));
    REQUIRE(game.startNextWave());

    runUntilWaveEnds(game);

    REQUIRE(game.getState() == GameManager::GameState::Victory);
    CHECK(game.getScore() == 100 + game.getRootIntegrity() * 10);
    CHECK_FALSE(game.startNextWave());
}

TEST_CASE("the game is lost when the root integrity reaches zero") {
    GameManager game(makeTestMap(), makeTestWaves(2));
    REQUIRE(game.startNextWave());
    game.damageRoot(GameManager::INITIAL_INTEGRITY);

    game.update(0.1f);

    CHECK(game.getState() == GameManager::GameState::Defeat);
    CHECK_FALSE(game.startNextWave());
}

TEST_CASE("destroying an enemy gives gold and score") {
    GameManager game(makeTestMap(), makeTestWaves(2));
    REQUIRE(game.startNextWave());
    game.update(0.1f);
    REQUIRE(game.getEnemies().size() == 1);
    int reward = game.getEnemies()[0]->getReward();
    game.getEnemies()[0]->takeDamage(100000.0f);

    game.update(0.1f);

    CHECK(game.getEnemies().empty());
    CHECK(game.getGold() == GameManager::INITIAL_GOLD + reward + 25);
    CHECK(game.getScore() == reward * 10 + 100);
    CHECK(game.getState() == GameManager::GameState::Preparing);
}

TEST_CASE("projectiles never outlive their target") {
    GameManager game(makeTestMap(), makeTestWaves(2));
    Cell cell = {0, 0};
    REQUIRE(game.buildTower(util::makeUnique<SlowShooterTower>(cell)));
    REQUIRE(game.startNextWave());
    game.update(0.1f);
    REQUIRE(game.getEnemies().size() == 1);
    REQUIRE(game.getProjectiles().size() == 1);
    game.getEnemies()[0]->takeDamage(100000.0f);

    game.update(0.1f);

    CHECK(game.getEnemies().empty());
    CHECK(game.getProjectiles().empty());
}

} // TEST_SUITE("GameManager")
