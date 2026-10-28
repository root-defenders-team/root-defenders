#include "Tower.hpp"

Tower::Tower(float startX, float startY, float width, float height, 
             float range, float damage, float fireRate, int cost)
    : Entity(startX, startY, width, height),
      _range(range), _damage(damage), _fireRate(fireRate), _fireCooldown(0.0f), _cost(cost), _level(1) {}

void Tower::upgrade() {
    _level++;
    _damage *= 1.25f;
    _range *= 1.1f;
}

float Tower::getRange() const { return _range; }
float Tower::getDamage() const { return _damage; }
int Tower::getCost() const { return _cost; }
int Tower::getLevel() const { return _level; }
float Tower::getFireRate() const { return _fireRate; }