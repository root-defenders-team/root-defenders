#include "Rect.hpp"

bool Rect::intersects(const Rect& other) const {
    return left < other.left + other.width
        && other.left < left + width
        && top < other.top + other.height
        && other.top < top + height;
}
