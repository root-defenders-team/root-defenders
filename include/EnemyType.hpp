/**
 * @file EnemyType.hpp
 * @brief Enum com os tipos de malware.
 */

#ifndef ENEMY_TYPE_HPP
#define ENEMY_TYPE_HPP

/**
 * @brief Tipos de malware existentes no jogo.
 */
enum class EnemyType {
    Virus,  ///< Equilibrado em vida e velocidade.
    Worm,   ///< Muito rápido e frágil.
    Rootkit ///< Lento, com muita vida e blindagem.
};

#endif // ENEMY_TYPE_HPP
