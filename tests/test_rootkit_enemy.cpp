#include <doctest/doctest.h>

#include <stdexcept>
#include <vector>

#include "RootkitEnemy.hpp"
#include "Vector2.hpp"

TEST_SUITE("RootkitEnemy") {

TEST_CASE("exposes rootkit symbol and balance attributes") {
    const std::vector<Vector2> path{Vector2{1.0f, 2.0f}, Vector2{5.0f, 2.0f}};
    const RootkitEnemy enemy(path);

    CHECK(enemy.getSymbol() == 'r');
    CHECK(enemy.getHp() == doctest::Approx(120.0f));
    CHECK(enemy.getMaxHp() == doctest::Approx(120.0f));
    CHECK(enemy.getSpeed() == doctest::Approx(0.6f));
    CHECK(enemy.getGoldReward() == 25);
    CHECK(enemy.getDamageToRoot() == 30);
    CHECK(enemy.getPosition().x == doctest::Approx(1.0f));
    CHECK(enemy.getPosition().y == doctest::Approx(2.0f));
}

TEST_CASE("moves along its path using its configured speed") {
    const std::vector<Vector2> path{Vector2{0.0f, 0.0f}, Vector2{5.0f, 0.0f}};
    RootkitEnemy enemy(path);

    enemy.update(1.0f);

    CHECK(enemy.getPosition().x == doctest::Approx(0.6f));
    CHECK(enemy.getPosition().y == doctest::Approx(0.0f));
}

TEST_CASE("armor reduces damage by three with a minimum of one") {
    const std::vector<Vector2> path{Vector2{0.0f, 0.0f}};
    RootkitEnemy enemy(path);

    enemy.takeDamage(10.0f);
    CHECK(enemy.getHp() == doctest::Approx(113.0f));

    enemy.takeDamage(3.0f);
    CHECK(enemy.getHp() == doctest::Approx(112.0f));

    enemy.takeDamage(0.0f);
    enemy.takeDamage(-5.0f);
    CHECK(enemy.getHp() == doctest::Approx(112.0f));
}

TEST_CASE("requires a non-empty path") {
    const std::vector<Vector2> path;

    CHECK_THROWS_AS(RootkitEnemy{path}, std::invalid_argument);
}

} // TEST_SUITE("RootkitEnemy")
