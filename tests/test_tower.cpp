#include <doctest/doctest.h>
#include "Tower.hpp"

class TorreTeste : public Tower {
public:
    TorreTeste(float x, float y, float w, float h, float range, float dmg, float rate, int cost)
        : Tower(x, y, w, h, range, dmg, rate, cost) {}

    void update(float) override {}
    void attack(std::vector<Enemy*>&, std::vector<Projectile*>&, float) override {}
};

TEST_CASE("Testando a construcao e evolucao da Torre") {
    TorreTeste torre(10, 10, 64, 64, 150.0f, 20.0f, 1.0f, 100);

    SUBCASE("Verifica se os valores iniciais estao corretos") {
        CHECK(torre.getCost() == 100);
        CHECK(torre.getLevel() == 1);
        CHECK(torre.getDamage() == 20.0f);
        CHECK(torre.getRange() == 150.0f);
        CHECK(torre.getFireRate() == 1.0f);
    }

    SUBCASE("Verifica se o upgrade sobe o nivel e o dano da torre") {
        torre.upgrade();
        CHECK(torre.getLevel() == 2); 
        CHECK(torre.getDamage() > 20.0f);
        CHECK(torre.getRange() > 150.0f);
    }
}