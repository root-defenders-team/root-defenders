#ifndef MAKE_UNIQUE_HPP
#define MAKE_UNIQUE_HPP

#include <memory>
#include <utility>

/**
 * @brief Utilidades gerais do projeto.
 */
namespace util {

/**
 * @brief Substituto do std::make_unique, que só existe a partir do C++14.
 * @tparam T Tipo do objeto criado.
 * @tparam Args Tipos dos argumentos do construtor de T.
 * @param args Argumentos repassados ao construtor de T.
 * @return std::unique_ptr dono do novo objeto.
 */
template <typename T, typename... Args>
std::unique_ptr<T> makeUnique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

} // namespace util

#endif // MAKE_UNIQUE_HPP
