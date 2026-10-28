#include <doctest/doctest.h>
#include "Tower.hpp"
#include "Enemy.hpp"
#include "TargetPriority.hpp"
#include <vector>

class TorreDummy : public Tower {
public:
    TorreDummy(float x, float y, float w, float h, float range, float dmg, float rate, int cost,
               TargetPriority priority = TargetPriority::First)
        : Tower(x, y, w, h, range, dmg, rate, cost, priority) {}

    void attack(std::vector<Enemy*>&, std::vector<Projectile*>&, float) override {}
};

class InimigoDummy : public Enemy {
public:
    InimigoDummy(float x, float y, float hp, float speed = 10.0f, std::size_t waypointIdx = 0)
        : Enemy(x, y, 32.0f, 32.0f, hp, speed, 20) {
        _waypointIndex = waypointIdx;
    }
};

TEST_SUITE("Tower") {

TEST_CASE("construcao e getters basicos da Tower") {
    TorreDummy torre(10.0f, 10.0f, 64.0f, 64.0f, 150.0f, 20.0f, 2.0f, 100);

    CHECK(torre.getCost() == 100);
    CHECK(torre.getLevel() == 1);
    CHECK(torre.getDamage() == doctest::Approx(20.0f));
    CHECK(torre.getRange() == doctest::Approx(150.0f));
    CHECK(torre.getFireRate() == doctest::Approx(2.0f));
    CHECK(torre.getCooldown() == doctest::Approx(0.0f));
    CHECK(torre.canShoot() == true);
    CHECK(torre.getTargetPriority() == TargetPriority::First);
}

TEST_CASE("upgrade da Tower aumenta atributos e custo") {
    TorreDummy torre(0.0f, 0.0f, 64.0f, 64.0f, 100.0f, 40.0f, 1.0f, 100);

    torre.upgrade();
    CHECK(torre.getLevel() == 2);
    CHECK(torre.getDamage() == doctest::Approx(50.0f));
    CHECK(torre.getRange() == doctest::Approx(110.0f));
    CHECK(torre.getCost() == 150);
}

TEST_CASE("deteccao de inimigos no raio de alcance") {
    TorreDummy torre(0.0f, 0.0f, 32.0f, 32.0f, 50.0f, 10.0f, 1.0f, 100);

    InimigoDummy dentro(30.0f, 0.0f, 100.0f);
    InimigoDummy naBorda(50.0f, 0.0f, 100.0f);
    InimigoDummy fora(50.1f, 0.0f, 100.0f);

    CHECK(torre.isInRange(dentro));
    CHECK(torre.isInRange(naBorda));
    CHECK_FALSE(torre.isInRange(fora));
}

TEST_CASE("controle de cadencia e recarga (cooldown)") {
    TorreDummy torre(0.0f, 0.0f, 32.0f, 32.0f, 100.0f, 10.0f, 2.0f, 100);

    CHECK(torre.canShoot());

    torre.resetCooldown();
    CHECK_FALSE(torre.canShoot());
    CHECK(torre.getCooldown() == doctest::Approx(0.5f));

    torre.updateCooldown(0.2f);
    CHECK_FALSE(torre.canShoot());
    CHECK(torre.getCooldown() == doctest::Approx(0.3f));

    torre.updateCooldown(0.3f);
    CHECK(torre.canShoot());
    CHECK(torre.getCooldown() == doctest::Approx(0.0f));
}

TEST_CASE("selecao de alvos por TargetPriority") {
    TorreDummy torre(0.0f, 0.0f, 32.0f, 32.0f, 100.0f, 10.0f, 1.0f, 100);

    // Inimigos em diferentes posicoes, vidas e waypoints
    InimigoDummy e1(20.0f, 0.0f, 50.0f, 10.0f, 1);
    InimigoDummy e2(40.0f, 0.0f, 150.0f, 10.0f, 3);
    InimigoDummy e3(10.0f, 0.0f, 20.0f, 10.0f, 2);
    InimigoDummy fora(150.0f, 0.0f, 5.0f, 10.0f, 5);

    std::vector<Enemy*> lista = { &e1, &e2, &e3, &fora };

    SUBCASE("Prioridade First escolhe o mais avancado nos waypoints") {
        torre.setTargetPriority(TargetPriority::First);
        CHECK(torre.selectTarget(lista) == &e2);
    }

    SUBCASE("Prioridade Strongest escolhe o com mais HP") {
        torre.setTargetPriority(TargetPriority::Strongest);
        CHECK(torre.selectTarget(lista) == &e2);
    }

    SUBCASE("Prioridade Weakest escolhe o com menos HP") {
        torre.setTargetPriority(TargetPriority::Weakest);
        CHECK(torre.selectTarget(lista) == &e3);
    }

    SUBCASE("Prioridade Closest escolhe o mais proximo da torre") {
        torre.setTargetPriority(TargetPriority::Closest);
        CHECK(torre.selectTarget(lista) == &e3);
    }

    SUBCASE("Retorna nullptr quando nao ha nenhum inimigo no alcance ou todos mortos") {
        std::vector<Enemy*> vazia;
        CHECK(torre.selectTarget(vazia) == nullptr);

        e1.takeDamage(100.0f);
        e2.takeDamage(200.0f);
        e3.takeDamage(50.0f);
        CHECK(torre.selectTarget(lista) == nullptr);
    }
}

} // TEST_SUITE("Tower")