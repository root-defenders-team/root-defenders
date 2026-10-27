#ifndef RECT_HPP
#define RECT_HPP

/**
 * @brief Retângulo alinhado aos eixos, usado como caixa de colisão das entidades.
 *
 * É um agregado: aceita inicialização como Rect r = {left, top, width, height};.
 */
struct Rect {
    float left;   ///< Coordenada X da borda esquerda.
    float top;    ///< Coordenada Y da borda superior.
    float width;  ///< Largura do retângulo.
    float height; ///< Altura do retângulo.

    /**
     * @brief Verifica se este retângulo se sobrepõe a outro.
     *
     * Retângulos que apenas encostam pelas bordas não contam como sobreposição.
     *
     * @param other Retângulo comparado.
     * @return true se houver área em comum entre os dois.
     */
    bool intersects(const Rect& other) const;
};

#endif // RECT_HPP
