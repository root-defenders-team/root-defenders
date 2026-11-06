#include "Map.hpp"

Map::Map(std::size_t width, std::size_t height)
    : width(width), height(height) {
    if (width == 0 || height == 0) {
        throw std::invalid_argument("As dimensoes do mapa devem ser maiores que zero.");
    }

    cells.assign(height, std::vector<CellType>(width, CellType::Buildable));
    occupiedCells.assign(height, std::vector<bool>(width, false));
}

std::size_t Map::getWidth() const {
    return width;
}

std::size_t Map::getHeight() const {
    return height;
}

bool Map::isInside(const Cell& cell) const {
    return cell.x >= 0 && cell.y >= 0 &&
           static_cast<std::size_t>(cell.x) < width &&
           static_cast<std::size_t>(cell.y) < height;
}

bool Map::isPath(const Cell& cell) const {
    return isInside(cell) && cells[cell.y][cell.x] == CellType::Path;
}

bool Map::isBlocked(const Cell& cell) const {
    return !isInside(cell) || cells[cell.y][cell.x] == CellType::Blocked;
}

bool Map::isBuildable(const Cell& cell) const {
    return isInside(cell) && cells[cell.y][cell.x] == CellType::Buildable &&
           !occupiedCells[cell.y][cell.x];
}

CellType Map::getCellType(const Cell& cell) const {
    if (!isInside(cell)) {
        throw std::out_of_range("A celula deve estar dentro do mapa.");
    }

    return cells[cell.y][cell.x];
}

void Map::setCellType(const Cell& cell, CellType type) {
    if (!isInside(cell)) {
        throw std::out_of_range("A celula deve estar dentro do mapa.");
    }

    cells[cell.y][cell.x] = type;
}

bool Map::occupyCell(const Cell& cell) {
    if (!isBuildable(cell)) {
        return false;
    }

    occupiedCells[cell.y][cell.x] = true;
    return true;
}

bool Map::releaseCell(const Cell& cell) {
    if (!isInside(cell) || !occupiedCells[cell.y][cell.x]) {
        return false;
    }

    occupiedCells[cell.y][cell.x] = false;
    return true;
}

void Map::addWaypoint(const Cell& waypoint) {
    if (!isInside(waypoint)) {
        throw std::out_of_range("O waypoint deve estar dentro do mapa.");
    }

    waypoints.push_back(waypoint);
    path.push_back(waypoint.toVector2());
}

const std::vector<Cell>& Map::getWaypoints() const {
    return waypoints;
}

const std::vector<Vector2>& Map::getPath() const {
    return path;
}

bool Map::hasPath() const {
    return waypoints.size() >= 2;
}
