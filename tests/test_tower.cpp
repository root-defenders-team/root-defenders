#include <doctest/doctest.h>

#include <memory>
#include <stdexcept>
#include <vector>

#include "Cell.hpp"
#include "Enemy.hpp"
#include "Projectile.hpp"
#include "TargetPriority.hpp"
#include "Tower.hpp"

namespace {

class DummyTower : public Tower {
public:
    int fireCount;
    Enemy* lastTarget;

    DummyTower(const Cell& cell, float range, float damage, float fireInterval, int cost)
        : Tower(cell, range, damage, fireInterval, cost),
          fireCount(0),
          lastTarget(nullptr) {}

    void testSetRange(float r) { setRange(r); }
    void testSetDamage(float d) { setDamage(d); }
    void testSetFireInterval(float i) { setFireInterval(i); }

protected:
    void fire(Enemy& target, const std::vector<Enemy*>& /*enemies*/,
              std::vector<std::unique_ptr<Projectile> >& /*projectiles*/) override {
        fireCount++;
        lastTarget = &target;
    }

    void applyUpgrade() override {
        setDamage(getDamage() + 5.0f);
    }
};

class DummyEnemyPath {
protected:
    std::vector<Vector2> path;

    DummyEnemyPath(float x, float y)
        : path{Vector2(x, y), Vector2(x + 1000.0f, y)} {}
};

class DummyEnemy : private DummyEnemyPath, public Enemy {
public:
    DummyEnemy(float x, float y, float hp = 100.0f, float distance = 0.0f)
        : DummyEnemyPath(x, y), Enemy(DummyEnemyPath::path, hp, 10.0f, 20, 10) {
        distanceTravelled = distance;
    }
};

} // namespace

TEST_SUITE("Tower") {

TEST_CASE("inicializacao com Cell e valores validos") {
    Cell cell = {2, 3};
    DummyTower tower(cell, 150.0f, 20.0f, 1.5f, 100);

    CHECK(tower.getCell() == cell);
    CHECK(tower.getPosition().x == doctest::Approx(2.0f));
    CHECK(tower.getPosition().y == doctest::Approx(3.0f));
    CHECK(tower.getRange() == doctest::Approx(150.0f));
    CHECK(tower.getDamage() == doctest::Approx(20.0f));
    CHECK(tower.getFireInterval() == doctest::Approx(1.5f));
    CHECK(tower.getCost() == 100);
    CHECK(tower.getLevel() == 1);
    CHECK(tower.getCooldown() == doctest::Approx(0.0f));
    CHECK(tower.getSellValue() == 50); // 50% de 100
    CHECK(tower.canUpgrade() == true);
    CHECK(tower.getTargetPriority() == TargetPriority::First);
}

TEST_CASE("tratamento de excecoes no construtor de Tower") {
    Cell cell = {0, 0};

    // range <= 0
    CHECK_THROWS_AS(DummyTower(cell, 0.0f, 10.0f, 1.0f, 100), std::invalid_argument);
    CHECK_THROWS_AS(DummyTower(cell, -5.0f, 10.0f, 1.0f, 100), std::invalid_argument);

    // damage <= 0
    CHECK_THROWS_AS(DummyTower(cell, 10.0f, 0.0f, 1.0f, 100), std::invalid_argument);
    CHECK_THROWS_AS(DummyTower(cell, 10.0f, -2.0f, 1.0f, 100), std::invalid_argument);

    // fireInterval <= 0
    CHECK_THROWS_AS(DummyTower(cell, 10.0f, 10.0f, 0.0f, 100), std::invalid_argument);
    CHECK_THROWS_AS(DummyTower(cell, 10.0f, 10.0f, -1.0f, 100), std::invalid_argument);

    // cost <= 0
    CHECK_THROWS_AS(DummyTower(cell, 10.0f, 10.0f, 1.0f, 0), std::invalid_argument);
    CHECK_THROWS_AS(DummyTower(cell, 10.0f, 10.0f, 1.0f, -50), std::invalid_argument);
}

TEST_CASE("tratamento de excecoes nos setters de Tower") {
    Cell cell = {0, 0};
    DummyTower tower(cell, 50.0f, 10.0f, 1.0f, 100);

    CHECK_THROWS_AS(tower.testSetRange(0.0f), std::invalid_argument);
    CHECK_THROWS_AS(tower.testSetRange(-1.0f), std::invalid_argument);

    CHECK_THROWS_AS(tower.testSetDamage(0.0f), std::invalid_argument);
    CHECK_THROWS_AS(tower.testSetDamage(-5.0f), std::invalid_argument);

    CHECK_THROWS_AS(tower.testSetFireInterval(0.0f), std::invalid_argument);
    CHECK_THROWS_AS(tower.testSetFireInterval(-0.5f), std::invalid_argument);
}

TEST_CASE("upgrade da Tower e excecao de nivel maximo") {
    Cell cell = {0, 0};
    DummyTower tower(cell, 50.0f, 10.0f, 1.0f, 100);

    // Nivel 1 -> 2
    CHECK(tower.getLevel() == 1);
    CHECK(tower.canUpgrade() == true);
    tower.upgrade();
    CHECK(tower.getLevel() == 2);
    CHECK(tower.getDamage() == doctest::Approx(15.0f));
    CHECK(tower.getUpgradeCost() == 150); // (100 * 2 * 3) / 4 = 150
    // Investido: 100 + 75 = 175 -> sellValue = 175 / 2 = 87
    CHECK(tower.getSellValue() == 87);

    // Nivel 2 -> 3 (MAX_LEVEL)
    CHECK(tower.canUpgrade() == true);
    tower.upgrade();
    CHECK(tower.getLevel() == 3);
    CHECK(tower.getDamage() == doctest::Approx(20.0f));
    CHECK_FALSE(tower.canUpgrade());

    // Nivel 3 -> tentar upgrade lanca std::logic_error
    CHECK_THROWS_AS(tower.upgrade(), std::logic_error);
}

TEST_CASE("verificacao de alcance isInRange") {
    Cell cell = {0, 0};
    DummyTower tower(cell, 10.0f, 10.0f, 1.0f, 100);

    DummyEnemy dentro(5.0f, 0.0f);
    DummyEnemy naBorda(10.0f, 0.0f);
    DummyEnemy fora(10.1f, 0.0f);

    CHECK(tower.isInRange(dentro));
    CHECK(tower.isInRange(naBorda));
    CHECK_FALSE(tower.isInRange(fora));
}

TEST_CASE("controle de cooldown via update") {
    Cell cell = {0, 0};
    DummyTower tower(cell, 10.0f, 10.0f, 1.0f, 100);
    DummyEnemy enemy(5.0f, 0.0f);
    std::vector<Enemy*> enemies = {&enemy};
    std::vector<std::unique_ptr<Projectile> > projectiles;

    tower.tryAttack(enemies, projectiles);
    CHECK(tower.getCooldown() == doctest::Approx(1.0f));

    tower.update(0.4f);
    CHECK(tower.getCooldown() == doctest::Approx(0.6f));

    tower.update(0.6f);
    CHECK(tower.getCooldown() == doctest::Approx(0.0f));

    // update extra nao deixa cooldown negativo
    tower.update(0.5f);
    CHECK(tower.getCooldown() == doctest::Approx(0.0f));
}

TEST_CASE("tryAttack segue Template Method") {
    Cell cell = {0, 0};
    DummyTower tower(cell, 10.0f, 10.0f, 2.0f, 100);
    std::vector<std::unique_ptr<Projectile> > projectiles;

    SUBCASE("Sem inimigos no alcance nao dispara e mantem cooldown zerado") {
        DummyEnemy fora(50.0f, 0.0f);
        std::vector<Enemy*> enemies = {&fora};

        tower.tryAttack(enemies, projectiles);
        CHECK(tower.fireCount == 0);
        CHECK(tower.getCooldown() == doctest::Approx(0.0f));
    }

    SUBCASE("Com inimigo no alcance dispara e define cooldown") {
        DummyEnemy dentro(5.0f, 0.0f);
        std::vector<Enemy*> enemies = {&dentro};

        tower.tryAttack(enemies, projectiles);
        CHECK(tower.fireCount == 1);
        CHECK(tower.lastTarget == &dentro);
        CHECK(tower.getCooldown() == doctest::Approx(2.0f));

        // Segundo tryAttack imediato nao atira pois cooldown > 0
        tower.tryAttack(enemies, projectiles);
        CHECK(tower.fireCount == 1);
    }
}

TEST_CASE("selecao de alvo por TargetPriority") {
    Cell cell = {0, 0};
    DummyTower tower(cell, 50.0f, 10.0f, 1.0f, 100);
    std::vector<std::unique_ptr<Projectile> > projectiles;

    DummyEnemy e1(20.0f, 0.0f, 50.0f, 10.0f);  // HP 50, distTravelled 10, distTorre 20
    DummyEnemy e2(40.0f, 0.0f, 150.0f, 30.0f); // HP 150, distTravelled 30, distTorre 40
    DummyEnemy e3(10.0f, 0.0f, 20.0f, 20.0f);  // HP 20, distTravelled 20, distTorre 10
    DummyEnemy fora(100.0f, 0.0f, 5.0f, 50.0f); // Fora do alcance

    std::vector<Enemy*> enemies = {&e1, &e2, &e3, &fora};

    SUBCASE("Prioridade First escolhe o que percorreu maior distancia no caminho") {
        tower.setTargetPriority(TargetPriority::First);
        tower.tryAttack(enemies, projectiles);
        CHECK(tower.lastTarget == &e2);
    }

    SUBCASE("Prioridade Strongest escolhe o com mais HP") {
        tower.setTargetPriority(TargetPriority::Strongest);
        tower.tryAttack(enemies, projectiles);
        CHECK(tower.lastTarget == &e2);
    }

    SUBCASE("Prioridade Weakest escolhe o com menos HP") {
        tower.setTargetPriority(TargetPriority::Weakest);
        tower.tryAttack(enemies, projectiles);
        CHECK(tower.lastTarget == &e3);
    }

    SUBCASE("Prioridade Closest escolhe o mais proximo da torre") {
        tower.setTargetPriority(TargetPriority::Closest);
        tower.tryAttack(enemies, projectiles);
        CHECK(tower.lastTarget == &e3);
    }
}

} // TEST_SUITE("Tower")
