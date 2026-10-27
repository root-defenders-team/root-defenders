#include "Entity.hpp"

Entity::Entity(float startX, float startY, float width, float height)
    : position(startX, startY), size(width, height), active(true) {
}

sf::FloatRect Entity::getBounds() const {
    return sf::FloatRect(position.x, position.y, size.x, size.y);
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

sf::Vector2f Entity::getPosition() const {
    return position;
}

void Entity::setPosition(const sf::Vector2f& newPosition) {
    position = newPosition;
}
