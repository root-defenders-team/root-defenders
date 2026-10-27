#include <doctest/doctest.h>

#include <filesystem>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

#include "Enemy.hpp"
#include "GameManager.hpp"
#include "Projectile.hpp"
#include "Tower.hpp"

namespace {

class FakeTower : public Tower {
public:
    explicit FakeTower(int cost)
        : Tower(0.0f, 0.0f, 1.0f, 1.0f, 2.0f, 10.0f, 1.0f, cost) {}

    void update(float) override {}

    void render(sf::RenderWindow&) override {}

    void attack(std::vector<Enemy*>&, std::vector<Projectile*>&, float) override {}
};

class FakeEnemy : public Enemy {
public:
    FakeEnemy(float hp, int goldReward)
        : Enemy(0.0f, 0.0f, 1.0f, 1.0f, hp, 1.0f, goldReward) {}

    void update(float) override {}

    void render(sf::RenderWindow&) override {}
};

class GameManagerTest {
protected:
    static constexpr std::size_t kWidth = 10;
    static constexpr std::size_t kHeight = 8;

    GameManagerTest()
        : highScorePath_(std::filesystem::temp_directory_path()
              / "tower_defense_game_manager_test_highscore.txt") {
        removeHighScoreFile();
    }

    ~GameManagerTest() {
        removeHighScoreFile();
    }

    GameManagerTest(const GameManagerTest&) = delete;
    GameManagerTest& operator=(const GameManagerTest&) = delete;

    std::string highScorePath() const {
        return highScorePath_.string();
    }

    void writeHighScoreFile(const std::string& content) const {
        std::ofstream file(highScorePath_);
        file << content;
    }

    int readHighScoreFile() const {
        std::ifstream file(highScorePath_);
        int value = -1;
        file >> value;
        return value;
    }

    static FakeEnemy* spawnFakeEnemy(GameManager& game, float hp, int goldReward) {
        auto enemy = std::make_unique<FakeEnemy>(hp, goldReward);
        FakeEnemy* raw = enemy.get();
        game.spawnEnemy(std::move(enemy));
        return raw;
    }

    static void defeatEnemy(GameManager& game, FakeEnemy& enemy) {
        enemy.takeDamage(1000.0f);
        game.update(0.0f);
        game.removeInactiveEntities();
    }

private:
    void removeHighScoreFile() const {
        std::error_code error;
        std::filesystem::remove(highScorePath_, error);
    }

    std::filesystem::path highScorePath_;
};

}

TEST_SUITE("GameManager") {

TEST_CASE_FIXTURE(GameManagerTest, "rejects maps without positive dimensions") {
    CHECK_THROWS_AS(GameManager(0, kHeight, highScorePath()), std::invalid_argument);
    CHECK_THROWS_AS(GameManager(kWidth, 0, highScorePath()), std::invalid_argument);
    CHECK_THROWS_AS(GameManager(0, 0, highScorePath()), std::invalid_argument);

    CHECK_NOTHROW(GameManager(1, 1, highScorePath()));
}

TEST_CASE_FIXTURE(GameManagerTest, "starts a running game with initial resources") {
    GameManager game(kWidth, kHeight, highScorePath());

    CHECK(game.state() == GameManager::GameState::Running);
    CHECK(game.gold() > 0);
    CHECK(game.score() == 0);
    CHECK(game.enemiesLeaked() == 0);
    CHECK(game.isWaveFinished());
}

TEST_CASE_FIXTURE(GameManagerTest, "adds gold to the player") {
    GameManager game(kWidth, kHeight, highScorePath());
    const int initialGold = game.gold();

    game.addGold(50);
    CHECK(game.gold() == initialGold + 50);

    game.addGold(0);
    CHECK(game.gold() == initialGold + 50);
}

TEST_CASE_FIXTURE(GameManagerTest, "spends gold only when the player can afford it") {
    GameManager game(kWidth, kHeight, highScorePath());
    const int initialGold = game.gold();

    CHECK_FALSE(game.spendGold(initialGold + 1));
    CHECK(game.gold() == initialGold);

    CHECK(game.spendGold(initialGold));
    CHECK(game.gold() == 0);

    CHECK_FALSE(game.spendGold(1));
    CHECK(game.gold() == 0);
}

TEST_CASE_FIXTURE(GameManagerTest, "rejects negative gold spending") {
    GameManager game(kWidth, kHeight, highScorePath());
    const int initialGold = game.gold();

    CHECK_FALSE(game.spendGold(-10));
    CHECK(game.gold() == initialGold);
}

TEST_CASE_FIXTURE(GameManagerTest, "builds a tower on a free cell and charges its cost") {
    GameManager game(kWidth, kHeight, highScorePath());
    const int initialGold = game.gold();
    const int cost = initialGold / 2;

    CHECK(game.buildTower(std::make_unique<FakeTower>(cost), 2, 3));
    CHECK(game.gold() == initialGold - cost);
}

TEST_CASE_FIXTURE(GameManagerTest, "does not build a tower on an occupied cell") {
    GameManager game(kWidth, kHeight, highScorePath());
    game.addGold(100);

    REQUIRE(game.buildTower(std::make_unique<FakeTower>(10), 4, 4));
    const int goldAfterFirstTower = game.gold();

    CHECK_FALSE(game.buildTower(std::make_unique<FakeTower>(10), 4, 4));
    CHECK(game.gold() == goldAfterFirstTower);
}

TEST_CASE_FIXTURE(GameManagerTest, "does not build a tower outside the map") {
    GameManager game(kWidth, kHeight, highScorePath());
    const int initialGold = game.gold();
    const int width = static_cast<int>(kWidth);
    const int height = static_cast<int>(kHeight);

    CHECK_FALSE(game.buildTower(std::make_unique<FakeTower>(1), -1, 0));
    CHECK_FALSE(game.buildTower(std::make_unique<FakeTower>(1), 0, -1));
    CHECK_FALSE(game.buildTower(std::make_unique<FakeTower>(1), width, 0));
    CHECK_FALSE(game.buildTower(std::make_unique<FakeTower>(1), 0, height));
    CHECK(game.gold() == initialGold);
}

TEST_CASE_FIXTURE(GameManagerTest, "does not build a tower without enough gold") {
    GameManager game(kWidth, kHeight, highScorePath());
    const int initialGold = game.gold();

    CHECK_FALSE(game.buildTower(std::make_unique<FakeTower>(initialGold + 1), 1, 1));
    CHECK(game.gold() == initialGold);

    CHECK(game.buildTower(std::make_unique<FakeTower>(initialGold), 1, 1));
    CHECK(game.gold() == 0);
}

TEST_CASE_FIXTURE(GameManagerTest, "does not build a null tower") {
    GameManager game(kWidth, kHeight, highScorePath());
    const int initialGold = game.gold();

    CHECK_FALSE(game.buildTower(nullptr, 1, 1));
    CHECK(game.gold() == initialGold);
}

TEST_CASE_FIXTURE(GameManagerTest, "starts the first wave and creates its enemies") {
    GameManager game(kWidth, kHeight, highScorePath());

    CHECK(game.startNextWave());
    CHECK(game.currentWave() == 1);
    CHECK_FALSE(game.isWaveFinished());
}

TEST_CASE_FIXTURE(GameManagerTest, "keeps the wave unfinished while an enemy is active") {
    GameManager game(kWidth, kHeight, highScorePath());

    FakeEnemy* enemy = spawnFakeEnemy(game, 50.0f, 5);
    CHECK_FALSE(game.isWaveFinished());

    game.removeInactiveEntities();
    CHECK_FALSE(game.isWaveFinished());

    enemy->setActive(false);
    game.removeInactiveEntities();
    CHECK(game.isWaveFinished());
}

TEST_CASE_FIXTURE(GameManagerTest, "rewards gold and score when an enemy is defeated") {
    GameManager game(kWidth, kHeight, highScorePath());
    const int initialGold = game.gold();

    FakeEnemy* enemy = spawnFakeEnemy(game, 10.0f, 25);
    defeatEnemy(game, *enemy);

    CHECK(game.gold() == initialGold + 25);
    CHECK(game.score() > 0);
    CHECK(game.isWaveFinished());
}

TEST_CASE_FIXTURE(GameManagerTest, "updates without entities keep the game unchanged") {
    GameManager game(kWidth, kHeight, highScorePath());
    const int initialGold = game.gold();

    game.update(0.016f);

    CHECK(game.state() == GameManager::GameState::Running);
    CHECK(game.gold() == initialGold);
    CHECK(game.score() == 0);
    CHECK(game.enemiesLeaked() == 0);
}

TEST_CASE_FIXTURE(GameManagerTest, "counts enemies that leaked through the path") {
    GameManager game(kWidth, kHeight, highScorePath());

    game.registerEnemyLeak();
    CHECK(game.enemiesLeaked() == 1);

    game.registerEnemyLeak();
    CHECK(game.enemiesLeaked() == 2);
}

TEST_CASE_FIXTURE(GameManagerTest, "keeps running while no enemy has leaked") {
    GameManager game(kWidth, kHeight, highScorePath());

    game.updateGameState();

    CHECK(game.state() == GameManager::GameState::Running);
}

TEST_CASE_FIXTURE(GameManagerTest, "is not won while waves remain") {
    GameManager game(kWidth, kHeight, highScorePath());

    REQUIRE(game.startNextWave());
    game.updateGameState();

    CHECK(game.state() != GameManager::GameState::Victory);
}

TEST_CASE_FIXTURE(GameManagerTest, "ends in defeat when the leak limit is reached") {
    GameManager game(kWidth, kHeight, highScorePath());

    for (int i = 0; i < 1000 && game.state() == GameManager::GameState::Running; ++i) {
        game.registerEnemyLeak();
        game.updateGameState();
    }

    CHECK(game.state() == GameManager::GameState::Defeat);
    CHECK(game.enemiesLeaked() > 0);
}

TEST_CASE_FIXTURE(GameManagerTest, "starting a new game resets the match") {
    GameManager game(kWidth, kHeight, highScorePath());
    const int initialGold = game.gold();

    game.addGold(100);
    REQUIRE(game.buildTower(std::make_unique<FakeTower>(10), 0, 0));
    spawnFakeEnemy(game, 10.0f, 5);
    game.registerEnemyLeak();
    game.startNextWave();

    game.startNewGame();

    CHECK(game.state() == GameManager::GameState::Running);
    CHECK(game.gold() == initialGold);
    CHECK(game.score() == 0);
    CHECK(game.enemiesLeaked() == 0);
    CHECK(game.isWaveFinished());
    CHECK(game.buildTower(std::make_unique<FakeTower>(1), 0, 0));
}

TEST_CASE_FIXTURE(GameManagerTest, "starting a new game recovers from a defeat") {
    GameManager game(kWidth, kHeight, highScorePath());

    for (int i = 0; i < 1000 && game.state() == GameManager::GameState::Running; ++i) {
        game.registerEnemyLeak();
        game.updateGameState();
    }
    REQUIRE(game.state() == GameManager::GameState::Defeat);

    game.startNewGame();

    CHECK(game.state() == GameManager::GameState::Running);
    CHECK(game.enemiesLeaked() == 0);
}

TEST_CASE_FIXTURE(GameManagerTest, "starts with zero high score when the file does not exist") {
    GameManager game(kWidth, kHeight, highScorePath());

    CHECK(game.highScore() == 0);
}

TEST_CASE_FIXTURE(GameManagerTest, "loads the high score from the file on construction") {
    writeHighScoreFile("150");

    GameManager game(kWidth, kHeight, highScorePath());

    CHECK(game.highScore() == 150);
}

TEST_CASE_FIXTURE(GameManagerTest, "reloads the high score after the file changes") {
    GameManager game(kWidth, kHeight, highScorePath());
    REQUIRE(game.highScore() == 0);

    writeHighScoreFile("320");
    game.loadHighScore();

    CHECK(game.highScore() == 320);
}

TEST_CASE_FIXTURE(GameManagerTest, "does not replace a higher record when saving") {
    writeHighScoreFile("500");
    GameManager game(kWidth, kHeight, highScorePath());

    game.saveHighScore();

    CHECK(game.highScore() == 500);
    CHECK(readHighScoreFile() == 500);
}

TEST_CASE_FIXTURE(GameManagerTest, "saves a new record that survives a new execution") {
    int recordScore = 0;
    {
        GameManager game(kWidth, kHeight, highScorePath());
        FakeEnemy* enemy = spawnFakeEnemy(game, 10.0f, 25);
        defeatEnemy(game, *enemy);
        recordScore = game.score();
        REQUIRE(recordScore > 0);

        game.saveHighScore();

        CHECK(game.highScore() == recordScore);
        CHECK(readHighScoreFile() == recordScore);
    }

    GameManager nextExecution(kWidth, kHeight, highScorePath());

    CHECK(nextExecution.highScore() == recordScore);
}

}
