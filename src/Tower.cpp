#include "Tower.hpp"

#include <cmath>
#include <stdexcept>

#include "Enemy.hpp"

const int Tower::MAX_LEVEL;

Tower::Tower(const Cell& cell, float range, float damage, float fireInterval, int cost)
    : Entity(cell.toVector2().x, cell.toVector2().y, 1.0f, 1.0f),
      cell(cell),
      range(range),
      damage(damage),
      fireInterval(fireInterval),
      cooldown(0.0f),
      cost(cost),
      level(1),
      investedGold(cost),
      targetPriority(TargetPriority::First) {
    if (range <= 0.0f) {
        throw std::invalid_argument("Tower range must be positive");
    }
    if (damage <= 0.0f) {
        throw std::invalid_argument("Tower damage must be positive");
    }
    if (fireInterval <= 0.0f) {
        throw std::invalid_argument("Tower fire interval must be positive");
    }
    if (cost <= 0) {
        throw std::invalid_argument("Tower cost must be positive");
    }
}

void Tower::update(float deltaTime) {
    if (cooldown > 0.0f && deltaTime > 0.0f) {
        cooldown -= deltaTime;
        if (cooldown < 0.0f) {
            cooldown = 0.0f;
        }
    }
}

void Tower::tryAttack(const std::vector<Enemy*>& enemies,
                      std::vector<std::unique_ptr<Projectile> >& projectiles) {
    if (cooldown > 0.0f) {
        return;
    }

    Enemy* target = selectTarget(enemies);
    if (target == nullptr) {
        return;
    }

    fire(*target, enemies, projectiles);
    cooldown = fireInterval;
}

bool Tower::isInRange(const Enemy& enemy) const {
    return getPosition().distanceTo(enemy.getPosition()) <= range;
}

bool Tower::canUpgrade() const {
    return level < MAX_LEVEL;
}

void Tower::upgrade() {
    if (!canUpgrade()) {
        throw std::logic_error("Tower already at maximum level");
    }
    investedGold += getUpgradeCost();
    level++;
    applyUpgrade();
}

int Tower::getUpgradeCost() const {
    return (cost * level * 3) / 4;
}

int Tower::getSellValue() const {
    return investedGold / 2;
}

Cell Tower::getCell() const {
    return cell;
}

float Tower::getRange() const {
    return range;
}

float Tower::getDamage() const {
    return damage;
}

float Tower::getFireInterval() const {
    return fireInterval;
}

float Tower::getCooldown() const {
    return cooldown;
}

int Tower::getCost() const {
    return cost;
}

int Tower::getLevel() const {
    return level;
}

TargetPriority Tower::getTargetPriority() const {
    return targetPriority;
}

void Tower::setTargetPriority(TargetPriority newPriority) {
    targetPriority = newPriority;
}

void Tower::setRange(float newRange) {
    if (newRange <= 0.0f) {
        throw std::invalid_argument("Tower range must be positive");
    }
    range = newRange;
}

void Tower::setDamage(float newDamage) {
    if (newDamage <= 0.0f) {
        throw std::invalid_argument("Tower damage must be positive");
    }
    damage = newDamage;
}

void Tower::setFireInterval(float newFireInterval) {
    if (newFireInterval <= 0.0f) {
        throw std::invalid_argument("Tower fire interval must be positive");
    }
    fireInterval = newFireInterval;
}

Enemy* Tower::selectTarget(const std::vector<Enemy*>& enemies) const {
    Enemy* selected = nullptr;

    for (std::size_t i = 0; i < enemies.size(); ++i) {
        Enemy* enemy = enemies[i];
        if (enemy == nullptr || !enemy->isActive() || enemy->isDead() || !isInRange(*enemy)) {
            continue;
        }

        if (selected == nullptr) {
            selected = enemy;
            continue;
        }

        switch (targetPriority) {
            case TargetPriority::First: {
                if (enemy->getDistanceTravelled() > selected->getDistanceTravelled()) {
                    selected = enemy;
                }
                break;
            }

            case TargetPriority::Strongest: {
                if (enemy->getHp() > selected->getHp()) {
                    selected = enemy;
                }
                break;
            }

            case TargetPriority::Weakest: {
                if (enemy->getHp() < selected->getHp()) {
                    selected = enemy;
                }
                break;
            }

            case TargetPriority::Closest: {
                float distEnemy = getPosition().distanceTo(enemy->getPosition());
                float distSelected = getPosition().distanceTo(selected->getPosition());
                if (distEnemy < distSelected) {
                    selected = enemy;
                }
                break;
            }
        }
    }

    return selected;
}