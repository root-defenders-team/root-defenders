#include "Projectile.hpp"

#include "Enemy.hpp"

#include <cmath>
#include <stdexcept>

namespace {

const float PROJECTILE_SIZE = 0.2f;

} // namespace

Projectile::Projectile(const Vector2& startPosition, Enemy& target,
                       float speed, float damage)
    : Entity(startPosition.x, startPosition.y, PROJECTILE_SIZE, PROJECTILE_SIZE),
      target(&target),
      speed(speed),
      damage(damage),
      impacted(false) {
    if (!std::isfinite(this->speed) || this->speed <= 0.0f ||
        !std::isfinite(this->damage) || this->damage <= 0.0f) {
        throw std::invalid_argument(
            "A velocidade e o dano devem ser positivos e finitos.");
    }
}

void Projectile::update(float deltaTime) {
    if (!active || impacted) {
        return;
    }

    if (target == nullptr || !target->isActive() || target->isDead()) {
        setActive(false);
        return;
    }

    if (deltaTime <= 0.0f) {
        return;
    }

    const Vector2 targetPosition = target->getPosition();
    const Vector2 direction = targetPosition - position;
    const float distance = direction.length();
    const float travelDistance = speed * deltaTime;

    if (distance <= travelDistance) {
        position = targetPosition;
        hit();
        return;
    }

    if (distance > 0.0f) {
        position = position + direction * (travelDistance / distance);
    }
}

char Projectile::getSymbol() const {
    return '*';
}

void Projectile::hit() {
    if (!active || impacted) {
        return;
    }

    if (target == nullptr || !target->isActive() || target->isDead()) {
        setActive(false);
        return;
    }

    target->takeDamage(damage);
    impacted = true;
    setActive(false);
}

bool Projectile::hasImpacted() const {
    return impacted;
}

const Enemy* Projectile::getTarget() const {
    return target;
}

float Projectile::getSpeed() const {
    return speed;
}

float Projectile::getDamage() const {
    return damage;
}

void Projectile::setSpeed(float newSpeed) {
    if (!std::isfinite(newSpeed) || newSpeed <= 0.0f) {
        throw std::invalid_argument(
            "A velocidade deve ser positiva e finita.");
    }

    speed = newSpeed;
}

void Projectile::setDamage(float newDamage) {
    if (!std::isfinite(newDamage) || newDamage <= 0.0f) {
        throw std::invalid_argument("O dano deve ser positivo e finito.");
    }

    damage = newDamage;
}
