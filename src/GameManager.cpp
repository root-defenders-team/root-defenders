#include "GameManager.hpp"

#include <stdexcept>

#include "Enemy.hpp"
#include "Projectile.hpp"
#include "Tower.hpp"

const int GameManager::INITIAL_GOLD;
const int GameManager::INITIAL_INTEGRITY;

GameManager::GameManager(const Map& map, const std::vector<Wave>& waves)
    : map(map),
      waves(waves),
      gold(INITIAL_GOLD),
      rootIntegrity(INITIAL_INTEGRITY),
      score(0),
      currentWave(0),
      state(GameState::Preparing) {
    if (!map.hasPath()) {
        throw std::invalid_argument("O mapa precisa ter um caminho.");
    }
    if (waves.empty()) {
        throw std::invalid_argument("A partida precisa ter pelo menos uma onda.");
    }
}

GameManager::~GameManager() {
}

int GameManager::getGold() const {
    return gold;
}

void GameManager::addGold(int amount) {
    if (amount < 0) {
        throw std::invalid_argument("A quantidade de CPU não pode ser negativa.");
    }
    gold += amount;
}

bool GameManager::spendGold(int amount) {
    if (amount < 0 || amount > gold) {
        return false;
    }
    gold -= amount;
    return true;
}

void GameManager::damageRoot(int amount) {
    if (amount <= 0) {
        return;
    }
    rootIntegrity -= amount;
    if (rootIntegrity < 0) {
        rootIntegrity = 0;
    }
}

int GameManager::getRootIntegrity() const {
    return rootIntegrity;
}

int GameManager::getScore() const {
    return score;
}

std::size_t GameManager::getCurrentWave() const {
    return currentWave;
}

std::size_t GameManager::getTotalWaves() const {
    return waves.size();
}

GameManager::GameState GameManager::getState() const {
    return state;
}

const Map& GameManager::getMap() const {
    return map;
}

const std::vector<std::unique_ptr<Tower> >& GameManager::getTowers() const {
    return towers;
}

const std::vector<std::unique_ptr<Enemy> >& GameManager::getEnemies() const {
    return enemies;
}

const std::vector<std::unique_ptr<Projectile> >& GameManager::getProjectiles() const {
    return projectiles;
}
