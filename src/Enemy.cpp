#include "Enemy.hpp"

Enemy::Enemy(float startX, float startY, float width, float height, float hp, float speed, int goldReward)
    : Entity(startX, startY, width, height),
      _hp(hp),
      _maxHp(hp),
      _speed(speed),
      _goldReward(goldReward),
      _waypointIndex(0) {}

void Enemy::update(float /*deltaTime*/) {
    // Comportamento genérico de atualização por frame.
    // Movimentação com rota específica é realizada via moveAlongPath.
}

void Enemy::moveAlongPath(const std::vector<Vector2>& waypoints, float deltaTime) {
    if (hasReachedEnd(waypoints) || deltaTime <= 0.0f) {
        return;
    }

    float remainingDistance = _speed * deltaTime;

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
}

void Enemy::takeDamage(float amount) {
    if (amount > 0.0f) {
        _hp -= amount;
        if (_hp < 0.0f) {
            _hp = 0.0f;
        }
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

bool Enemy::hasReachedEnd(const std::vector<Vector2>& waypoints) const {
    return _waypointIndex >= waypoints.size();
}