#include "Enemy.hpp"

#include <stdexcept>

Enemy::Enemy(float startX, float startY, float width, float height, float hp, float speed, int goldReward)
    : Entity(startX, startY, width, height),
      _hp(hp),
      _maxHp(hp),
      _speed(speed),
      _goldReward(goldReward),
      _waypointIndex(0),
      _distanceTravelled(0.0f) {}

void Enemy::update(float /*deltaTime*/) {
    // Comportamento genérico de atualização por frame.
    // Movimentação com rota específica é realizada via moveAlongPath.
}

void Enemy::moveAlongPath(const std::vector<Vector2>& waypoints, float deltaTime) {
    if (hasReachedEnd(waypoints) || deltaTime <= 0.0f) {
        return;
    }

    float remainingDistance = _speed * deltaTime;
    float startRemaining = remainingDistance;

    while (remainingDistance > 0.0f && !hasReachedEnd(waypoints)) {
        Vector2 target = waypoints[_waypointIndex];
        float distanceToTarget = position.distanceTo(target);

        if (distanceToTarget <= remainingDistance) {
            position = target;
            remainingDistance -= distanceToTarget;
            _waypointIndex++;
        } else {
            Vector2 direction = target - position;
            Vector2 unitDirection = direction * (1.0f / distanceToTarget);
            position = position + (unitDirection * remainingDistance);
            remainingDistance = 0.0f;
        }
    }

    _distanceTravelled += (startRemaining - remainingDistance);
}

void Enemy::takeDamage(float amount) {
    if (amount < 0.0f) {
        throw std::invalid_argument("Enemy damage amount cannot be negative");
    }
    _hp -= amount;
    if (_hp < 0.0f) {
        _hp = 0.0f;
    }
}

bool Enemy::isDead() const {
    return _hp <= 0.0f;
}

int Enemy::getGoldReward() const {
    return _goldReward;
}

float Enemy::getHp() const {
    return _hp;
}

float Enemy::getMaxHp() const {
    return _maxHp;
}

float Enemy::getSpeed() const {
    return _speed;
}

std::size_t Enemy::getWaypointIndex() const {
    return _waypointIndex;
}

float Enemy::getDistanceTravelled() const {
    return _distanceTravelled;
}

bool Enemy::hasReachedEnd(const std::vector<Vector2>& waypoints) const {
    return _waypointIndex >= waypoints.size();
}