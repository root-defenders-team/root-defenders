#include "Enemy.hpp"

Enemy::Enemy(float startX, float startY, float width, float height, float hp, float speed, int goldReward)
    : Entity(startX, startY, width, height), _hp(hp), _maxHp(hp), _speed(speed), _goldReward(goldReward), _waypointIndex(0) {}

void Enemy::takeDamage(float amount) {
    _hp -= amount; 
}

bool Enemy::isDead() const {
    return _hp <= 0;
}

int Enemy::getGoldReward() const {
    return _goldReward;
}

float Enemy::getHp() const {
    return _hp;
}

float Enemy::getSpeed() const {
    return _speed;
}

bool Enemy::hasReachedEnd(const std::vector<Vector2>& waypoints) const {
    return _waypointIndex >= waypoints.size();
}