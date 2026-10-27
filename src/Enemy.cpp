#include "../include/Enemy.hpp"

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

bool Enemy::hasReachedEnd(const std::vector<sf::Vector2f>& waypoints) const {
    return false;
}