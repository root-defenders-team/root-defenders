#ifndef SCORE_ENTRY_HPP
#define SCORE_ENTRY_HPP

#include <string>

/**
 * @brief Uma linha do ranking: quem jogou, quantos pontos fez e até qual onda chegou.
 *
 * No arquivo do ranking, cada entrada vira uma linha no formato
 * pontos;onda alcançada;nome (por exemplo, 4820;8;Thiago).
 */
struct ScoreEntry {
    std::string name; ///< Nome do jogador, sem ';' e com no máximo 20 caracteres.
    int score;        ///< Pontuação final da partida.
    int wave;         ///< Última onda alcançada.
};

#endif // SCORE_ENTRY_HPP
