#include "Entity.hpp"

Entity::Entity(float startX, float startY, float width, float height)
    : position(startX, startY), size(width, height), active(true) {
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