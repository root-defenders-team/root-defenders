#include <doctest/doctest.h>

#include <stdexcept>
#include <vector>

#include "Vector2.hpp"
#include "VirusEnemy.hpp"

TEST_SUITE("VirusEnemy") {

TEST_CASE("exposes virus symbol and balance attributes") {
    const std::vector<Vector2> path{Vector2{1.0f, 2.0f}, Vector2{5.0f, 2.0f}};
    const VirusEnemy enemy(path);

    CHECK(enemy.getSymbol() == 'v');
    CHECK(enemy.getHp() == doctest::Approx(30.0f));
    CHECK(enemy.getMaxHp() == doctest::Approx(30.0f));
    CHECK(enemy.getSpeed() == doctest::Approx(1.0f));
    CHECK(enemy.getReward() == 10);
    CHECK(enemy.getDamageToRoot() == 10);
    CHECK(enemy.getPosition().x == doctest::Approx(1.0f));
    CHECK(enemy.getPosition().y == doctest::Approx(2.0f));
}

TEST_CASE("moves along its path using its configured speed") {
    const std::vector<Vector2> path{Vector2{0.0f, 0.0f}, Vector2{5.0f, 0.0f}};
    VirusEnemy enemy(path);

    enemy.update(2.0f);

    CHECK(enemy.getPosition().x == doctest::Approx(2.0f));
    CHECK(enemy.getPosition().y == doctest::Approx(0.0f));
}

TEST_CASE("requires a non-empty path") {
    const std::vector<Vector2> emptyPath;
    const std::vector<Vector2> shortPath{Vector2{0.0f, 0.0f}};

    CHECK_THROWS_AS(VirusEnemy{emptyPath}, std::invalid_argument);
    CHECK_THROWS_AS(VirusEnemy{shortPath}, std::invalid_argument);
}

} // TEST_SUITE("VirusEnemy")
