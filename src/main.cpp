#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "FileFormatException.hpp"
#include "GameManager.hpp"
#include "Map.hpp"
#include "MapLoader.hpp"
#include "ScoreBoard.hpp"
#include "Shell.hpp"
#include "Wave.hpp"
#include "WaveLoader.hpp"

namespace {

const char* MAP_PATH = "assets/maps/level1.txt"; ///< Mapa padrão da fase.
const char* WAVES_PATH = "assets/waves.txt";     ///< Ondas da fase.
const char* RANKING_PATH = "assets/ranking.txt"; ///< Ranking, criado pelo jogo se não existir.

/// Mostra o menu inicial e devolve o caminho do mapa escolhido pelo jogador.
std::string selectMapFromMenu() {
    std::cout << "\033[1;36m==================== [ ROOT DEFENSE ] ====================\033[0m\n"
              << "Selecione o nível de dificuldade do servidor:\n"
              << "  1. Nível 1 - Muito fácil   (assets/maps/level1.txt)\n"
              << "  2. Nível 2 - Fácil         (assets/maps/level2.txt)\n"
              << "  3. Nível 3 - Médio         (assets/maps/level3.txt)\n"
              << "  4. Nível 4 - Difícil       (assets/maps/level4.txt)\n"
              << "  5. Nível 5 - Muito difícil (assets/maps/level5.txt)\n"
              << "Escolha [1-5] (Enter para Nível 1): ";

    std::string choice;
    if (!std::getline(std::cin, choice)) {
        return MAP_PATH;
    }

    std::size_t first = choice.find_first_not_of(" \t\r");
    if (first == std::string::npos) {
        return MAP_PATH;
    }
    char option = choice[first];
    if (option >= '1' && option <= '5') {
        return std::string("assets/maps/level") + option + ".txt";
    }
    return MAP_PATH;
}

} // namespace

/**
 * @brief Ponto de entrada do ROOT DEFENSE.
 *
 * Carrega o mapa, as ondas e o ranking, monta a partida e entrega o controle ao
 * Shell. Arquivo de dados inválido mostra o arquivo, a linha e o motivo; qualquer
 * outro erro inesperado também é mostrado. Nos dois casos, o programa sai com código 1.
 *
 * @param argc Quantidade de argumentos, incluindo o nome do programa.
 * @param argv Nome do programa e caminho opcional do mapa.
 * @return 0 quando o jogo termina normalmente; 1 em caso de erro.
 */
int main(int argc, char* argv[]) {
    if (argc > 2) {
        std::cerr << "Uso: " << argv[0] << " [arquivo_do_mapa]" << std::endl;
        return 1;
    }

    std::string mapPath = MAP_PATH;
    if (argc == 2) {
        mapPath = argv[1];
    } else {
        mapPath = selectMapFromMenu();
    }

    try {
        Map map = MapLoader::load(mapPath);
        std::vector<Wave> waves = WaveLoader::load(WAVES_PATH);
        GameManager game(map, waves);
        ScoreBoard scoreBoard(RANKING_PATH);
        scoreBoard.load();

        Shell shell(game, scoreBoard, std::cin, std::cout);
        shell.run();
    } catch (const FileFormatException& error) {
        // A mensagem da exceção já traz o arquivo e a linha do erro.
        std::cerr << "Erro no arquivo de dados: " << error.what() << std::endl;
        return 1;
    } catch (const std::exception& error) {
        std::cerr << "Erro inesperado: " << error.what() << std::endl;
        return 1;
    }
    return 0;
}