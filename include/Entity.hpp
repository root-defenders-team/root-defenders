#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "Vector2.hpp"
#include "Rect.hpp"

/**
 * @brief Classe abstrata base para todas as entidades do jogo.
 * 
 * A classe Entity centraliza as propriedades físicas e lógicas comuns a todos os 
 * objetos em jogo (Torres, Inimigos e Projéteis). 
 */
class Entity {
protected:
    Vector2 position; ///< Armazena a posição horizontal e vertical da entidade.
    Vector2 size;     ///< Armazena a largura e a altura da entidade.
    bool active;      ///< Controla se a entidade está ativa no jogo.

public:
    /**
     * @brief Construtor padrão da Entidade.
     * @param startX Posição horizontal inicial no mapa.
     * @param startY Posição vertical inicial no mapa.
     * @param width Largura da entidade.
     * @param height Altura da entidade.
     * @throws std::invalid_argument se width ou height forem menores ou iguais a zero.
     */
    Entity(float startX, float startY, float width, float height);

    /**
     * @brief Destrutor virtual para garantir a correta destruição das subclasses.
     */
    virtual ~Entity() = default;

    /**
     * @brief Define a interface de atualização de estado.
     * 
     * Método virtual puro que obriga as subclasses a implementarem suas lógicas 
     * a cada ciclo do loop principal.
     * 
     * @param deltaTime Tempo decorrido desde a última atualização (em segundos).
     */
    virtual void update(float deltaTime) = 0;

    /**
     * @brief Retorna o caractere que representa a entidade no mapa.
     * @return Caractere desenhado no mapa.
     */
    virtual char getSymbol() const;

    /**
     * @brief Permite verificações básicas de colisão entre entidades.
     * 
     * @param other Referência constante para a entidade alvo da verificação.
     * @return true se as bounding boxes das entidades estiverem sobrepostas, false caso contrário.
     */
    bool checkCollision(const Entity& other) const;

    /**
     * @brief Representa a área ocupada pela entidade.
     * 
     * Essencial para a lógica de colisão e para os limites físicos no mapa.
     * 
     * @return Rect contendo o retângulo delimitador (Bounding Box) da entidade.
     */
    Rect getBounds() const;

    /**
     * @brief Consulta o status de atividade da entidade.
     * @return true se a entidade estiver ativa, false se puder ser removida ou ignorada.
     */
    bool isActive() const;

    /**
     * @brief Altera o estado de atividade da entidade.
     * @param state Novo estado (true para ativa, false para inativa).
     */
    void setActive(bool state);

    /**
     * @brief Obtém a posição atual da entidade.
     * @return Vector2 contendo as coordenadas X e Y.
     */
    Vector2 getPosition() const;

    /**
     * @brief Define uma nova posição no mapa para a entidade.
     * @param newPosition Vetor 2D contendo as novas coordenadas.
     */
    void setPosition(const Vector2& newPosition);
};

#endif // ENTITY_HPP