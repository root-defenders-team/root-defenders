#include "Map.hpp"

Map::Map(std::size_t width, std::size_t height)
    : width_(width), height_(height) {
    if (width_ == 0 || height_ == 0) {
        throw std::invalid_argument("As dimensoes do mapa devem ser maiores que zero.");
    }

    cells_.assign(height_, std::vector<CellType>(width_, CellType::Buildable));
    occupiedCells_.assign(height_, std::vector<bool>(width_, false));
}

std::size_t Map::getWidth() const {
    return width_;
}

std::size_t Map::getHeight() const {
    return height_;
}

bool Map::isInside(const Cell& cell) const {
    return cell.x >= 0 && cell.y >= 0 &&
           static_cast<std::size_t>(cell.x) < width_ &&
           static_cast<std::size_t>(cell.y) < height_;
}

bool Map::isPath(const Cell& cell) const {
    return isInside(cell) && cells_[cell.y][cell.x] == CellType::Path;
}

bool Map::isBlocked(const Cell& cell) const {
    return !isInside(cell) || cells_[cell.y][cell.x] == CellType::Blocked;
}

bool Map::isBuildable(const Cell& cell) const {
    return isInside(cell) && cells_[cell.y][cell.x] == CellType::Buildable &&
           !occupiedCells_[cell.y][cell.x];
}

CellType Map::getCellType(const Cell& cell) const {
    if (!isInside(cell)) {
        throw std::out_of_range("A celula deve estar dentro do mapa.");
    }

    return cells_[cell.y][cell.x];
}

void Map::setCellType(const Cell& cell, CellType type) {
    if (!isInside(cell)) {
        throw std::out_of_range("A celula deve estar dentro do mapa.");
    }

    cells_[cell.y][cell.x] = type;
}

bool Map::occupyCell(const Cell& cell) {
    if (!isBuildable(cell)) {
        return false;
    }

    occupiedCells_[cell.y][cell.x] = true;
    return true;
}

bool Map::releaseCell(const Cell& cell) {
    if (!isInside(cell) || !occupiedCells_[cell.y][cell.x]) {
        return false;
    }

    occupiedCells_[cell.y][cell.x] = false;
    return true;
}

void Map::addWaypoint(const Cell& waypoint) {
    if (!isInside(waypoint)) {
        throw std::out_of_range("O waypoint deve estar dentro do mapa.");
    }

    waypoints_.push_back(waypoint);
    path_.push_back(waypoint.toVector2());
}

const std::vector<Cell>& Map::getWaypoints() const {
    return waypoints_;
}

const std::vector<Vector2>& Map::getPath() const {
    return path_;
}

bool Map::hasPath() const {
    return waypoints_.size() >= 2;
}
