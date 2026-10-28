#include "Tower.hpp"
#include "Enemy.hpp"

Tower::Tower(float startX, float startY, float width, float height, 
             float range, float damage, float fireRate, int cost)
    : Entity(startX, startY, width, height),
      _range(range),
      _damage(damage),
      _fireRate(fireRate),
      _fireCooldown(0.0f),
      _cost(cost),
      _level(1) {}

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

float Tower::getRange() const { return _range; }
float Tower::getDamage() const { return _damage; }
int Tower::getCost() const { return _cost; }
int Tower::getLevel() const { return _level; }
float Tower::getFireRate() const { return _fireRate; }
float Tower::getCooldown() const { return _fireCooldown; }