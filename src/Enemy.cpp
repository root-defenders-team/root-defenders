#include "Enemy.hpp"

#include <cmath>
#include <stdexcept>

namespace {

const float DEFAULT_ENEMY_SIZE = 0.6f;
const int DEFAULT_DAMAGE_TO_ROOT = 10;

} // namespace

Enemy::Enemy(const std::vector<Vector2>& path, float maxHp, float speed,
             int reward, int damageToRoot)
    : Entity(path.empty() ? 0.0f : path.front().x,
             path.empty() ? 0.0f : path.front().y,
             DEFAULT_ENEMY_SIZE, DEFAULT_ENEMY_SIZE),
      path(path),
      hp(maxHp),
      maxHp(maxHp),
      speed(speed),
      reward(reward),
      damageToRoot(damageToRoot),
      waypointIndex(path.size() > 1 ? 1 : 0),
      distanceTravelled(0.0f) {
    if (path.size() < 2) {
        throw std::invalid_argument("Enemy path must contain at least 2 waypoints");
    }
    if (maxHp <= 0.0f) {
        throw std::invalid_argument("Enemy maxHp must be positive");
    }
    if (speed <= 0.0f) {
        throw std::invalid_argument("Enemy speed must be positive");
    }
    if (reward < 0) {
        throw std::invalid_argument("Enemy reward cannot be negative");
    }
    if (damageToRoot <= 0) {
        throw std::invalid_argument("Enemy damageToRoot must be positive");
    }
}

Enemy::Enemy(float startX, float startY, float width, float height,
             float hp, float speed, int goldReward)
    : Entity(startX, startY, width, height),
      path(fallbackPath_),
      hp(hp),
      maxHp(hp),
      speed(speed),
      reward(goldReward),
      damageToRoot(DEFAULT_DAMAGE_TO_ROOT),
      waypointIndex(0),
      distanceTravelled(0.0f),
      fallbackPath_({Vector2(startX, startY), Vector2(startX + 1000.0f, startY)}) {
}

void Enemy::update(float deltaTime) {
    if (!active || isDead() || hasReachedEnd() || deltaTime <= 0.0f) {
        return;
    }

    float remainingDistance = speed * deltaTime;
    float startRemaining = remainingDistance;

    while (remainingDistance > 0.0f && !hasReachedEnd()) {
        Vector2 target = path[waypointIndex];
        float distanceToTarget = position.distanceTo(target);

        if (distanceToTarget <= remainingDistance) {
            position = target;
            remainingDistance -= distanceToTarget;
            waypointIndex++;
        } else {
            Vector2 direction = target - position;
            Vector2 unitDirection = direction * (1.0f / distanceToTarget);
            position = position + (unitDirection * remainingDistance);
            remainingDistance = 0.0f;
        }
    }

    distanceTravelled += (startRemaining - remainingDistance);
}

void Enemy::moveAlongPath(const std::vector<Vector2>& waypoints, float deltaTime) {
    if (hasReachedEnd(waypoints) || deltaTime <= 0.0f) {
        return;
    }

    float remainingDistance = speed * deltaTime;
    float startRemaining = remainingDistance;

    while (remainingDistance > 0.0f && waypointIndex < waypoints.size()) {
        Vector2 target = waypoints[waypointIndex];
        float distanceToTarget = position.distanceTo(target);

        if (distanceToTarget <= remainingDistance) {
            position = target;
            remainingDistance -= distanceToTarget;
            waypointIndex++;
        } else {
            Vector2 direction = target - position;
            Vector2 unitDirection = direction * (1.0f / distanceToTarget);
            position = position + (unitDirection * remainingDistance);
            remainingDistance = 0.0f;
        }
    }

    distanceTravelled += (startRemaining - remainingDistance);
}

void Enemy::takeDamage(float amount) {
    if (amount < 0.0f) {
        throw std::invalid_argument("Enemy damage amount cannot be negative");
    }
    hp -= amount;
    if (hp <= 0.0f) {
        hp = 0.0f;
    }
}

bool Enemy::isDead() const {
    return hp <= 0.0f;
}

bool Enemy::hasReachedEnd() const {
    return waypointIndex >= path.size();
}

bool Enemy::hasReachedEnd(const std::vector<Vector2>& waypoints) const {
    return waypointIndex >= waypoints.size();
}

float Enemy::getHp() const {
    return hp;
}

float Enemy::getMaxHp() const {
    return maxHp;
}

float Enemy::getSpeed() const {
    return speed;
}

int Enemy::getReward() const {
    return reward;
}

int Enemy::getGoldReward() const {
    return reward;
}

int Enemy::getDamageToRoot() const {
    return damageToRoot;
}

float Enemy::getDistanceTravelled() const {
    return distanceTravelled;
}

std::size_t Enemy::getWaypointIndex() const {
    return waypointIndex;
}