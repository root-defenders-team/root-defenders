#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SFML/Graphics.hpp>

/**
 * @brief Classe abstrata base para todas as entidades do jogo.
 * 
 * A classe Entity centraliza as propriedades físicas e lógicas comuns a todos os 
 * objetos em jogo (Torres, Inimigos e Projéteis). 
 */
class Entity {
protected:
    sf::Vector2f position; ///< Armazena a posição horizontal e vertical da entidade.
    sf::Vector2f size;     ///< Armazena a largura e a altura da entidade.
    bool active;           ///< Controla se a entidade está ativa no jogo.

public:
    /**
     * @brief Construtor padrão da Entidade.
     * @param startX Posição horizontal inicial no mapa.
     * @param startY Posição vertical inicial no mapa.
     * @param width Largura da entidade.
     * @param height Altura da entidade.
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
     * @brief Define a interface de renderização.
     * 
     * Método virtual puro que obriga as subclasses a saberem como se desenhar 
     * na tela.
     * 
     * @param window Referência para a janela principal (sf::RenderWindow) da SFML.
     */
    virtual void render(sf::RenderWindow& window) = 0;

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
     * Essencial para a lógica de colisão e para os limites visuais na SFML.
     * 
     * @return sf::FloatRect contendo o retângulo delimitador (Bounding Box) da entidade.
     */
    sf::FloatRect getBounds() const;

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
     * @return sf::Vector2f contendo as coordenadas X e Y.
     */
    sf::Vector2f getPosition() const;

    /**
     * @brief Define uma nova posição no mapa para a entidade.
     * @param newPosition Vetor 2D contendo as novas coordenadas.
     */
    void setPosition(const sf::Vector2f& newPosition);
};

#endif // ENTITY_HPP