#include <doctest/doctest.h>

#include <memory>
#include <stdexcept>
#include <vector>

#include "AntivirusTower.hpp"
#include "Cell.hpp"
#include "Enemy.hpp"
#include "Projectile.hpp"

namespace {

class FakeEnemyPath {
protected:
    std::vector<Vector2> path;

    FakeEnemyPath(float x, float y)
        : path{Vector2(x, y), Vector2(x + 1000.0f, y)} {}
};

class FakeEnemy : private FakeEnemyPath, public Enemy {
public:
    FakeEnemy(float x, float y, float hp = 100.0f)
        : FakeEnemyPath(x, y), Enemy(FakeEnemyPath::path, hp, 1.0f, 10, 10) {}
};

} // namespace

TEST_SUITE("AntivirusTower") {

TEST_CASE("inicializa com atributos e simbolo corretos") {
    Cell cell = {2, 3};
    AntivirusTower tower(cell);

    CHECK(tower.getCell() == cell);
    CHECK(tower.getSymbol() == 'A');
    CHECK(tower.getCost() == 100);
    CHECK(tower.getRange() == doctest::Approx(4.0f));
    CHECK(tower.getDamage() == doctest::Approx(30.0f));
    CHECK(tower.getFireInterval() == doctest::Approx(2.0f));
    CHECK(tower.getLevel() == 1);
}

TEST_CASE("fire cria projetil com velocidade 6 e dano da torre") {
    Cell cell = {0, 0};
    AntivirusTower tower(cell);
    FakeEnemy target(3.0f, 0.0f);

    std::vector<Enemy*> enemies = {&target};
    std::vector<std::unique_ptr<Projectile> > projectiles;

    tower.tryAttack(enemies, projectiles);

    REQUIRE(projectiles.size() == 1);
    CHECK(projectiles[0]->getTarget() == &target);
    CHECK(projectiles[0]->getSpeed() == doctest::Approx(6.0f));
    CHECK(projectiles[0]->getDamage() == doctest::Approx(30.0f));
    CHECK(tower.getCooldown() == doctest::Approx(2.0f));
}

TEST_CASE("applyUpgrade aumenta o dano em 15 por nivel") {
    Cell cell = {0, 0};
    AntivirusTower tower(cell);

    tower.upgrade();
    CHECK(tower.getLevel() == 2);
    CHECK(tower.getDamage() == doctest::Approx(45.0f));
    CHECK(tower.getUpgradeCost() == 150);

    tower.upgrade();
    CHECK(tower.getLevel() == 3);
    CHECK(tower.getDamage() == doctest::Approx(60.0f));
    CHECK_FALSE(tower.canUpgrade());

    CHECK_THROWS_AS(tower.upgrade(), std::logic_error);
}

} // TEST_SUITE("AntivirusTower")
