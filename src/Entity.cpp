#include "Entity.hpp"

#include <stdexcept>

Entity::Entity(float startX, float startY, float width, float height)
    : position(startX, startY), size(width, height), active(true) {
    if (width <= 0.0f || height <= 0.0f) {
        throw std::invalid_argument("Largura e altura da entidade devem ser maiores que zero.");
    }
}

char Entity::getSymbol() const {
    return ' ';
}

Rect Entity::getBounds() const {
    return Rect{position.x, position.y, size.x, size.y};
}

bool Entity::checkCollision(const Entity& other) const {
    return this->getBounds().intersects(other.getBounds());
}

bool Entity::isActive() const {
    return active;
}

void Entity::setActive(bool state) {
    active = state;
}

Vector2 Entity::getPosition() const {
    return position;
}

void Entity::setPosition(const Vector2& newPosition) {
    position = newPosition;
}

char Entity::getSymbol() const {
    return ' ';
}
