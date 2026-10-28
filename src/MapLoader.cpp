#include "MapLoader.hpp"

#include <fstream>
#include <stdexcept>
#include <vector>

namespace {

std::runtime_error formatError(const std::string& sourceName, int lineNumber,
                               const std::string& message) {
    return std::runtime_error(sourceName + ", linha " +
                              std::to_string(lineNumber) + ": " + message);
}

void removeCarriageReturn(std::string& line) {
    if (!line.empty() && line[line.size() - 1] == '\r') {
        line.erase(line.size() - 1);
    }
}

void setCellType(Map& map, const Cell& cell, char symbol) {
    if (symbol == 'X') {
        map.setCellType(cell, CellType::Blocked);
    } else if (symbol == '#' || symbol == 'E' || symbol == '@') {
        map.setCellType(cell, CellType::Path);
    }
}

void addPathWaypoints(Map& map, const Cell& entry, const Cell& destination,
                      const std::string& sourceName) {
    std::vector<std::vector<bool> > visited(
        map.getHeight(), std::vector<bool>(map.getWidth(), false));
    const int horizontalSteps[] = {0, 1, 0, -1};
    const int verticalSteps[] = {-1, 0, 1, 0};
    Cell current = entry;

    visited[current.y][current.x] = true;
    map.addWaypoint(current);

    while (!(current == destination)) {
        Cell next = {-1, -1};
        int nextCount = 0;

        for (int index = 0; index < 4; ++index) {
            Cell candidate = {current.x + horizontalSteps[index],
                              current.y + verticalSteps[index]};

            if (map.isInside(candidate) && map.isPath(candidate) &&
                !visited[candidate.y][candidate.x]) {
                next = candidate;
                ++nextCount;
            }
        }

        if (nextCount == 0) {
            throw formatError(sourceName, current.y + 1,
                              "caminho interrompido.");
        }

        if (nextCount > 1) {
            throw formatError(sourceName, current.y + 1,
                              "caminho com bifurcacao.");
        }

        current = next;
        visited[current.y][current.x] = true;
        map.addWaypoint(current);
    }

    for (std::size_t y = 0; y < map.getHeight(); ++y) {
        for (std::size_t x = 0; x < map.getWidth(); ++x) {
            Cell cell = {static_cast<int>(x), static_cast<int>(y)};

            if (map.isPath(cell) && !visited[y][x]) {
                throw formatError(sourceName, static_cast<int>(y) + 1,
                                  "caminho desconectado.");
            }
        }
    }
}

} // namespace

Map MapLoader::load(const std::string& filePath) {
    std::ifstream file(filePath.c_str());

    if (!file.is_open()) {
        throw formatError(filePath, 0, "nao foi possivel abrir o arquivo.");
    }

    return parse(file, filePath);
}

Map MapLoader::parse(std::istream& input, const std::string& sourceName) {
    std::vector<std::string> lines;
    std::string line;
    std::size_t width = 0;
    int lineNumber = 0;

    while (std::getline(input, line)) {
        ++lineNumber;
        removeCarriageReturn(line);

        if (line.empty()) {
            throw formatError(sourceName, lineNumber, "linha vazia.");
        }

        if (width == 0) {
            width = line.size();
        } else if (line.size() != width) {
            throw formatError(sourceName, lineNumber,
                              "todas as linhas devem ter o mesmo tamanho.");
        }

        lines.push_back(line);
    }

    if (lines.empty()) {
        throw formatError(sourceName, 0, "arquivo de mapa vazio.");
    }

    Map map(width, lines.size());
    Cell entry = {-1, -1};
    Cell destination = {-1, -1};
    int entryCount = 0;
    int destinationCount = 0;

    for (std::size_t y = 0; y < lines.size(); ++y) {
        for (std::size_t x = 0; x < lines[y].size(); ++x) {
            Cell cell = {static_cast<int>(x), static_cast<int>(y)};
            const char symbol = lines[y][x];

            if (symbol != '.' && symbol != '#' && symbol != 'X' &&
                symbol != 'E' && symbol != '@') {
                throw formatError(sourceName, static_cast<int>(y) + 1,
                                  "caractere desconhecido.");
            }

            setCellType(map, cell, symbol);

            if (symbol == 'E') {
                entry = cell;
                ++entryCount;
            }

            if (symbol == '@') {
                destination = cell;
                ++destinationCount;
            }
        }
    }

    if (entryCount != 1) {
        throw formatError(sourceName, 0, "o mapa deve possuir uma unica entrada.");
    }

    if (destinationCount != 1) {
        throw formatError(sourceName, 0, "o mapa deve possuir um unico destino.");
    }

    addPathWaypoints(map, entry, destination, sourceName);
    return map;
}
