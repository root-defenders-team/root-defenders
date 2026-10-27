#include "GameManager.hpp"

#include <stdexcept>

#include "Enemy.hpp"
#include "Projectile.hpp"
#include "Tower.hpp"

namespace {

const int WAVE_BONUS_BASE = 20;      ///< CPU fixa ao concluir uma onda.
const int WAVE_BONUS_PER_WAVE = 5;   ///< CPU extra por número da onda concluída.
const int WAVE_SCORE = 100;          ///< Pontos ao concluir uma onda.
const int VICTORY_SCORE_FACTOR = 10; ///< Pontos por ponto de integridade na vitória.

} // namespace

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

void GameManager::startNewGame() {
    // Os malwares saem primeiro: eles guardam uma referência para o caminho do mapa.
    enemies.clear();
    projectiles.clear();
    for (std::size_t i = 0; i < towers.size(); ++i) {
        map.releaseCell(towers[i]->getCell());
    }
    towers.clear();

    for (std::size_t i = 0; i < waves.size(); ++i) {
        waves[i].reset();
    }

    gold = INITIAL_GOLD;
    rootIntegrity = INITIAL_INTEGRITY;
    score = 0;
    currentWave = 0;
    state = GameState::Preparing;
}

bool GameManager::startNextWave() {
    if (state != GameState::Preparing || currentWave >= waves.size()) {
        return false;
    }
    waves[currentWave].reset();
    ++currentWave;
    state = GameState::WaveRunning;
    return true;
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

bool GameManager::isWaveFinished() const {
    if (currentWave == 0) {
        return true;
    }
    if (!waves[currentWave - 1].isFinishedSpawning()) {
        return false;
    }
    for (std::size_t i = 0; i < enemies.size(); ++i) {
        if (enemies[i]->isActive()) {
            return false;
        }
    }
    return true;
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

void GameManager::updateGameState() {
    if (state != GameState::WaveRunning) {
        return;
    }
    if (rootIntegrity == 0) {
        state = GameState::Defeat;
        return;
    }
    if (!isWaveFinished()) {
        return;
    }

    gold += WAVE_BONUS_BASE + WAVE_BONUS_PER_WAVE * static_cast<int>(currentWave);
    score += WAVE_SCORE;

    if (currentWave == waves.size()) {
        score += rootIntegrity * VICTORY_SCORE_FACTOR;
        state = GameState::Victory;
    } else {
        state = GameState::Preparing;
    }
}
