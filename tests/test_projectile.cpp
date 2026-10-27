#include <doctest/doctest.h>

#include <stdexcept>
#include <vector>

#include "Enemy.hpp"
#include "Projectile.hpp"

namespace {

class FakeEnemy : public Enemy {
public:
    FakeEnemy(float x, float y, float hp = 100.0f)
        : Enemy(x, y, 1.0f, 1.0f, hp, 1.0f, 0) {}

    void update(float) override {}

    void render(sf::RenderWindow&) override {}
};

class ProjectileTest {
protected:
    FakeEnemy target{10.0f, 0.0f};
};

} // namespace

TEST_SUITE("Projectile") {

TEST_CASE_FIXTURE(ProjectileTest, "rejects non-positive speed or damage") {
    CHECK_THROWS_AS(
        Projectile(sf::Vector2f{0.0f, 0.0f}, target, 0.0f, 10),
        std::invalid_argument);
    CHECK_THROWS_AS(
        Projectile(sf::Vector2f{0.0f, 0.0f}, target, -1.0f, 10),
        std::invalid_argument);
    CHECK_THROWS_AS(
        Projectile(sf::Vector2f{0.0f, 0.0f}, target, 1.0f, 0),
        std::invalid_argument);
    CHECK_THROWS_AS(
        Projectile(sf::Vector2f{0.0f, 0.0f}, target, 1.0f, -1),
        std::invalid_argument);

    CHECK_NOTHROW(Projectile(sf::Vector2f{0.0f, 0.0f}, target, 1.0f, 10));
}

TEST_CASE_FIXTURE(ProjectileTest, "stores its initial state and target") {
    const sf::Vector2f startPosition{2.0f, 3.0f};
    Projectile projectile(startPosition, target, 4.0f, 15);

    CHECK(projectile.getPosition().x == doctest::Approx(2.0f));
    CHECK(projectile.getPosition().y == doctest::Approx(3.0f));
    CHECK(projectile.getSpeed() == doctest::Approx(4.0f));
    CHECK(projectile.getDamage() == 15);
    CHECK_FALSE(projectile.hasImpacted());
    CHECK(&projectile.getEnemy() == &target);
}

TEST_CASE_FIXTURE(ProjectileTest, "updates projectile properties through setters") {
    Projectile projectile(sf::Vector2f{0.0f, 0.0f}, target, 1.0f, 10);
    const sf::Vector2f newPosition{5.0f, 6.0f};

    projectile.setPosition(newPosition);
    projectile.setSpeed(3.5f);
    projectile.setDamage(25);
    projectile.setImpacted(true);

    CHECK(projectile.getPosition().x == doctest::Approx(5.0f));
    CHECK(projectile.getPosition().y == doctest::Approx(6.0f));
    CHECK(projectile.getSpeed() == doctest::Approx(3.5f));
    CHECK(projectile.getDamage() == 25);
    CHECK(projectile.hasImpacted());
}

TEST_CASE_FIXTURE(ProjectileTest, "moves toward its target according to speed and elapsed time") {
    Projectile projectile(sf::Vector2f{0.0f, 0.0f}, target, 2.0f, 10);

    projectile.update(1.0f);

    CHECK(projectile.getPosition().x == doctest::Approx(2.0f));
    CHECK(projectile.getPosition().y == doctest::Approx(0.0f));
    CHECK_FALSE(projectile.hasImpacted());
}

TEST_CASE_FIXTURE(ProjectileTest, "hit damages the target and marks the projectile impacted") {
    FakeEnemy fragileTarget(10.0f, 0.0f, 20.0f);
    Projectile projectile(sf::Vector2f{10.0f, 0.0f}, fragileTarget, 1.0f, 20);

    projectile.hit();

    CHECK(fragileTarget.isDead());
    CHECK(projectile.hasImpacted());
}

} // TEST_SUITE("Projectile")
