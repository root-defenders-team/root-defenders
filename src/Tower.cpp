#include "Tower.hpp"
#include "Enemy.hpp"

Tower::Tower(float startX, float startY, float width, float height, 
             float range, float damage, float fireRate, int cost,
             TargetPriority priority)
    : Entity(startX, startY, width, height),
      _range(range),
      _damage(damage),
      _fireRate(fireRate),
      _fireCooldown(0.0f),
      _cost(cost),
      _level(1),
      _targetPriority(priority) {}

void Tower::update(float deltaTime) {
    updateCooldown(deltaTime);
}

void Tower::upgrade() {
    _level++;
    _damage *= 1.25f;
    _range *= 1.10f;
    _cost = static_cast<int>(_cost * 1.5f);
}

bool Tower::isInRange(const Enemy& enemy) const {
    return position.distanceTo(enemy.getPosition()) <= _range;
}

bool Tower::canShoot() const {
    return _fireCooldown <= 0.0f;
}

void Tower::updateCooldown(float deltaTime) {
    if (_fireCooldown > 0.0f && deltaTime > 0.0f) {
        _fireCooldown -= deltaTime;
        if (_fireCooldown < 0.0f) {
            _fireCooldown = 0.0f;
        }
    }
}

void Tower::resetCooldown() {
    if (_fireRate > 0.0f) {
        _fireCooldown = 1.0f / _fireRate;
    } else {
        _fireCooldown = 0.0f;
    }
}

Enemy* Tower::selectTarget(const std::vector<Enemy*>& enemies) const {
    Enemy* selected = nullptr;

    for (Enemy* enemy : enemies) {
        if (!enemy || enemy->isDead() || !enemy->isActive() || !isInRange(*enemy)) {
            continue;
        }

        if (!selected) {
            selected = enemy;
            continue;
        }

        switch (_targetPriority) {
            case TargetPriority::First:
                if (enemy->getWaypointIndex() > selected->getWaypointIndex()) {
                    selected = enemy;
                }
                break;

            case TargetPriority::Strongest:
                if (enemy->getHp() > selected->getHp()) {
                    selected = enemy;
                }
                break;

            case TargetPriority::Weakest:
                if (enemy->getHp() < selected->getHp()) {
                    selected = enemy;
                }
                break;

            case TargetPriority::Closest: {
                float distEnemy = position.distanceTo(enemy->getPosition());
                float distSelected = position.distanceTo(selected->getPosition());
                if (distEnemy < distSelected) {
                    selected = enemy;
                }
                break;
            }
        }
    }

    return selected;
}

float Tower::getRange() const { return _range; }
float Tower::getDamage() const { return _damage; }
int Tower::getCost() const { return _cost; }
int Tower::getLevel() const { return _level; }
float Tower::getFireRate() const { return _fireRate; }
float Tower::getCooldown() const { return _fireCooldown; }
TargetPriority Tower::getTargetPriority() const { return _targetPriority; }
void Tower::setTargetPriority(TargetPriority priority) { _targetPriority = priority; }