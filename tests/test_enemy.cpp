#include <doctest/doctest.h>

#include <stdexcept>
#include <vector>

#include "Enemy.hpp"
#include "Vector2.hpp"

namespace {

class DummyEnemyDoc2 : public Enemy {
public:
    DummyEnemyDoc2(const std::vector<Vector2>& path, float maxHp, float speed,
                   int reward, int damageToRoot)
        : Enemy(path, maxHp, speed, reward, damageToRoot) {}
};

} // namespace

TEST_SUITE("Enemy") {

TEST_CASE("inicializacao com path e valores validos (Doc 2)") {
    std::vector<Vector2> path = {
        Vector2(0.0f, 0.0f),
        Vector2(10.0f, 0.0f),
        Vector2(10.0f, 20.0f)
    };

    DummyEnemyDoc2 enemy(path, 100.0f, 10.0f, 25, 15);

    CHECK(enemy.getPosition().x == doctest::Approx(0.0f));
    CHECK(enemy.getPosition().y == doctest::Approx(0.0f));
    CHECK(enemy.getHp() == doctest::Approx(100.0f));
    CHECK(enemy.getMaxHp() == doctest::Approx(100.0f));
    CHECK(enemy.getSpeed() == doctest::Approx(10.0f));
    CHECK(enemy.getReward() == 25);
    CHECK(enemy.getDamageToRoot() == 15);
    CHECK(enemy.getDistanceTravelled() == doctest::Approx(0.0f));
    CHECK(enemy.getWaypointIndex() == 1);
    CHECK_FALSE(enemy.isDead());
    CHECK_FALSE(enemy.hasReachedEnd());
}

TEST_CASE("validacao de excecoes no construtor de Enemy (Doc 2 p. 29)") {
    std::vector<Vector2> validPath = {Vector2(0.0f, 0.0f), Vector2(10.0f, 0.0f)};
    std::vector<Vector2> emptyPath;
    std::vector<Vector2> onePointPath = {Vector2(0.0f, 0.0f)};

    // Caminho com menos de 2 pontos
    CHECK_THROWS_AS(DummyEnemyDoc2(emptyPath, 100.0f, 10.0f, 20, 10), std::invalid_argument);
    CHECK_THROWS_AS(DummyEnemyDoc2(onePointPath, 100.0f, 10.0f, 20, 10), std::invalid_argument);

    // Vida <= 0
    CHECK_THROWS_AS(DummyEnemyDoc2(validPath, 0.0f, 10.0f, 20, 10), std::invalid_argument);
    CHECK_THROWS_AS(DummyEnemyDoc2(validPath, -10.0f, 10.0f, 20, 10), std::invalid_argument);

    // Velocidade <= 0
    CHECK_THROWS_AS(DummyEnemyDoc2(validPath, 100.0f, 0.0f, 20, 10), std::invalid_argument);
    CHECK_THROWS_AS(DummyEnemyDoc2(validPath, 100.0f, -5.0f, 20, 10), std::invalid_argument);

    // Recompensa < 0 (0 e permitido)
    CHECK_NOTHROW(DummyEnemyDoc2(validPath, 100.0f, 10.0f, 0, 10));
    CHECK_THROWS_AS(DummyEnemyDoc2(validPath, 100.0f, 10.0f, -1, 10), std::invalid_argument);

    // Dano ao /root <= 0
    CHECK_THROWS_AS(DummyEnemyDoc2(validPath, 100.0f, 10.0f, 20, 0), std::invalid_argument);
    CHECK_THROWS_AS(DummyEnemyDoc2(validPath, 100.0f, 10.0f, 20, -5), std::invalid_argument);
}

TEST_CASE("takeDamage, dano letal e excecao para dano negativo") {
    std::vector<Vector2> path = {Vector2(0.0f, 0.0f), Vector2(10.0f, 0.0f)};
    DummyEnemyDoc2 enemy(path, 100.0f, 10.0f, 20, 10);

    enemy.takeDamage(30.0f);
    CHECK(enemy.getHp() == doctest::Approx(70.0f));
    CHECK_FALSE(enemy.isDead());

    // Dano negativo lanca invalid_argument
    CHECK_THROWS_AS(enemy.takeDamage(-5.0f), std::invalid_argument);
    CHECK(enemy.getHp() == doctest::Approx(70.0f));

    // Dano letal zera HP e marca como morto
    enemy.takeDamage(80.0f);
    CHECK(enemy.getHp() == doctest::Approx(0.0f));
    CHECK(enemy.isDead());
}

TEST_CASE("movimentacao em Enemy::update e progressao de rota") {
    std::vector<Vector2> path = {
        Vector2(0.0f, 0.0f),
        Vector2(10.0f, 0.0f),
        Vector2(10.0f, 20.0f)
    };

    DummyEnemyDoc2 enemy(path, 100.0f, 10.0f, 20, 10);

    // Move 0.5s a 10 u/s -> anda 5 unidades em direcao a (10, 0)
    enemy.update(0.5f);
    CHECK(enemy.getPosition().x == doctest::Approx(5.0f));
    CHECK(enemy.getPosition().y == doctest::Approx(0.0f));
    CHECK(enemy.getDistanceTravelled() == doctest::Approx(5.0f));
    CHECK_FALSE(enemy.hasReachedEnd());

    // Move mais 0.5s -> atinge (10, 0) e avanca para o proximo waypoint
    enemy.update(0.5f);
    CHECK(enemy.getPosition().x == doctest::Approx(10.0f));
    CHECK(enemy.getPosition().y == doctest::Approx(0.0f));
    CHECK(enemy.getDistanceTravelled() == doctest::Approx(10.0f));
    CHECK(enemy.getWaypointIndex() == 2);
    CHECK_FALSE(enemy.hasReachedEnd());

    // Move 1.0s -> anda 10 unidades em direcao a (10, 20) -> chega em (10, 10)
    enemy.update(1.0f);
    CHECK(enemy.getPosition().x == doctest::Approx(10.0f));
    CHECK(enemy.getPosition().y == doctest::Approx(10.0f));
    CHECK(enemy.getDistanceTravelled() == doctest::Approx(20.0f));

    // Move 1.0s -> atinge o fim do caminho (10, 20)
    enemy.update(1.0f);
    CHECK(enemy.getPosition().x == doctest::Approx(10.0f));
    CHECK(enemy.getPosition().y == doctest::Approx(20.0f));
    CHECK(enemy.getDistanceTravelled() == doctest::Approx(30.0f));
    CHECK(enemy.hasReachedEnd());

    // Updates posteriores apos atingir o final nao alteram posicao
    enemy.update(1.0f);
    CHECK(enemy.getPosition().x == doctest::Approx(10.0f));
    CHECK(enemy.getPosition().y == doctest::Approx(20.0f));
    CHECK(enemy.getDistanceTravelled() == doctest::Approx(30.0f));
}

TEST_CASE("update com deltaTime <= 0 ou inimigo inativo/morto nao move") {
    std::vector<Vector2> path = {Vector2(0.0f, 0.0f), Vector2(10.0f, 0.0f)};
    DummyEnemyDoc2 enemy(path, 100.0f, 10.0f, 20, 10);

    enemy.update(0.0f);
    CHECK(enemy.getPosition().x == doctest::Approx(0.0f));
    CHECK(enemy.getDistanceTravelled() == doctest::Approx(0.0f));

    enemy.update(-1.0f);
    CHECK(enemy.getPosition().x == doctest::Approx(0.0f));

    enemy.setActive(false);
    enemy.update(1.0f);
    CHECK(enemy.getPosition().x == doctest::Approx(0.0f));

    enemy.setActive(true);
    enemy.takeDamage(100.0f);
    enemy.update(1.0f);
    CHECK(enemy.getPosition().x == doctest::Approx(0.0f));
}

} // TEST_SUITE("Enemy")