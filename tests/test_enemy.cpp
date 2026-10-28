#include <doctest/doctest.h>
#include "Enemy.hpp"
#include "Vector2.hpp"
#include <vector>

class InimigoTeste : public Enemy {
public:
    InimigoTeste(float x, float y, float w, float h, float hp, float speed, int gold)
        : Enemy(x, y, w, h, hp, speed, gold) {}
};

TEST_SUITE("Enemy") {

TEST_CASE("inicializacao e getters basicos") {
    InimigoTeste enemy(0.0f, 0.0f, 32.0f, 32.0f, 100.0f, 10.0f, 50);

    CHECK(enemy.getHp() == 100.0f);
    CHECK(enemy.getMaxHp() == 100.0f);
    CHECK(enemy.getSpeed() == 10.0f);
    CHECK(enemy.getGoldReward() == 50);
    CHECK(enemy.getWaypointIndex() == 0);
    CHECK_FALSE(enemy.isDead());
}

TEST_CASE("mecanica de dano e morte") {
    InimigoTeste enemy(0.0f, 0.0f, 32.0f, 32.0f, 100.0f, 10.0f, 50);

    enemy.takeDamage(30.0f);
    CHECK(enemy.getHp() == 70.0f);
    CHECK_FALSE(enemy.isDead());

    // Dano negativo deve ser ignorado
    enemy.takeDamage(-10.0f);
    CHECK(enemy.getHp() == 70.0f);

    // Dano letal
    enemy.takeDamage(80.0f);
    CHECK(enemy.getHp() == 0.0f);
    CHECK(enemy.isDead());
}

TEST_CASE("movimentacao ao longo dos waypoints") {
    InimigoTeste enemy(0.0f, 0.0f, 32.0f, 32.0f, 100.0f, 10.0f, 50);
    std::vector<Vector2> waypoints = {
        Vector2(10.0f, 0.0f),
        Vector2(10.0f, 20.0f)
    };

    CHECK_FALSE(enemy.hasReachedEnd(waypoints));
    CHECK(enemy.getWaypointIndex() == 0);

    // Move 0.5s com velocidade 10 -> anda 5 unidades em direcao a (10, 0)
    enemy.moveAlongPath(waypoints, 0.5f);
    CHECK(enemy.getPosition().x == doctest::Approx(5.0f));
    CHECK(enemy.getPosition().y == doctest::Approx(0.0f));
    CHECK(enemy.getWaypointIndex() == 0);

    // Move mais 0.5s -> chega exatamente em (10, 0) e avanca para o proximo waypoint
    enemy.moveAlongPath(waypoints, 0.5f);
    CHECK(enemy.getPosition().x == doctest::Approx(10.0f));
    CHECK(enemy.getPosition().y == doctest::Approx(0.0f));
    CHECK(enemy.getWaypointIndex() == 1);
    CHECK_FALSE(enemy.hasReachedEnd(waypoints));

    // Move 1.0s -> anda 10 unidades em direcao a (10, 20) -> chega em (10, 10)
    enemy.moveAlongPath(waypoints, 1.0f);
    CHECK(enemy.getPosition().x == doctest::Approx(10.0f));
    CHECK(enemy.getPosition().y == doctest::Approx(10.0f));
    CHECK(enemy.getWaypointIndex() == 1);

    // Move 1.0s -> chega ao fim da rota (10, 20)
    enemy.moveAlongPath(waypoints, 1.0f);
    CHECK(enemy.getPosition().x == doctest::Approx(10.0f));
    CHECK(enemy.getPosition().y == doctest::Approx(20.0f));
    CHECK(enemy.getWaypointIndex() == 2);
    CHECK(enemy.hasReachedEnd(waypoints));

    // Movimentar apos alcancar o fim nao deve alterar nada
    enemy.moveAlongPath(waypoints, 1.0f);
    CHECK(enemy.getPosition().x == doctest::Approx(10.0f));
    CHECK(enemy.getPosition().y == doctest::Approx(20.0f));
}

TEST_CASE("movimentacao com tempo zero ou negativo nao altera posicao") {
    InimigoTeste enemy(5.0f, 5.0f, 32.0f, 32.0f, 100.0f, 10.0f, 50);
    std::vector<Vector2> waypoints = { Vector2(20.0f, 5.0f) };

    enemy.moveAlongPath(waypoints, 0.0f);
    CHECK(enemy.getPosition().x == doctest::Approx(5.0f));

    enemy.moveAlongPath(waypoints, -1.0f);
    CHECK(enemy.getPosition().x == doctest::Approx(5.0f));
}

} // TEST_SUITE("Enemy")