#include <cmath>

#include "Vector2.hpp"

Vector2::Vector2() : x(0.0f), y(0.0f) {}

Vector2::Vector2(float x, float y) : x(x), y(y) {}

float Vector2::length() const {
    return std::sqrt(x * x + y * y);
}

float Vector2::distanceTo(const Vector2& other) const {
    return (other - *this).length();
}

Vector2 Vector2::operator+(const Vector2& other) const {
    return Vector2(x + other.x, y + other.y);
}

Vector2 Vector2::operator-(const Vector2& other) const {
    return Vector2(x - other.x, y - other.y);
}

Vector2 Vector2::operator*(float scalar) const {
    return Vector2(x * scalar, y * scalar);
}
