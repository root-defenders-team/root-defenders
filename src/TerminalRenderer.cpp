#include "TerminalRenderer.hpp"

#include <cmath>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "Cell.hpp"
#include "CellType.hpp"
#include "Enemy.hpp"
#include "GameManager.hpp"
#include "Map.hpp"
#include "Projectile.hpp"
#include "ScoreBoard.hpp"
#include "ScoreEntry.hpp"
#include "TargetPriority.hpp"
#include "Tower.hpp"
#include "Vector2.hpp"

namespace {

const char* priorityText(TargetPriority priority) {
    switch (priority) {
        case TargetPriority::First:
            return "first";
        case TargetPriority::Strongest:
            return "strong";
        case TargetPriority::Weakest:
            return "weak";
        case TargetPriority::Closest:
            return "close";
    }
    return "first";
}

const char* RESET_COLOR = "\033[0m";    ///< Volta o terminal para a cor padrão.
const char* BOLD_CYAN = "\033[1;36m";   ///< Destaque em ciano para títulos e bordas.
const char* BOLD_YELLOW = "\033[1;33m"; ///< Destaque em amarelo para CPU.
const char* BOLD_GREEN = "\033[1;32m";  ///< Destaque em verde para integridade alta.
const char* BOLD_RED = "\033[1;31m";    ///< Destaque em vermelho para integridade crítica.
const char* BOLD_MAGENTA = "\033[1;35m";///< Destaque em magenta para pontuação.
const char* DIM_COLOR = "\033[90m";     ///< Cinza discreto para coordenadas e divisórias.

/// Escolhe a cor da integridade conforme o estado do /root.
const char* integrityColorFor(int integrity) {
    if (integrity >= 70) {
        return BOLD_GREEN;
    }
    if (integrity >= 35) {
        return BOLD_YELLOW;
    }
    return BOLD_RED;
}

/// Cor ANSI de cada símbolo do mapa; "" para os que ficam na cor padrão.
const char* colorFor(char symbol) {
    switch (symbol) {
        case '.':
            return "\033[90m";   // célula livre: cinza discreto
        case '#':
            return "\033[33m";   // caminho: amarelo
        case 'X':
            return "\033[90m";   // obstáculo: cinza
        case 'E':
            return "\033[1;36m"; // entrada: ciano
        case '@':
            return "\033[1;35m"; // /root: magenta
        case 'F':
        case 'A':
        case 'S':
            return "\033[1;32m"; // defesas: verde
        case 'v':
        case 'w':
        case 'r':
            return "\033[1;31m"; // malwares: vermelho
        case '*':
            return "\033[1;33m"; // projétil: amarelo forte
    }
    return "";
}

/// Escreve o número da linha alinhado em 2 posições, seguido de um espaço.
void writeRowNumber(std::size_t y, bool useColors, std::ostream& out) {
    if (useColors) {
        out << DIM_COLOR;
    }
    if (y < 10) {
        out << ' ';
    }
    out << y << ' ';
    if (useColors) {
        out << RESET_COLOR;
    }
}

/// Escreve os números das colunas: a dezena (só se o mapa passar de 10) e a unidade.
void writeColumnNumbers(std::size_t width, bool useColors, std::ostream& out) {
    if (useColors) {
        out << DIM_COLOR;
    }
    if (width > 10) {
        out << "   ";
        for (std::size_t x = 0; x < width; ++x) {
            if (x < 10) {
                out << ' ';
            } else {
                out << (x / 10) % 10;
            }
        }
        out << "\n";
    }
    out << "   ";
    for (std::size_t x = 0; x < width; ++x) {
        out << x % 10;
    }
    if (useColors) {
        out << RESET_COLOR;
    }
    out << "\n";
}

} // namespace

TerminalRenderer::TerminalRenderer(bool useColors)
    : useColors(useColors) {
}

void TerminalRenderer::drawFrame(const GameManager& game, std::ostream& out) const {
    out << "\033[2J\033[H";
    drawHeader(game, out);
    drawMap(game, out);
    if (useColors) {
        out << DIM_COLOR << "------------------------------------------------------------"
            << RESET_COLOR << "\n"
            << BOLD_CYAN << "Legenda:" << RESET_COLOR
            << " \033[1;36mE\033[0m entrada | \033[1;35m@\033[0m /root | "
            << "\033[33m#\033[0m caminho | \033[90mX\033[0m obstáculo | "
            << "\033[90m.\033[0m livre | \033[1;33m*\033[0m projétil\n"
            << BOLD_CYAN << "Defesas:" << RESET_COLOR
            << " \033[1;32mF\033[0m firewall, \033[1;32mA\033[0m antivírus, "
            << "\033[1;32mS\033[0m scanner | "
            << BOLD_CYAN << "Malwares:" << RESET_COLOR
            << " \033[1;31mv\033[0m vírus, \033[1;31mw\033[0m worm, "
            << "\033[1;31mr\033[0m rootkit\n"
            << DIM_COLOR << "============================================================"
            << RESET_COLOR << "\n";
    } else {
        out << "Legenda: E entrada | @ /root | # caminho | X obstáculo | . livre | * projétil\n"
            << "Defesas: F firewall, A antivírus, S scanner | Malwares: v vírus, w worm, r rootkit\n";
    }
}

void TerminalRenderer::drawHeader(const GameManager& game, std::ostream& out) const {
    if (!useColors) {
        out << "CPU: " << game.getGold()
            << " | Integridade: " << game.getRootIntegrity()
            << " | Onda: " << game.getCurrentWave() << "/" << game.getTotalWaves()
            << " | Pontos: " << game.getScore() << "\n";
        return;
    }

    out << BOLD_CYAN << "==================== [ ROOT DEFENSE ] ===================="
        << RESET_COLOR << "\n"
        << BOLD_YELLOW << "CPU: " << game.getGold() << RESET_COLOR
        << " | " << integrityColorFor(game.getRootIntegrity())
        << "Integridade: " << game.getRootIntegrity() << RESET_COLOR
        << " | " << BOLD_CYAN
        << "Onda: " << game.getCurrentWave() << "/" << game.getTotalWaves() << RESET_COLOR
        << " | " << BOLD_MAGENTA
        << "Pontos: " << game.getScore() << RESET_COLOR << "\n"
        << DIM_COLOR << "------------------------------------------------------------"
        << RESET_COLOR << "\n";
}

void TerminalRenderer::drawMap(const GameManager& game, std::ostream& out) const {
    const Map& map = game.getMap();
    std::size_t width = map.getWidth();
    std::size_t height = map.getHeight();

    std::vector<std::string> grid(height, std::string(width, '.'));

    for (std::size_t y = 0; y < height; ++y) {
        for (std::size_t x = 0; x < width; ++x) {
            Cell cell = {static_cast<int>(x), static_cast<int>(y)};
            CellType type = map.getCellType(cell);
            if (type == CellType::Path) {
                grid[y][x] = '#';
            } else if (type == CellType::Blocked) {
                grid[y][x] = 'X';
            }
        }
    }

    const std::vector<Cell>& waypoints = map.getWaypoints();
    if (!waypoints.empty()) {
        const Cell& start = waypoints.front();
        if (map.isInside(start)) {
            grid[start.y][start.x] = 'E';
        }
        const Cell& end = waypoints.back();
        if (map.isInside(end)) {
            grid[end.y][end.x] = '@';
        }
    }

    const std::vector<std::unique_ptr<Tower> >& towers = game.getTowers();
    for (std::size_t i = 0; i < towers.size(); ++i) {
        if (towers[i] && towers[i]->isActive()) {
            Cell cell = towers[i]->getCell();
            if (map.isInside(cell)) {
                grid[cell.y][cell.x] = towers[i]->getSymbol();
            }
        }
    }

    const std::vector<std::unique_ptr<Projectile> >& projectiles = game.getProjectiles();
    for (std::size_t i = 0; i < projectiles.size(); ++i) {
        if (projectiles[i] && projectiles[i]->isActive()) {
            Vector2 pos = projectiles[i]->getPosition();
            Cell cell = {static_cast<int>(std::round(pos.x)),
                         static_cast<int>(std::round(pos.y))};
            if (map.isInside(cell)) {
                grid[cell.y][cell.x] = projectiles[i]->getSymbol();
            }
        }
    }

    const std::vector<std::unique_ptr<Enemy> >& enemies = game.getEnemies();
    for (std::size_t i = 0; i < enemies.size(); ++i) {
        if (enemies[i] && enemies[i]->isActive() && !enemies[i]->isDead()) {
            Vector2 pos = enemies[i]->getPosition();
            Cell cell = {static_cast<int>(std::round(pos.x)),
                         static_cast<int>(std::round(pos.y))};
            if (map.isInside(cell)) {
                grid[cell.y][cell.x] = enemies[i]->getSymbol();
            }
        }
    }

    writeColumnNumbers(width, useColors, out);
    for (std::size_t y = 0; y < height; ++y) {
        writeRowNumber(y, useColors, out);
        if (!useColors) {
            out << grid[y] << "\n";
            continue;
        }
        for (std::size_t x = 0; x < width; ++x) {
            const char* color = colorFor(grid[y][x]);
            if (color[0] == '\0') {
                out << grid[y][x];
            } else {
                out << color << grid[y][x] << RESET_COLOR;
            }
        }
        out << "\n";
    }
}

void TerminalRenderer::drawTowerList(const GameManager& game, std::ostream& out) const {
    const std::vector<std::unique_ptr<Tower> >& towers = game.getTowers();
    if (useColors) {
        out << BOLD_CYAN << "=== DEFESAS INSTALADAS ===" << RESET_COLOR << "\n";
    }
    if (towers.empty()) {
        out << "Nenhuma defesa instalada.\n";
        return;
    }

    for (std::size_t i = 0; i < towers.size(); ++i) {
        const Tower& tower = *towers[i];
        Cell cell = tower.getCell();
        out << "(" << cell.x << ", " << cell.y << ") "
            << tower.getSymbol()
            << " | Nivel: " << tower.getLevel()
            << " | Alcance: " << tower.getRange()
            << " | Dano: " << tower.getDamage()
            << " | Intervalo: " << tower.getFireInterval()
            << " | Alvo: " << priorityText(tower.getTargetPriority())
            << " | Upgrade: ";
        if (tower.canUpgrade()) {
            out << tower.getUpgradeCost() << " CPU";
        } else {
            out << "MAX";
        }
        out << " | Reembolso: " << tower.getSellValue() << " CPU\n";
    }
}

void TerminalRenderer::drawRanking(const ScoreBoard& board, std::ostream& out) const {
    const std::vector<ScoreEntry>& entries = board.getEntries();
    if (useColors) {
        out << BOLD_CYAN << "=== RANKING ===" << RESET_COLOR << "\n";
    } else {
        out << "=== RANKING ===\n";
    }
    if (entries.empty()) {
        out << "Nenhuma pontuacao registrada.\n";
        return;
    }

    for (std::size_t i = 0; i < entries.size(); ++i) {
        out << (i + 1) << ". " << entries[i].name
            << " - " << entries[i].score << " pts"
            << " (onda " << entries[i].wave << ")\n";
    }
}
