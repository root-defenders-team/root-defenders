#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include <SFML/Graphics.hpp>

class Enemy;

/**
 * @brief Projétil disparado por uma torre contra um inimigo.
 *
 * O projétil acompanha o alvo durante o voo e informa quando ocorre o
 * impacto. Ele não é dono do Enemy: o GameManager deve garantir que o alvo
 * continue existindo enquanto o projétil estiver ativo. O método hit() aplica
 * o dano e marca o projétil como impactado.
 */
class Projectile {
public:
	/**
	 * @brief Cria um projétil direcionado a um inimigo.
	 *
	 * @param startPosition Posição inicial do projétil no mapa.
	 * @param target Instância do inimigo perseguido; não é transferida a posse.
	 * @param speed Velocidade de movimento em unidades por segundo.
	 * @param damage Dano causado ao alvo no impacto.
	 * @throws std::invalid_argument se speed ou damage não forem positivos.
	 */
	Projectile(const sf::Vector2f& startPosition, Enemy& target,
		float speed, int damage);

	/**
	 * @brief Move o projétil em direção ao alvo usando a velocidade configurada
	 *        e detecta o impacto.
	 * @param deltaTime Tempo decorrido desde a última atualização, em segundos.
	 */
	void update(float deltaTime);

	/** @brief Desenha o projétil na janela principal da SFML. */
	void render(sf::RenderWindow& window);

	/** @brief Aplica o dano ao inimigo e marca o projétil como impactado. */
	void hit();

	/** @brief Retorna o inimigo alvo, sem transferir sua posse. */
	[[nodiscard]] Enemy& getEnemy() const noexcept;

	/** @brief Retorna a posição atual do projétil. */
	[[nodiscard]] const sf::Vector2f& getPosition() const noexcept;

	/** @brief Define a posição atual do projétil. */
	void setPosition(const sf::Vector2f& newPosition) noexcept;

	/** @brief Retorna a velocidade do projétil em unidades por segundo. */
	[[nodiscard]] float getSpeed() const noexcept;

	/** @brief Define a velocidade do projétil em unidades por segundo. */
	void setSpeed(float newSpeed);

	/** @brief Retorna o dano causado quando o projétil atinge o alvo. */
	[[nodiscard]] int getDamage() const noexcept;

	/** @brief Define o dano causado quando o projétil atinge o alvo. */
	void setDamage(int newDamage);

	/**
	 * @brief Informa se o projétil atingiu o alvo.
	 * @return true após a colisão; false enquanto estiver em voo.
	 */
	[[nodiscard]] bool hasImpacted() const noexcept;

	/** @brief Define se o projétil já atingiu o alvo. */
	void setImpacted(bool impacted) noexcept;

private:
	Enemy& enemy_; ///< Instância alvo, mantida pelo GameManager.
	sf::Vector2f position_; ///< Posição atual do projétil no mapa.
	float speed_; ///< Velocidade do projétil em unidades por segundo.
	int damage_; ///< Dano aplicado ao inimigo quando ocorre o impacto.
	bool impacted_; ///< Indica se o projétil já atingiu o alvo.
};

#endif // PROJECTILE_HPP
