#include <doctest/doctest.h>

#include <memory>
#include <stdexcept>
#include <vector>

#include "Cell.hpp"
#include "Enemy.hpp"
#include "FirewallTower.hpp"
#include "Projectile.hpp"

namespace {

class FakeEnemy : public Enemy {
public:
    FakeEnemy(float x, float y, float hp = 100.0f)
        : Enemy(x, y, 0.6f, 0.6f, hp, 1.0f, 10) {}
};

} // namespace

TEST_SUITE("FirewallTower") {

TEST_CASE("inicializa com atributos e simbolo corretos") {
    Cell cell = {1, 2};
    FirewallTower tower(cell);

    CHECK(tower.getCell() == cell);
    CHECK(tower.getSymbol() == 'F');
    CHECK(tower.getCost() == 50);
    CHECK(tower.getRange() == doctest::Approx(2.5f));
    CHECK(tower.getDamage() == doctest::Approx(6.0f));
    CHECK(tower.getFireInterval() == doctest::Approx(0.5f));
    CHECK(tower.getLevel() == 1);
}

TEST_CASE("fire cria projetil com velocidade 8 e dano da torre") {
    Cell cell = {0, 0};
    FirewallTower tower(cell);
    FakeEnemy target(2.0f, 0.0f);

    std::vector<Enemy*> enemies = {&target};
    std::vector<std::unique_ptr<Projectile> > projectiles;

    tower.tryAttack(enemies, projectiles);

    REQUIRE(projectiles.size() == 1);
    CHECK(projectiles[0]->getTarget() == &target);
    CHECK(projectiles[0]->getSpeed() == doctest::Approx(8.0f));
    CHECK(projectiles[0]->getDamage() == doctest::Approx(6.0f));
    CHECK(tower.getCooldown() == doctest::Approx(0.5f));
}

TEST_CASE("applyUpgrade reduz o intervalo de disparo em 0.1 por nivel") {
    Cell cell = {0, 0};
    FirewallTower tower(cell);

    tower.upgrade();
    CHECK(tower.getLevel() == 2);
    CHECK(tower.getFireInterval() == doctest::Approx(0.4f));
    CHECK(tower.getUpgradeCost() == 75);

    tower.upgrade();
    CHECK(tower.getLevel() == 3);
    CHECK(tower.getFireInterval() == doctest::Approx(0.3f));
    CHECK_FALSE(tower.canUpgrade());

    CHECK_THROWS_AS(tower.upgrade(), std::logic_error);
}

} // TEST_SUITE("FirewallTower")
