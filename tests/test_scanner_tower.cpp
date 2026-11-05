#include <doctest/doctest.h>

#include <memory>
#include <stdexcept>
#include <vector>

#include "Cell.hpp"
#include "Enemy.hpp"
#include "Projectile.hpp"
#include "ScannerTower.hpp"

namespace {

class FakeEnemy : public Enemy {
public:
    FakeEnemy(float x, float y, float hp = 100.0f)
        : Enemy(x, y, 0.6f, 0.6f, hp, 1.0f, 10) {}
};

} // namespace

TEST_SUITE("ScannerTower") {

TEST_CASE("inicializa com atributos e simbolo corretos") {
    Cell cell = {4, 5};
    ScannerTower tower(cell);

    CHECK(tower.getCell() == cell);
    CHECK(tower.getSymbol() == 'S');
    CHECK(tower.getCost() == 120);
    CHECK(tower.getRange() == doctest::Approx(2.0f));
    CHECK(tower.getDamage() == doctest::Approx(8.0f));
    CHECK(tower.getFireInterval() == doctest::Approx(1.0f));
    CHECK(tower.getLevel() == 1);
}

TEST_CASE("fire aplica dano em todos os inimigos no alcance sem criar projetil") {
    Cell cell = {0, 0};
    ScannerTower tower(cell);

    FakeEnemy dentro1(1.0f, 0.0f, 50.0f);
    FakeEnemy dentro2(0.0f, 2.0f, 50.0f);
    FakeEnemy fora(3.0f, 0.0f, 50.0f);

    std::vector<Enemy*> enemies = {&dentro1, &dentro2, &fora};
    std::vector<std::unique_ptr<Projectile> > projectiles;

    tower.tryAttack(enemies, projectiles);

    CHECK(projectiles.empty());
    CHECK(dentro1.getHp() == doctest::Approx(42.0f));
    CHECK(dentro2.getHp() == doctest::Approx(42.0f));
    CHECK(fora.getHp() == doctest::Approx(50.0f));
    CHECK(tower.getCooldown() == doctest::Approx(1.0f));
}

TEST_CASE("applyUpgrade aumenta o alcance em 0.5 por nivel") {
    Cell cell = {0, 0};
    ScannerTower tower(cell);

    tower.upgrade();
    CHECK(tower.getLevel() == 2);
    CHECK(tower.getRange() == doctest::Approx(2.5f));
    CHECK(tower.getUpgradeCost() == 180);

    tower.upgrade();
    CHECK(tower.getLevel() == 3);
    CHECK(tower.getRange() == doctest::Approx(3.0f));
    CHECK_FALSE(tower.canUpgrade());

    CHECK_THROWS_AS(tower.upgrade(), std::logic_error);
}

} // TEST_SUITE("ScannerTower")
