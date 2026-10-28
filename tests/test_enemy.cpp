#include <doctest/doctest.h>
#include "Enemy.hpp"

class InimigoTeste : public Enemy {
public:
    InimigoTeste(float x, float y, float w, float h, float hp, float speed, int gold)
        : Enemy(x, y, w, h, hp, speed, gold) {}

    void update(float) override {}
};

TEST_CASE("Testando a vida e recompensa do Inimigo") {
    InimigoTeste orc(0, 0, 32, 32, 100.0f, 10.0f, 50); 

    CHECK(orc.getGoldReward() == 50); 
    CHECK(orc.isDead() == false);
    CHECK(orc.getHp() == 100.0f);
    CHECK(orc.getSpeed() == 10.0f);

    orc.takeDamage(30.0f);
    CHECK(orc.getHp() == 70.0f);
    CHECK_FALSE(orc.isDead());

    orc.takeDamage(70.0f);
    CHECK(orc.isDead() == true);
}