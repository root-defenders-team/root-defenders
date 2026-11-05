#include <doctest/doctest.h>

#include <stdexcept>
#include <vector>

#include "Enemy.hpp"
#include "EnemyFactory.hpp"

TEST_SUITE("EnemyFactory") {

TEST_CASE("parses supported enemy names") {
    CHECK(EnemyFactory::parseType("virus") == EnemyType::Virus);
    CHECK(EnemyFactory::parseType("worm") == EnemyType::Worm);
    CHECK(EnemyFactory::parseType("rootkit") == EnemyType::Rootkit);
}

TEST_CASE("rejects unknown enemy names") {
    CHECK_THROWS_AS(EnemyFactory::parseType("unknown"), std::invalid_argument);
    CHECK_THROWS_AS(EnemyFactory::parseType("Virus"), std::invalid_argument);
    CHECK_THROWS_AS(EnemyFactory::parseType(""), std::invalid_argument);
}

TEST_CASE("creates each supported enemy type") {
    const std::vector<Vector2> path{
        Vector2{1.0f, 2.0f}, Vector2{5.0f, 2.0f}};

    std::unique_ptr<Enemy> virus = EnemyFactory::create(EnemyType::Virus, path);
    std::unique_ptr<Enemy> worm = EnemyFactory::create(EnemyType::Worm, path);
    std::unique_ptr<Enemy> rootkit =
        EnemyFactory::create(EnemyType::Rootkit, path);

    REQUIRE(virus);
    REQUIRE(worm);
    REQUIRE(rootkit);
    CHECK(virus->getSymbol() == 'v');
    CHECK(worm->getSymbol() == 'w');
    CHECK(rootkit->getSymbol() == 'r');
}

TEST_CASE("rejects unknown enum values and empty paths") {
    const std::vector<Vector2> path{
        Vector2{0.0f, 0.0f}, Vector2{1.0f, 0.0f}};
    const std::vector<Vector2> emptyPath;

    CHECK_THROWS_AS(
        EnemyFactory::create(static_cast<EnemyType>(-1), path),
        std::invalid_argument);
    CHECK_THROWS_AS(
        EnemyFactory::create(EnemyType::Virus, emptyPath),
        std::invalid_argument);
    CHECK_THROWS_AS(
        EnemyFactory::create(EnemyType::Worm, emptyPath),
        std::invalid_argument);
    CHECK_THROWS_AS(
        EnemyFactory::create(EnemyType::Rootkit, emptyPath),
        std::invalid_argument);
}

} // TEST_SUITE("EnemyFactory")
