#include "Map.hpp"

Map::Map(std::size_t width, std::size_t height)
    : width_(width), height_(height) {
    if (width_ == 0 || height_ == 0) {
        throw std::invalid_argument("As dimensoes do mapa devem ser maiores que zero.");
    }

    traversableCells_.assign(height_, std::vector<bool>(width_, true));
    occupiedCells_.assign(height_, std::vector<bool>(width_, false));
}

std::size_t Map::width() const noexcept {
    return width_;
}

std::size_t Map::height() const noexcept {
    return height_;
}

bool Map::isInside(int x, int y) const noexcept {
    return x >= 0 && y >= 0 && static_cast<std::size_t>(x) < width_ &&
           static_cast<std::size_t>(y) < height_;
}

bool Map::isTraversable(int x, int y) const noexcept {
    return isInside(x, y) && traversableCells_[y][x];
}

bool Map::isBlocked(int x, int y) const noexcept {
    return !isTraversable(x, y);
}

bool Map::setTraversable(int x, int y, bool traversable) noexcept {
    if (!isInside(x, y)) {
        return false;
    }

    if (!traversable && occupiedCells_[y][x]) {
        return false;
    }

    traversableCells_[y][x] = traversable;
    return true;
}

bool Map::isCellFree(int x, int y) const noexcept {
    return isInside(x, y) && !occupiedCells_[y][x];
}

bool Map::occupyCell(int x, int y) noexcept {
    if (!isCellFree(x, y)) {
        return false;
    }

    occupiedCells_[y][x] = true;
    return true;
}

bool Map::releaseCell(int x, int y) noexcept {
    if (!isInside(x, y) || !occupiedCells_[y][x]) {
        return false;
    }

    occupiedCells_[y][x] = false;
    return true;
}

void Map::addWaypoint(Point waypoint) {
    if (!isInside(waypoint.x, waypoint.y)) {
        throw std::out_of_range("O waypoint deve estar dentro do mapa.");
    }

    waypoints_.push_back(waypoint);
}

const std::vector<Map::Point>& Map::waypoints() const noexcept {
    return waypoints_;
}
