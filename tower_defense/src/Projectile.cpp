#include "../include/Projectile.hpp"

#include "../include/Enemy.hpp"

#include <cmath>
#include <stdexcept>

Projectile::Projectile(const sf::Vector2f& startPosition, Enemy& target,
                       float speed, int damage)
    : enemy_(target),
      position_(startPosition),
      speed_(speed),
      damage_(damage),
      impacted_(false) {
    if (speed_ <= 0.0f || damage_ <= 0) {
        throw std::invalid_argument("A velocidade e o dano devem ser positivos.");
    }
}

void Projectile::update(float deltaTime) {
    if (impacted_ || deltaTime <= 0.0f) {
        return;
    }

    const sf::Vector2f targetPosition = enemy_.getPosition();
    const float dx = targetPosition.x - position_.x;
    const float dy = targetPosition.y - position_.y;
    const float distance = std::sqrt(dx * dx + dy * dy);

    if (distance == 0.0f) {
        hit();
        return;
    }

    const float travelDistance = speed_ * deltaTime;
    if (travelDistance >= distance) {
        position_ = targetPosition;
        hit();
        return;
    }

    const float movementRatio = travelDistance / distance;
    position_.x += dx * movementRatio;
    position_.y += dy * movementRatio;
}

void Projectile::render(sf::RenderWindow& window) {
    if (impacted_) {
        return;
    }

    sf::CircleShape shape(4.0f);
    shape.setFillColor(sf::Color::Red);
    shape.setPosition(position_.x - shape.getRadius(),
                      position_.y - shape.getRadius());
    window.draw(shape);
}

void Projectile::hit() {
    if (impacted_) {
        return;
    }

    enemy_.takeDamage(static_cast<float>(damage_));
    impacted_ = true;
}

Enemy& Projectile::getEnemy() const noexcept {
    return enemy_;
}

const sf::Vector2f& Projectile::getPosition() const noexcept {
    return position_;
}

void Projectile::setPosition(const sf::Vector2f& newPosition) noexcept {
    position_ = newPosition;
}

float Projectile::getSpeed() const noexcept {
    return speed_;
}

void Projectile::setSpeed(float newSpeed) {
    if (newSpeed <= 0.0f) {
        throw std::invalid_argument("A velocidade deve ser positiva.");
    }

    speed_ = newSpeed;
}

int Projectile::getDamage() const noexcept {
    return damage_;
}

void Projectile::setDamage(int newDamage) {
    if (newDamage <= 0) {
        throw std::invalid_argument("O dano deve ser positivo.");
    }

    damage_ = newDamage;
}

bool Projectile::hasImpacted() const noexcept {
    return impacted_;
}

void Projectile::setImpacted(bool impacted) noexcept {
    impacted_ = impacted;
}
