#include <doctest/doctest.h>

#include <stdexcept>
#include <vector>

#include "Vector2.hpp"
#include "WormEnemy.hpp"

TEST_SUITE("WormEnemy") {

TEST_CASE("exposes worm symbol and balance attributes") {
    const std::vector<Vector2> path{Vector2{1.0f, 2.0f}, Vector2{5.0f, 2.0f}};
    const WormEnemy enemy(path);

    CHECK(enemy.getSymbol() == 'w');
    CHECK(enemy.getHp() == doctest::Approx(15.0f));
    CHECK(enemy.getMaxHp() == doctest::Approx(15.0f));
    CHECK(enemy.getSpeed() == doctest::Approx(2.0f));
    CHECK(enemy.getGoldReward() == 5);
    CHECK(enemy.getDamageToRoot() == 8);
    CHECK(enemy.getPosition().x == doctest::Approx(1.0f));
    CHECK(enemy.getPosition().y == doctest::Approx(2.0f));
}

TEST_CASE("moves along its path using its configured speed") {
    const std::vector<Vector2> path{Vector2{0.0f, 0.0f}, Vector2{5.0f, 0.0f}};
    WormEnemy enemy(path);

    enemy.update(1.0f);

    CHECK(enemy.getPosition().x == doctest::Approx(2.0f));
    CHECK(enemy.getPosition().y == doctest::Approx(0.0f));
}

TEST_CASE("requires a non-empty path") {
    const std::vector<Vector2> path;

    CHECK_THROWS_AS(WormEnemy{path}, std::invalid_argument);
}

} // TEST_SUITE("WormEnemy")
