#include "GameManager.hpp"

#include <stdexcept>
#include <utility>

#include "Enemy.hpp"
#include "EnemyFactory.hpp"
#include "Projectile.hpp"
#include "Tower.hpp"

namespace {

const int WAVE_BONUS_BASE = 20;      ///< CPU fixa ao concluir uma onda.
const int WAVE_BONUS_PER_WAVE = 5;   ///< CPU extra por número da onda concluída.
const int WAVE_SCORE = 100;          ///< Pontos ao concluir uma onda.
const int VICTORY_SCORE_FACTOR = 10; ///< Pontos por ponto de integridade na vitória.
const int KILL_SCORE_FACTOR = 10;    ///< Pontos por CPU de recompensa de um malware.

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

void GameManager::update(float deltaTime) {
    if (state != GameState::WaveRunning) {
        return;
    }

    // 1. A onda atual diz quem nasce neste tick.
    std::vector<EnemyType> born = waves[currentWave - 1].update(deltaTime);
    for (std::size_t i = 0; i < born.size(); ++i) {
        spawnEnemy(EnemyFactory::create(born[i], map.getPath()));
    }

    // 2. Os malwares andam pelo caminho.
    for (std::size_t i = 0; i < enemies.size(); ++i) {
        if (enemies[i]->isActive()) {
            enemies[i]->update(deltaTime);
        }
    }

    // 3. Quem chegou ao fim danifica o /root e sai do jogo, sem dar recompensa.
    for (std::size_t i = 0; i < enemies.size(); ++i) {
        if (enemies[i]->isActive() && enemies[i]->hasReachedEnd()) {
            damageRoot(enemies[i]->getDamageToRoot());
            enemies[i]->setActive(false);
        }
    }

    // 4. As defesas recarregam e atacam os malwares ativos.
    std::vector<Enemy*> activeEnemies;
    for (std::size_t i = 0; i < enemies.size(); ++i) {
        if (enemies[i]->isActive()) {
            activeEnemies.push_back(enemies[i].get());
        }
    }
    for (std::size_t i = 0; i < towers.size(); ++i) {
        towers[i]->update(deltaTime);
        towers[i]->tryAttack(activeEnemies, projectiles);
    }

    // 5. Os projéteis andam e acertam.
    for (std::size_t i = 0; i < projectiles.size(); ++i) {
        if (projectiles[i]->isActive()) {
            projectiles[i]->update(deltaTime);
        }
    }

    // 6. Malwares destruídos rendem CPU e pontos.
    for (std::size_t i = 0; i < enemies.size(); ++i) {
        if (enemies[i]->isActive() && enemies[i]->isDead()) {
            gold += enemies[i]->getReward();
            score += enemies[i]->getReward() * KILL_SCORE_FACTOR;
            enemies[i]->setActive(false);
        }
    }

    // 7. Remoção segura e 8. fim de onda, vitória e derrota.
    removeInactiveEntities();
    updateGameState();
}

void GameManager::spawnEnemy(std::unique_ptr<Enemy> enemy) {
    if (enemy) {
        enemies.push_back(std::move(enemy));
    }
}

void GameManager::removeInactiveEntities() {
    // (a) Um projétil cujo alvo saiu do jogo não pode mais acessá-lo.
    for (std::size_t i = 0; i < projectiles.size(); ++i) {
        const Enemy* target = projectiles[i]->getTarget();
        if (target != nullptr && !target->isActive()) {
            projectiles[i]->setActive(false);
        }
    }

    // (b) Apaga os projéteis inativos.
    std::size_t i = 0;
    while (i < projectiles.size()) {
        if (projectiles[i]->isActive()) {
            ++i;
        } else {
            projectiles.erase(projectiles.begin() + i);
        }
    }

    // (c) Só agora apaga os malwares inativos: nenhum projétil aponta mais para eles.
    i = 0;
    while (i < enemies.size()) {
        if (enemies[i]->isActive()) {
            ++i;
        } else {
            enemies.erase(enemies.begin() + i);
        }
    }
}

bool GameManager::buildTower(std::unique_ptr<Tower> tower) {
    if (!tower) {
        return false;
    }
    Cell cell = tower->getCell();
    if (!map.isBuildable(cell)) {
        return false;
    }
    if (!spendGold(tower->getCost())) {
        return false;
    }
    map.occupyCell(cell);
    towers.push_back(std::move(tower));
    return true;
}

bool GameManager::upgradeTower(const Cell& cell) {
    Tower* tower = findTower(cell);
    if (tower == nullptr || !tower->canUpgrade()) {
        return false;
    }
    // O custo é lido antes do upgrade, pois ele muda com o nível.
    if (!spendGold(tower->getUpgradeCost())) {
        return false;
    }
    tower->upgrade();
    return true;
}

bool GameManager::sellTower(const Cell& cell) {
    for (std::size_t i = 0; i < towers.size(); ++i) {
        if (towers[i]->getCell() == cell) {
            gold += towers[i]->getSellValue();
            map.releaseCell(cell);
            towers.erase(towers.begin() + i);
            return true;
        }
    }
    return false;
}

bool GameManager::setTargetPriority(const Cell& cell, TargetPriority priority) {
    Tower* tower = findTower(cell);
    if (tower == nullptr) {
        return false;
    }
    tower->setTargetPriority(priority);
    return true;
}

const Tower* GameManager::getTowerAt(const Cell& cell) const {
    return findTower(cell);
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

Tower* GameManager::findTower(const Cell& cell) const {
    for (std::size_t i = 0; i < towers.size(); ++i) {
        if (towers[i]->getCell() == cell) {
            return towers[i].get();
        }
    }
    return nullptr;
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
