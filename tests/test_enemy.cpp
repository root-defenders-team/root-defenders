#include "../include/doctest.h"
#include "../include/Enemy.hpp"


class InimigoTeste : public Enemy {
public:
    InimigoTeste(float x, float y, float w, float h, float hp, float speed, int gold)
        : Enemy(x, y, w, h, hp, speed, gold) {}

    void update(float deltaTime) override {}
    void render(sf::RenderWindow& window) override {}
};

TEST_CASE("Testando a vida e recompensa do Inimigo") {
   
    InimigoTeste orc(0, 0, 32, 32, 100.0f, 10.0f, 50); 

    CHECK(orc.getGoldReward() == 50); 
    CHECK(orc.isDead() == false);
}