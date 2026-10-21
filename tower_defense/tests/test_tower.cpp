#include "../include/doctest.h"
#include "../include/Tower.hpp"

class TorreTeste : public Tower {
public:
    TorreTeste(float x, float y, float w, float h, float range, float dmg, float rate, int cost)
        : Tower(x, y, w, h, range, dmg, rate, cost) {}

    void update(float deltaTime) override {}
    void render(sf::RenderWindow& window) override {}
    void attack(std::vector<Enemy*>& enemies, std::vector<Projectile*>& projectiles, float deltaTime) override {}
};

TEST_CASE("Testando a construcao e evolucao da Torre") {
    TorreTeste torre(10, 10, 64, 64, 150.0f, 20.0f, 1.0f, 100);

    SUBCASE("Verifica se os valores iniciais estao corretos") {
        CHECK(torre.getCost() == 100);
        CHECK(torre.getLevel() == 1);
        CHECK(torre.getDamage() == 20.0f);
        CHECK(torre.getRange() == 150.0f);
    }

    SUBCASE("Verifica se o upgrade sobe o nivel e o dano da torre") {
        torre.upgrade();
        CHECK(torre.getLevel() == 2); 
        CHECK(torre.getDamage() > 20.0f);
    }
}