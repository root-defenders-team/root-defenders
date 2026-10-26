#ifndef VECTOR2_HPP
#define VECTOR2_HPP

/**
 * @brief Vetor 2D de valor, usado para posições, tamanhos e direções no mapa.
 *
 * As unidades são células do mapa. Os campos são públicos, no mesmo espírito
 * do sf::Vector2f da SFML.
 */
struct Vector2 {
    float x; ///< Componente horizontal (coluna).
    float y; ///< Componente vertical (linha).

    /**
     * @brief Cria o vetor nulo (0, 0).
     */
    Vector2();

    /**
     * @brief Cria um vetor com as componentes informadas.
     * @param x Componente horizontal.
     * @param y Componente vertical.
     */
    Vector2(float x, float y);

    /**
     * @brief Calcula o comprimento (norma euclidiana) do vetor.
     * @return Comprimento do vetor.
     */
    float length() const;

    /**
     * @brief Calcula a distância euclidiana até outro ponto.
     * @param other Ponto de destino.
     * @return Distância entre os dois pontos.
     */
    float distanceTo(const Vector2& other) const;

    /**
     * @brief Soma componente a componente.
     * @param other Vetor somado.
     * @return Novo vetor com a soma.
     */
    Vector2 operator+(const Vector2& other) const;

    /**
     * @brief Subtração componente a componente.
     * @param other Vetor subtraído.
     * @return Novo vetor com a diferença.
     */
    Vector2 operator-(const Vector2& other) const;

    /**
     * @brief Multiplica as duas componentes por um escalar.
     * @param scalar Fator de multiplicação.
     * @return Novo vetor escalado.
     */
    Vector2 operator*(float scalar) const;
};

#endif // VECTOR2_HPP
