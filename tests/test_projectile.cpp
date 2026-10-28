#include <doctest/doctest.h>

#include <stdexcept>

#include "Enemy.hpp"
#include "Projectile.hpp"

namespace {

class FakeEnemy : public Enemy {
public:
    FakeEnemy(float x, float y, float hp = 100.0f)
        : Enemy(x, y, 1.0f, 1.0f, hp, 1.0f, 0) {}

    void update(float) override {}
};

class ProjectileTest {
protected:
    FakeEnemy target{10.0f, 0.0f};
};

} // namespace

TEST_SUITE("Projectile") {

TEST_CASE_FIXTURE(ProjectileTest, "rejects non-positive speed or damage") {
    CHECK_THROWS_AS(
        Projectile(Vector2{0.0f, 0.0f}, target, 0.0f, 10.0f),
        std::invalid_argument);
    CHECK_THROWS_AS(
        Projectile(Vector2{0.0f, 0.0f}, target, -1.0f, 10.0f),
        std::invalid_argument);
    CHECK_THROWS_AS(
        Projectile(Vector2{0.0f, 0.0f}, target, 1.0f, 0.0f),
        std::invalid_argument);
    CHECK_THROWS_AS(
        Projectile(Vector2{0.0f, 0.0f}, target, 1.0f, -1.0f),
        std::invalid_argument);

    CHECK_NOTHROW(Projectile(Vector2{0.0f, 0.0f}, target, 1.0f, 10.0f));
}

TEST_CASE_FIXTURE(ProjectileTest, "stores its initial state and non-owning target") {
    Projectile projectile(Vector2{2.0f, 3.0f}, target, 4.0f, 15.5f);

    CHECK(projectile.getPosition().x == doctest::Approx(2.0f));
    CHECK(projectile.getPosition().y == doctest::Approx(3.0f));
    CHECK(projectile.getSpeed() == doctest::Approx(4.0f));
    CHECK(projectile.getDamage() == doctest::Approx(15.5f));
    CHECK_FALSE(projectile.hasImpacted());
    CHECK(projectile.isActive());
    CHECK(projectile.getTarget() == &target);
    CHECK(projectile.getSymbol() == '*');
}

TEST_CASE_FIXTURE(ProjectileTest, "updates speed and damage through setters") {
    Projectile projectile(Vector2{0.0f, 0.0f}, target, 1.0f, 10.0f);

    projectile.setSpeed(3.5f);
    projectile.setDamage(25.5f);

    CHECK(projectile.getSpeed() == doctest::Approx(3.5f));
    CHECK(projectile.getDamage() == doctest::Approx(25.5f));

    CHECK_THROWS_AS(projectile.setSpeed(0.0f), std::invalid_argument);
    CHECK_THROWS_AS(projectile.setSpeed(-1.0f), std::invalid_argument);
    CHECK_THROWS_AS(projectile.setDamage(0.0f), std::invalid_argument);
    CHECK_THROWS_AS(projectile.setDamage(-1.0f), std::invalid_argument);
}

TEST_CASE_FIXTURE(ProjectileTest, "moves toward its target according to speed and elapsed time") {
    Projectile projectile(Vector2{0.0f, 0.0f}, target, 2.0f, 10.0f);

    projectile.update(1.0f);

    CHECK(projectile.getPosition().x == doctest::Approx(2.0f));
    CHECK(projectile.getPosition().y == doctest::Approx(0.0f));
    CHECK_FALSE(projectile.hasImpacted());
    CHECK(projectile.isActive());
}

TEST_CASE_FIXTURE(ProjectileTest, "damages target and deactivates on impact") {
    FakeEnemy fragileTarget(10.0f, 0.0f, 20.0f);
    Projectile projectile(Vector2{0.0f, 0.0f}, fragileTarget, 10.0f, 20.0f);

    projectile.update(1.0f);

    CHECK(fragileTarget.isDead());
    CHECK(projectile.hasImpacted());
    CHECK_FALSE(projectile.isActive());
}

TEST_CASE_FIXTURE(ProjectileTest, "deactivates without moving when target is inactive") {
    Projectile projectile(Vector2{0.0f, 0.0f}, target, 2.0f, 10.0f);
    target.setActive(false);

    projectile.update(1.0f);

    CHECK(projectile.getPosition().x == doctest::Approx(0.0f));
    CHECK(projectile.getPosition().y == doctest::Approx(0.0f));
    CHECK_FALSE(projectile.hasImpacted());
    CHECK_FALSE(projectile.isActive());
}

TEST_CASE_FIXTURE(ProjectileTest, "does not apply damage more than once") {
    FakeEnemy fragileTarget(10.0f, 0.0f, 20.0f);
    Projectile projectile(Vector2{10.0f, 0.0f}, fragileTarget, 1.0f, 10.0f);

    projectile.hit();
    projectile.hit();

    CHECK(fragileTarget.getHp() == doctest::Approx(10.0f));
    CHECK(projectile.hasImpacted());
    CHECK_FALSE(projectile.isActive());
}

} // TEST_SUITE("Projectile")
