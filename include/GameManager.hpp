#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP

#include <cstddef>
#include <memory>
#include <vector>

#include "Cell.hpp"
#include "Map.hpp"
#include "TargetPriority.hpp"
#include "Wave.hpp"

class Enemy;
class Projectile;
class Tower;

/**
 * @brief Aplica as regras de uma partida do ROOT DEFENSE.
 *
 * O GameManager é dono das defesas, dos malwares e dos projéteis, controla a
 * CPU, a pontuação, a integridade do /root e o andamento das ondas, e decide
 * a vitória e a derrota.
 *
 * Ele não lê teclado, não desenha e não abre arquivo: o Shell conversa com o
 * jogador, o TerminalRenderer desenha e o ScoreBoard guarda o ranking. Por
 * isso pode ser testado inteiro com um mapa pequeno montado no próprio teste.
 */
class GameManager {
public:
    /**
     * @brief Estados possíveis de uma partida.
     */
    enum class GameState {
        Preparing,   ///< Início da partida ou fim de uma onda; o jogador prepara as defesas.
        WaveRunning, ///< Uma onda está em andamento; o jogador só assiste.
        Victory,     ///< A última onda terminou com integridade acima de 0.
        Defeat       ///< A integridade do /root chegou a 0.
    };

    static const int INITIAL_GOLD = 200;      ///< CPU no início da partida.
    static const int INITIAL_INTEGRITY = 100; ///< Integridade do /root no início da partida.

    /**
     * @brief Cria uma partida com o mapa e as ondas já carregados.
     *
     * @param map Mapa da fase; o GameManager guarda uma cópia.
     * @param waves Ondas da partida, na ordem em que acontecem.
     * @throws std::invalid_argument se o mapa não tiver caminho ou se não houver ondas.
     */
    GameManager(const Map& map, const std::vector<Wave>& waves);

    /**
     * @brief Libera as defesas, os malwares e os projéteis.
     *
     * Definido no .cpp, pois Tower, Enemy e Projectile são tipos incompletos
     * neste header.
     */
    ~GameManager();

    /** @brief Cópia proibida: o GameManager é dono único das entidades. */
    GameManager(const GameManager&) = delete;

    /** @brief Atribuição por cópia proibida. */
    GameManager& operator=(const GameManager&) = delete;

    /**
     * @brief Recomeça a partida do zero.
     *
     * Remove os malwares primeiro (eles apontam para o caminho do mapa), depois
     * os projéteis e as defesas, libera as células e volta CPU, integridade,
     * pontos e ondas aos valores iniciais.
     */
    void startNewGame();

    /**
     * @brief Inicia a próxima onda.
     * @return false se já há uma onda rodando ou se as ondas acabaram.
     */
    bool startNextWave();

    /**
     * @brief Avança a simulação em um tick.
     *
     * Segue a ordem fixa das regras do jogo: nascimentos, movimento dos
     * malwares, dano ao /root, ataque das defesas, projéteis, recompensas,
     * remoção e atualização do estado.
     *
     * @param deltaTime Tempo do tick, em segundos (0,1 no jogo).
     */
    void update(float deltaTime);

    /**
     * @brief Coloca um malware na partida.
     * @param enemy Malware criado; o GameManager assume a posse.
     */
    void spawnEnemy(std::unique_ptr<Enemy> enemy);

    /**
     * @brief Apaga os projéteis e os malwares que ficaram inativos.
     *
     * Antes de apagar, desativa todo projétil cujo alvo esteja inativo, para
     * que nenhum projétil acesse um malware já apagado.
     */
    void removeInactiveEntities();

    /**
     * @brief Instala uma defesa na célula guardada nela.
     *
     * @param tower Defesa a instalar; o GameManager assume a posse.
     * @return false se a defesa for nula, a célula não aceitar construção,
     *         estiver ocupada ou não houver CPU suficiente.
     */
    bool buildTower(std::unique_ptr<Tower> tower);

    /**
     * @brief Evolui a defesa de uma célula em um nível.
     * @param cell Célula da defesa.
     * @return false se não houver defesa, ela já estiver no nível máximo ou faltar CPU.
     */
    bool upgradeTower(const Cell& cell);

    /**
     * @brief Remove a defesa de uma célula e devolve 50% do que foi gasto nela.
     * @param cell Célula da defesa.
     * @return false se não houver defesa na célula.
     */
    bool sellTower(const Cell& cell);

    /**
     * @brief Muda a prioridade de alvo da defesa de uma célula.
     * @param cell Célula da defesa.
     * @param priority Nova prioridade de alvo.
     * @return false se não houver defesa na célula.
     */
    bool setTargetPriority(const Cell& cell, TargetPriority priority);

    /**
     * @brief Procura a defesa instalada em uma célula.
     * @param cell Célula procurada.
     * @return A defesa, ou nullptr se não houver nenhuma.
     */
    const Tower* getTowerAt(const Cell& cell) const;

    /** @brief Retorna a CPU disponível. */
    int getGold() const;

    /**
     * @brief Soma CPU ao jogador.
     * @param amount Quantidade a somar.
     * @throws std::invalid_argument se amount for negativo.
     */
    void addGold(int amount);

    /**
     * @brief Tenta gastar CPU.
     * @param amount Quantidade a gastar.
     * @return false se amount for negativo ou maior que a CPU disponível.
     */
    bool spendGold(int amount);

    /**
     * @brief Tira integridade do /root; ela nunca fica abaixo de 0.
     * @param amount Dano causado ao /root; valores menores ou iguais a 0 são ignorados.
     */
    void damageRoot(int amount);

    /** @brief Retorna a integridade atual do /root. */
    int getRootIntegrity() const;

    /** @brief Retorna a pontuação da partida. */
    int getScore() const;

    /** @brief Retorna o número da onda atual (0 antes da primeira). */
    std::size_t getCurrentWave() const;

    /** @brief Retorna quantas ondas a partida tem. */
    std::size_t getTotalWaves() const;

    /**
     * @brief Diz se a onda atual terminou.
     * @return true quando a onda já soltou todos os malwares e não há nenhum ativo,
     *         ou quando nenhuma onda começou ainda.
     */
    bool isWaveFinished() const;

    /** @brief Retorna o estado atual da partida. */
    GameState getState() const;

    /** @brief Retorna o mapa da partida. */
    const Map& getMap() const;

    /** @brief Retorna as defesas instaladas. */
    const std::vector<std::unique_ptr<Tower> >& getTowers() const;

    /** @brief Retorna os malwares em jogo. */
    const std::vector<std::unique_ptr<Enemy> >& getEnemies() const;

    /** @brief Retorna os projéteis em voo. */
    const std::vector<std::unique_ptr<Projectile> >& getProjectiles() const;

private:
    /**
     * @brief Procura a defesa de uma célula, permitindo alterá-la.
     * @param cell Célula procurada.
     * @return A defesa, ou nullptr se não houver nenhuma.
     */
    Tower* findTower(const Cell& cell) const;

    /**
     * @brief Confere fim de onda, vitória e derrota e muda o estado.
     */
    void updateGameState();

    Map map;                                               ///< Mapa da fase.
    std::vector<Wave> waves;                               ///< Ondas da partida.
    std::vector<std::unique_ptr<Tower> > towers;           ///< Defesas instaladas.
    std::vector<std::unique_ptr<Enemy> > enemies;          ///< Malwares em jogo.
    std::vector<std::unique_ptr<Projectile> > projectiles; ///< Projéteis em voo.
    int gold;                                              ///< CPU disponível.
    int rootIntegrity;                                     ///< Integridade do /root.
    int score;                                             ///< Pontuação da partida.
    std::size_t currentWave;                               ///< Onda atual (0 antes da primeira).
    GameState state;                                       ///< Estado da partida.
};

#endif // GAME_MANAGER_HPP
