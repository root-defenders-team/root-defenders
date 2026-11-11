#include "ScoreBoard.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "GameException.hpp"

const std::size_t ScoreBoard::MAX_ENTRIES;

namespace {

/// Ordena da maior para a menor pontuação.
bool hasHigherScore(const ScoreEntry& first, const ScoreEntry& second) {
    return first.score > second.score;
}

/// Lê um inteiro não negativo; false se o texto não for exatamente isso.
bool readNonNegative(const std::string& text, int& value) {
    std::istringstream input(text);
    char extra = '\0';
    if (!(input >> value) || (input >> extra)) {
        return false;
    }
    return value >= 0;
}

/// Lê uma linha "<pontos>;<onda>;<nome>"; false se ela estiver corrompida.
bool readEntry(const std::string& line, ScoreEntry& entry) {
    std::size_t firstSeparator = line.find(';');
    if (firstSeparator == std::string::npos) {
        return false;
    }
    std::size_t secondSeparator = line.find(';', firstSeparator + 1);
    if (secondSeparator == std::string::npos) {
        return false;
    }
    std::string scoreText = line.substr(0, firstSeparator);
    std::string waveText = line.substr(firstSeparator + 1, secondSeparator - firstSeparator - 1);
    entry.name = line.substr(secondSeparator + 1);
    return readNonNegative(scoreText, entry.score) && readNonNegative(waveText, entry.wave) &&
           !entry.name.empty() && entry.name.find(';') == std::string::npos;
}

} // namespace

ScoreBoard::ScoreBoard(const std::string& filePath) : filePath(filePath), ignoredLines(0) {
}

void ScoreBoard::load() {
    entries.clear();
    ignoredLines = 0;

    std::ifstream file(filePath.c_str());
    if (!file) {
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Arquivo salvo no Windows deixa um '\r' no fim de cada linha.
        if (!line.empty() && line[line.size() - 1] == '\r') {
            line.erase(line.size() - 1);
        }
        if (line.empty()) {
            continue;
        }
        ScoreEntry entry;
        if (readEntry(line, entry)) {
            entries.push_back(entry);
        } else {
            ++ignoredLines;
        }
    }

    std::stable_sort(entries.begin(), entries.end(), hasHigherScore);
    if (entries.size() > MAX_ENTRIES) {
        entries.resize(MAX_ENTRIES);
    }
}

void ScoreBoard::save() const {
    std::ofstream file(filePath.c_str());
    if (!file) {
        throw GameException("não foi possível gravar o ranking em " + filePath);
    }
    for (std::size_t i = 0; i < entries.size(); ++i) {
        file << entries[i].score << ';' << entries[i].wave << ';' << entries[i].name << '\n';
    }
    file.close();
    if (!file) {
        throw GameException("não foi possível gravar o ranking em " + filePath);
    }
}

bool ScoreBoard::qualifies(int score) const {
    return entries.size() < MAX_ENTRIES || score > entries.back().score;
}

void ScoreBoard::addEntry(const ScoreEntry& entry) {
    if (entry.name.empty() || entry.name.find(';') != std::string::npos) {
        throw std::invalid_argument("O nome do ranking não pode ser vazio nem ter ';'.");
    }
    if (entry.score < 0 || entry.wave < 0) {
        throw std::invalid_argument("Pontuação e onda do ranking não podem ser negativas.");
    }

    std::size_t position = 0;
    while (position < entries.size() && entries[position].score >= entry.score) {
        ++position;
    }
    entries.insert(entries.begin() + position, entry);
    if (entries.size() > MAX_ENTRIES) {
        entries.pop_back();
    }
}

const std::vector<ScoreEntry>& ScoreBoard::getEntries() const {
    return entries;
}

int ScoreBoard::getHighScore() const {
    if (entries.empty()) {
        return 0;
    }
    return entries[0].score;
}

int ScoreBoard::getIgnoredLines() const {
    return ignoredLines;
}
