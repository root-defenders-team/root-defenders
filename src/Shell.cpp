#include "Shell.hpp"

#include <chrono>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

#include "Cell.hpp"
#include "CommandParser.hpp"
#include "Enemy.hpp"
#include "EnemyFactory.hpp"
#include "EnemyType.hpp"
#include "GameException.hpp"
#include "GameManager.hpp"
#include "Map.hpp"
#include "ScoreBoard.hpp"
#include "ScoreEntry.hpp"
#include "TargetPriority.hpp"
#include "Tower.hpp"
#include "TowerFactory.hpp"
#include "TowerType.hpp"
#include "Vector2.hpp"

namespace {

const char* PROMPT = "root@server:~# ";   ///< Prompt mostrado antes de cada comando.
const float TICK_SECONDS = 0.1f;          ///< Duração de um tick da simulação.
const int DEFAULT_FRAME_DELAY = 100;      ///< Pausa entre quadros no jogo, em milissegundos.
const bool USE_COLORS = true;             ///< O mapa é desenhado com cores ANSI.
const std::size_t MAX_NAME_LENGTH = 20;   ///< Tamanho máximo do nome no ranking.

/// Diz se a linha só tem espaços, TABs ou o '\r' que sobra de arquivos do Windows.
bool isBlank(const std::string& line) {
    return line.find_first_not_of(" \t\r") == std::string::npos;
}

/// Escreve uma célula no formato "(x, y)", usado em todas as mensagens.
std::string cellText(const Cell& cell) {
    std::ostringstream text;
    text << '(' << cell.x << ", " << cell.y << ')';
    return text.str();
}

/// Diz se a partida já terminou, em vitória ou em derrota.
bool isGameOver(const GameManager& game) {
    return game.getState() == GameManager::GameState::Victory ||
           game.getState() == GameManager::GameState::Defeat;
}

/// Nome de cada defesa como aparece nas mensagens.
std::string towerName(TowerType type) {
    switch (type) {
    case TowerType::Firewall:
        return "Firewall";
    case TowerType::Antivirus:
        return "Antivírus";
    case TowerType::Scanner:
        return "Scanner";
    }
    return "Defesa";
}

/// Descrição de cada defesa para o comando man.
std::string towerDescription(TowerType type) {
    switch (type) {
    case TowerType::Firewall:
        return "disparos rápidos e fracos, de alcance curto.";
    case TowerType::Antivirus:
        return "disparos fortes e lentos, de alcance longo.";
    case TowerType::Scanner:
        return "causa dano em todos os malwares ao redor, sem projétil.";
    }
    return "";
}

/// Nome de cada malware como aparece nas mensagens.
std::string enemyName(EnemyType type) {
    switch (type) {
    case EnemyType::Virus:
        return "Vírus";
    case EnemyType::Worm:
        return "Worm";
    case EnemyType::Rootkit:
        return "Rootkit";
    }
    return "Malware";
}

/// Descrição de cada malware para o comando man.
std::string enemyDescription(EnemyType type) {
    switch (type) {
    case EnemyType::Virus:
        return "equilibrado em vida e velocidade.";
    case EnemyType::Worm:
        return "muito rápido e frágil.";
    case EnemyType::Rootkit:
        return "lento e resistente; a blindagem tira 3 de cada golpe (mínimo 1).";
    }
    return "";
}

/// Tenta ler o tipo de uma defesa; false se o nome não for de nenhuma.
bool readTowerType(const std::string& name, TowerType& type) {
    try {
        type = TowerFactory::parseType(name);
        return true;
    } catch (const std::invalid_argument&) {
        return false;
    }
}

/// Tenta ler o tipo de um malware; false se o nome não for de nenhum.
bool readEnemyType(const std::string& name, EnemyType& type) {
    try {
        type = EnemyFactory::parseType(name);
        return true;
    } catch (const std::invalid_argument&) {
        return false;
    }
}

/// Limpa o nome para o ranking: tira ';' e espaços das pontas e corta em 20 caracteres.
std::string cleanName(const std::string& typed) {
    std::string name;
    for (std::size_t i = 0; i < typed.size(); ++i) {
        if (typed[i] != ';' && typed[i] != '\r') {
            name += typed[i];
        }
    }
    std::size_t first = name.find_first_not_of(' ');
    if (first == std::string::npos) {
        return "anonimo";
    }
    std::size_t last = name.find_last_not_of(' ');
    name = name.substr(first, last - first + 1);
    if (name.size() > MAX_NAME_LENGTH) {
        name = name.substr(0, MAX_NAME_LENGTH);
    }
    return name;
}

/// Lista os comandos, com um exemplo de cada.
void printHelp(std::ostream& output) {
    output << "Comandos:\n"
           << "  help                         lista os comandos\n"
           << "  install <tipo> <x> <y>       instala uma defesa (install firewall 5 3)\n"
           << "                               tipos: firewall, antivirus ou scanner\n"
           << "  upgrade <x> <y>              evolui a defesa da célula (upgrade 5 3)\n"
           << "  rm <x> <y>                   remove a defesa e devolve CPU (rm 5 3)\n"
           << "  target <x> <y> <prioridade>  first, strong, weak ou close "
           << "(target 5 3 strong)\n"
           << "  ls                           lista as defesas instaladas\n"
           << "  man <nome>                   manual de uma defesa ou malware (man rootkit)\n"
           << "  run                          inicia a próxima onda\n"
           << "  ranking                      mostra as 5 melhores pontuações\n"
           << "  exit                         sai do jogo\n";
}

} // namespace

Shell::Shell(GameManager& game, ScoreBoard& scoreBoard, std::istream& input,
             std::ostream& output)
    : game(game),
      scoreBoard(scoreBoard),
      input(input),
      output(output),
      renderer(USE_COLORS),
      frameDelay(DEFAULT_FRAME_DELAY) {
}

void Shell::run() {
    int ignoredLines = scoreBoard.getIgnoredLines();
    if (ignoredLines > 0) {
        output << "aviso: " << ignoredLines
               << " linha(s) corrompida(s) do ranking foram ignoradas\n";
    }
    renderer.drawFrame(game, output);
    output << "Como jogar: instale defesas nas células '.' com install <tipo> <x> <y>\n"
           << "(x é a coluna, numerada em cima; y é a linha, numerada à esquerda).\n"
           << "Exemplo: install firewall 4 5. Depois digite run para soltar a onda.\n"
           << "Digite help para ver todos os comandos.\n";

    std::string line;
    while (!isGameOver(game)) {
        output << "\033[1;32m" << PROMPT << "\033[0m";
        // Fim da entrada (Ctrl+D ou fim de um arquivo redirecionado) vale como exit.
        if (!std::getline(input, line)) {
            output << '\n';
            return;
        }
        if (!execute(line)) {
            return;
        }
    }
    finishGame();
}

bool Shell::execute(const std::string& line) {
    if (isBlank(line)) {
        return true;
    }
    try {
        Command command = CommandParser::parse(line);
        if (command.name == "exit") {
            return false;
        } else if (command.name == "help") {
            printHelp(output);
        } else if (command.name == "install") {
            handleInstall(command);
        } else if (command.name == "upgrade") {
            handleUpgrade(command);
        } else if (command.name == "rm") {
            handleRemove(command);
        } else if (command.name == "target") {
            handleTarget(command);
        } else if (command.name == "ls") {
            renderer.drawTowerList(game, output);
        } else if (command.name == "man") {
            handleManual(command);
        } else if (command.name == "run") {
            if (game.startNextWave()) {
                runWave();
            } else {
                output << "erro: não há onda para iniciar agora\n";
            }
        } else if (command.name == "ranking" || command.name == "rank") {
            renderer.drawRanking(scoreBoard, output);
        } else {
            output << "erro: comando desconhecido: " << command.name << '\n';
        }
    } catch (const GameException& error) {
        output << "erro: " << error.what() << '\n';
    }
    return true;
}

void Shell::setFrameDelay(int milliseconds) {
    if (milliseconds < 0) {
        throw std::invalid_argument("A pausa entre quadros não pode ser negativa.");
    }
    frameDelay = milliseconds;
}

void Shell::handleInstall(const Command& command) {
    TowerType type = TowerType::Firewall;
    if (!readTowerType(command.args[0], type)) {
        output << "erro: tipo de defesa desconhecido: " << command.args[0]
               << " (use firewall, antivirus ou scanner)\n";
        return;
    }
    Cell cell = CommandParser::parseCell(command.args[1], command.args[2]);

    const Map& map = game.getMap();
    if (!map.isInside(cell)) {
        output << "erro: a célula " << cellText(cell) << " está fora do mapa\n";
        return;
    }
    if (map.isPath(cell)) {
        output << "erro: a célula " << cellText(cell) << " faz parte do caminho\n";
        return;
    }
    if (map.isBlocked(cell)) {
        output << "erro: a célula " << cellText(cell) << " é um obstáculo\n";
        return;
    }
    if (game.getTowerAt(cell) != nullptr) {
        output << "erro: a célula " << cellText(cell) << " já tem uma defesa\n";
        return;
    }

    std::unique_ptr<Tower> tower = TowerFactory::create(type, cell);
    if (tower->getCost() > game.getGold()) {
        output << "erro: CPU insuficiente: custa " << tower->getCost() << " e você tem "
               << game.getGold() << '\n';
        return;
    }
    if (!game.buildTower(std::move(tower))) {
        output << "erro: não foi possível instalar a defesa em " << cellText(cell) << '\n';
        return;
    }
    output << towerName(type) << " instalado em " << cellText(cell)
           << ". CPU restante: " << game.getGold() << '\n';
}

void Shell::handleUpgrade(const Command& command) {
    Cell cell = CommandParser::parseCell(command.args[0], command.args[1]);
    const Tower* tower = game.getTowerAt(cell);
    if (tower == nullptr) {
        output << "erro: não há defesa em " << cellText(cell) << '\n';
        return;
    }
    if (!tower->canUpgrade()) {
        output << "erro: a defesa em " << cellText(cell) << " já está no nível máximo\n";
        return;
    }
    if (tower->getUpgradeCost() > game.getGold()) {
        output << "erro: CPU insuficiente: a evolução custa " << tower->getUpgradeCost()
               << " e você tem " << game.getGold() << '\n';
        return;
    }
    if (!game.upgradeTower(cell)) {
        output << "erro: não foi possível evoluir a defesa em " << cellText(cell) << '\n';
        return;
    }
    output << "Defesa em " << cellText(cell) << " evoluída para o nível " << tower->getLevel()
           << ". CPU restante: " << game.getGold() << '\n';
}

void Shell::handleRemove(const Command& command) {
    Cell cell = CommandParser::parseCell(command.args[0], command.args[1]);
    const Tower* tower = game.getTowerAt(cell);
    if (tower == nullptr) {
        output << "erro: não há defesa em " << cellText(cell) << '\n';
        return;
    }
    int refund = tower->getSellValue();
    game.sellTower(cell);
    output << "Defesa removida de " << cellText(cell) << ". CPU devolvida: " << refund
           << ". CPU restante: " << game.getGold() << '\n';
}

void Shell::handleTarget(const Command& command) {
    Cell cell = CommandParser::parseCell(command.args[0], command.args[1]);
    TargetPriority priority = CommandParser::parsePriority(command.args[2]);
    if (!game.setTargetPriority(cell, priority)) {
        output << "erro: não há defesa em " << cellText(cell) << '\n';
        return;
    }
    output << "Prioridade de alvo da defesa em " << cellText(cell) << ": " << command.args[2]
           << '\n';
}

void Shell::handleManual(const Command& command) {
    const std::string& name = command.args[0];

    TowerType towerType = TowerType::Firewall;
    if (readTowerType(name, towerType)) {
        Cell origin = {0, 0};
        std::unique_ptr<Tower> tower = TowerFactory::create(towerType, origin);
        output << towerName(towerType) << ": " << towerDescription(towerType) << '\n'
               << "Custo: " << tower->getCost() << " | Alcance: " << tower->getRange()
               << " | Dano: " << tower->getDamage() << " | Intervalo: "
               << tower->getFireInterval() << " s\n";
        return;
    }

    EnemyType enemyType = EnemyType::Virus;
    if (readEnemyType(name, enemyType)) {
        // Caminho de mentira, só para criar o malware e ler os atributos dele.
        std::vector<Vector2> path;
        path.push_back(Vector2(0.0f, 0.0f));
        path.push_back(Vector2(1.0f, 0.0f));
        std::unique_ptr<Enemy> enemy = EnemyFactory::create(enemyType, path);
        output << enemyName(enemyType) << ": " << enemyDescription(enemyType) << '\n'
               << "Vida: " << enemy->getMaxHp() << " | Velocidade: " << enemy->getSpeed()
               << " células/s | Recompensa: " << enemy->getReward()
               << " CPU | Dano ao /root: " << enemy->getDamageToRoot() << '\n';
        return;
    }

    output << "erro: não há manual para " << name
           << " (use o nome de uma defesa ou de um malware)\n";
}

void Shell::runWave() {
    output << "Onda " << game.getCurrentWave() << " de " << game.getTotalWaves()
           << " iniciada.\n";
    while (game.getState() == GameManager::GameState::WaveRunning) {
        game.update(TICK_SECONDS);
        renderer.drawFrame(game, output);
        output << "Onda " << game.getCurrentWave() << " de " << game.getTotalWaves()
               << " em andamento | malwares no mapa: " << game.getEnemies().size() << '\n'
               << "Aguarde o fim da onda: os comandos só valem depois dela.\n";
        if (frameDelay > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(frameDelay));
        }
    }
    if (game.getState() == GameManager::GameState::Preparing) {
        output << "Onda " << game.getCurrentWave() << " concluída. CPU: " << game.getGold()
               << " | Pontos: " << game.getScore() << " | Integridade do /root: "
               << game.getRootIntegrity() << '\n'
               << "Reforce as defesas e digite run para a próxima onda.\n";
    }
}

void Shell::finishGame() {
    if (game.getState() == GameManager::GameState::Victory) {
        output << "Vitória! O /root resistiu a todas as ondas.\n";
    } else {
        output << "Derrota! A integridade do /root chegou a 0.\n";
    }
    output << "Pontuação final: " << game.getScore() << '\n';

    if (scoreBoard.qualifies(game.getScore())) {
        output << "Sua pontuação entrou no ranking! Digite seu nome: ";
        std::string typed;
        std::getline(input, typed);

        ScoreEntry entry;
        entry.name = cleanName(typed);
        entry.score = game.getScore();
        entry.wave = static_cast<int>(game.getCurrentWave());
        scoreBoard.addEntry(entry);
        try {
            scoreBoard.save();
        } catch (const GameException& error) {
            output << "\naviso: o ranking não foi salvo: " << error.what() << '\n';
        }
    }
    output << '\n';
    renderer.drawRanking(scoreBoard, output);
}
