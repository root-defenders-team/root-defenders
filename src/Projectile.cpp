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
      target_(&target),
      speed_(speed),
      damage_(damage),
      impacted_(false) {
    if (!std::isfinite(speed_) || speed_ <= 0.0f ||
        !std::isfinite(damage_) || damage_ <= 0.0f) {
        throw std::invalid_argument(
            "A velocidade e o dano devem ser positivos e finitos.");
    }
}

void Projectile::update(float deltaTime) {
    if (!active || impacted_) {
        return;
    }

    if (target_ == nullptr || !target_->isActive() || target_->isDead()) {
        setActive(false);
        return;
    }

    if (deltaTime <= 0.0f) {
        return;
    }

    const Vector2 targetPosition = target_->getPosition();
    const Vector2 direction = targetPosition - position;
    const float distance = direction.length();
    const float travelDistance = speed_ * deltaTime;

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
    if (!active || impacted_) {
        return;
    }

    if (target_ == nullptr || !target_->isActive() || target_->isDead()) {
        setActive(false);
        return;
    }

    target_->takeDamage(damage_);
    impacted_ = true;
    setActive(false);
}

bool Projectile::hasImpacted() const {
    return impacted_;
}

const Enemy* Projectile::getTarget() const {
    return target_;
}

float Projectile::getSpeed() const {
    return speed_;
}

float Projectile::getDamage() const {
    return damage_;
}

void Projectile::setSpeed(float newSpeed) {
    if (!std::isfinite(newSpeed) || newSpeed <= 0.0f) {
        throw std::invalid_argument(
            "A velocidade deve ser positiva e finita.");
    }

    speed_ = newSpeed;
}

void Projectile::setDamage(float newDamage) {
    if (!std::isfinite(newDamage) || newDamage <= 0.0f) {
        throw std::invalid_argument("O dano deve ser positivo e finito.");
    }

    damage_ = newDamage;
}
