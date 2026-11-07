#include "WaveLoader.hpp"

#include <cmath>
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>

#include "EnemyFactory.hpp"

namespace {

bool parseInteger(const std::string& text, int& value) {
    std::istringstream parser(text);
    char extra;
    return (parser >> value) && !(parser >> extra);
}

bool parseInterval(const std::string& text, float& value) {
    std::istringstream parser(text);
    char extra;
    return (parser >> value) && !(parser >> extra) &&
           std::isfinite(value) && value >= 0.0f;
}

} // namespace

std::vector<Wave> WaveLoader::load(const std::string& filePath) {
    std::ifstream file(filePath.c_str());

    if (!file.is_open()) {
        throw FileFormatException(filePath, 0, "nao foi possivel abrir o arquivo.");
    }

    return parse(file, filePath);
}

std::vector<Wave> WaveLoader::parse(std::istream& input, const std::string& sourceName) {
    std::vector<Wave> waves;
    std::vector<SpawnGroup> groups;
    std::string line;
    int lineNumber = 0;
    int currentWave = 0;
    int totalInWave = 0;

    while (std::getline(input, line)) {
        ++lineNumber;
        std::istringstream row(line);
        std::vector<std::string> fields;
        std::string field;

        while (row >> field) {
            fields.push_back(field);
        }

        if (fields.empty() || fields[0].compare(0, 2, "//") == 0) {
            continue;
        }

        if (fields.size() != 4) {
            throw FileFormatException(sourceName, lineNumber,
                                      "cada grupo deve ter quatro campos.");
        }

        int waveNumber;
        if (!parseInteger(fields[0], waveNumber) || waveNumber <= 0) {
            throw FileFormatException(sourceName, lineNumber,
                                      "numero de onda invalido.");
        }

        EnemyType type;
        try {
            type = EnemyFactory::parseType(fields[1]);
        } catch (const std::invalid_argument&) {
            throw FileFormatException(sourceName, lineNumber,
                                      "tipo de inimigo desconhecido.");
        }

        int count;
        if (!parseInteger(fields[2], count) || count <= 0) {
            throw FileFormatException(sourceName, lineNumber,
                                      "quantidade de inimigos invalida.");
        }

        float interval;
        if (!parseInterval(fields[3], interval)) {
            throw FileFormatException(sourceName, lineNumber,
                                      "intervalo invalido.");
        }

        if (waveNumber != currentWave) {
            if (waveNumber <= currentWave || waveNumber - currentWave != 1) {
                throw FileFormatException(sourceName, lineNumber,
                                          "ondas fora de ordem.");
            }
            if (currentWave != 0) {
                waves.push_back(Wave(groups));
                groups.clear();
                totalInWave = 0;
            }
            currentWave = waveNumber;
        }

        if (count > std::numeric_limits<int>::max() - totalInWave) {
            throw FileFormatException(sourceName, lineNumber,
                                      "quantidade total da onda excede o limite.");
        }

        groups.push_back(SpawnGroup{type, count, interval});
        totalInWave += count;
    }

    if (input.bad() || (input.fail() && !input.eof())) {
        throw FileFormatException(sourceName, lineNumber + 1,
                                  "falha ao ler as ondas.");
    }

    if (groups.empty()) {
        throw FileFormatException(sourceName, 0, "arquivo de ondas vazio.");
    }

    waves.push_back(Wave(groups));
    return waves;
}
