#include "Cell.hpp"

bool Cell::operator==(const Cell& other) const {
    return x == other.x && y == other.y;
}

Vector2 Cell::toVector2() const {
    return Vector2(static_cast<float>(x), static_cast<float>(y));
}
