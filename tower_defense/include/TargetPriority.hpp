/**
 * @file TargetPriority.hpp
 * @brief Enum com as prioridades de alvo das defesas.
 */

#ifndef TARGET_PRIORITY_HPP
#define TARGET_PRIORITY_HPP

/**
 * @brief Critério que uma defesa usa para escolher o malware que vai atacar.
 */
enum class TargetPriority {
    First,     ///< O mais avançado no caminho (comando: first).
    Strongest, ///< O de mais vida (comando: strong).
    Weakest,   ///< O de menos vida (comando: weak).
    Closest    ///< O mais próximo da defesa (comando: close).
};

#endif // TARGET_PRIORITY_HPP
