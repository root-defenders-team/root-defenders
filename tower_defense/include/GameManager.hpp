#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP

#include <SFML/Graphics.hpp>

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "Map.hpp"

class Tower;
class Enemy;
class Projectile;

/**
 * @brief Coordena o fluxo geral de uma partida de Tower Defense.
 *
 * GameManager é o ponto central do jogo: executa o loop principal, inicia
 * partidas, controla as ondas de inimigos, administra o ouro do jogador,
 * atualiza torres, inimigos e projéteis, verifica as condições de vitória
 * e derrota e persiste a pontuação máxima em arquivo.
 *
 * GameManager não é uma Entity: ele não possui posição nem é desenhado
 * diretamente, apenas gerencia os objetos que são.
 * A implementação dos métodos será definida posteriormente no arquivo .cpp.
 */
class GameManager {
public:
    /**
     * @brief Estados possíveis de uma partida.
     */
    enum class GameState {
        Running, ///< Partida em andamento.
        Victory, ///< Todas as ondas foram concluídas.
        Defeat   ///< O limite de inimigos que atravessaram o caminho foi atingido.
    };

    /**
     * @brief Cria o gerenciador do jogo e carrega a pontuação máxima.
     *
     * @param mapWidth Quantidade de colunas do mapa.
     * @param mapHeight Quantidade de linhas do mapa.
     * @param highScoreFile Caminho do arquivo de texto da pontuação máxima.
     * @throws std::invalid_argument caso mapWidth ou mapHeight seja zero.
     */
    GameManager(std::size_t mapWidth, std::size_t mapHeight,
        const std::string& highScoreFile);

    /**
     * @brief Destrutor que libera as entidades gerenciadas.
     *
     * Definido no .cpp, pois Tower, Enemy e Projectile são tipos incompletos
     * neste header.
     */
    ~GameManager();

    /** @brief Cópia proibida: o GameManager é dono único dos recursos do jogo. */
    GameManager(const GameManager&) = delete;

    /** @brief Atribuição por cópia proibida. */
    GameManager& operator=(const GameManager&) = delete;

    /**
     * @brief Executa o loop principal do jogo.
     *
     * A cada ciclo processa eventos, atualiza o estado do jogo e renderiza
     * a tela, até a janela ser fechada.
     */
    void run();

    /**
     * @brief Inicia uma nova partida.
     *
     * Reinicia ouro, pontuação, ondas e contadores, e remove todas as
     * torres, inimigos e projéteis existentes.
     */
    void startNewGame();

    /**
     * @brief Atualiza torres, inimigos e projéteis.
     * @param deltaTime Tempo decorrido desde a última atualização (em segundos).
     */
    void update(float deltaTime);

    /**
     * @brief Desenha o mapa e todas as entidades ativas.
     * @param window Referência para a janela principal da SFML.
     */
    void render(sf::RenderWindow& window);

    /**
     * @brief Inicia a próxima onda de inimigos.
     *
     * A dificuldade aumenta progressivamente a cada onda.
     *
     * @return false se todas as ondas já tiverem sido iniciadas.
     */
    bool startNextWave();

    /**
     * @brief Verifica se a onda atual foi concluída.
     * @return true quando não há inimigos ativos nem pendentes na onda.
     */
    [[nodiscard]] bool isWaveFinished() const noexcept;

    /** @brief Retorna o número da onda atual (começando em 1). */
    [[nodiscard]] std::size_t currentWave() const noexcept;

    /**
     * @brief Adiciona um inimigo à partida.
     * @param enemy Inimigo criado; o GameManager assume sua posse.
     */
    void spawnEnemy(std::unique_ptr<Enemy> enemy);

    /**
     * @brief Remove inimigos e projéteis que não estão mais ativos.
     */
    void removeInactiveEntities();

    /**
     * @brief Constrói uma torre em uma célula do mapa.
     *
     * @param tower Torre a ser construída; o GameManager assume sua posse.
     * @param x Coordenada horizontal da célula.
     * @param y Coordenada vertical da célula.
     * @return false se a célula for inválida, estiver ocupada ou se o
     *         jogador não tiver ouro suficiente.
     */
    bool buildTower(std::unique_ptr<Tower> tower, int x, int y);

    /** @brief Retorna a quantidade de ouro atual do jogador. */
    [[nodiscard]] int gold() const noexcept;

    /**
     * @brief Adiciona ouro ao jogador (ex.: recompensa por inimigo derrotado).
     * @param amount Quantidade de ouro a adicionar.
     */
    void addGold(int amount) noexcept;

    /**
     * @brief Tenta descontar ouro do jogador.
     * @param amount Quantidade de ouro a gastar.
     * @return true se havia ouro suficiente e o valor foi descontado.
     */
    bool spendGold(int amount) noexcept;

    /**
     * @brief Registra que um inimigo completou o caminho.
     */
    void registerEnemyLeak() noexcept;

    /** @brief Retorna quantos inimigos atravessaram o caminho. */
    [[nodiscard]] int enemiesLeaked() const noexcept;

    /**
     * @brief Verifica as condições de vitória e derrota e atualiza o estado.
     */
    void updateGameState() noexcept;

    /** @brief Retorna o estado atual da partida. */
    [[nodiscard]] GameState state() const noexcept;

    /** @brief Retorna a pontuação da partida atual. */
    [[nodiscard]] int score() const noexcept;

    /** @brief Retorna a pontuação máxima registrada. */
    [[nodiscard]] int highScore() const noexcept;

    /**
     * @brief Lê a pontuação máxima do arquivo de texto.
     *
     * Caso o arquivo não exista, a pontuação máxima é considerada zero.
     */
    void loadHighScore();

    /**
     * @brief Salva a pontuação máxima no arquivo de texto.
     *
     * O recorde só é substituído se a pontuação atual for maior.
     */
    void saveHighScore();

private:
    /** @brief Trata os eventos da janela (fechar, cliques, teclas). */
    void processEvents();

    sf::RenderWindow window_;                          ///< Janela principal do jogo.
    Map map_;                                          ///< Cenário e rota dos inimigos.
    std::vector<std::unique_ptr<Tower>> towers_;       ///< Torres construídas.
    std::vector<std::unique_ptr<Enemy>> enemies_;      ///< Inimigos em jogo.
    std::vector<std::unique_ptr<Projectile>> projectiles_; ///< Projéteis em voo.

    int gold_;                  ///< Ouro disponível para o jogador.
    int enemiesLeaked_;         ///< Inimigos que atravessaram o caminho.
    int maxEnemiesLeaked_;      ///< Limite de inimigos antes da derrota.
    std::size_t currentWave_;   ///< Número da onda atual.
    std::size_t totalWaves_;    ///< Total de ondas da partida.
    int score_;                 ///< Pontuação da partida atual.
    int highScore_;             ///< Maior pontuação já registrada.
    std::string highScoreFile_; ///< Caminho do arquivo da pontuação máxima.
    GameState state_;           ///< Estado atual da partida.
};

#endif // GAMEMANAGER_HPP