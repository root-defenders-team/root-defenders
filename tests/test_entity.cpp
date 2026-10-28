#include <doctest/doctest.h>

#include "Entity.hpp"
#include "Vector2.hpp"
#include "Rect.hpp"

// Classe concreta mínima apenas para permitir instanciar e testar a base Entity
class DummyEntity : public Entity {
public:
    DummyEntity(float x, float y, float w, float h) : Entity(x, y, w, h) {}
    void update(float) override {}
};

TEST_SUITE("Entity") {

TEST_CASE("initializes with correct position, size, and active state") {
    DummyEntity entity(10.5f, 20.0f, 30.0f, 40.0f);

    CHECK(entity.getPosition().x == 10.5f);
    CHECK(entity.getPosition().y == 20.0f);
    
    // Assumindo que a entidade inicie ativa por padrão no construtor
    CHECK(entity.isActive() == true); 

    Rect bounds = entity.getBounds();
    CHECK(bounds.left == 10.5f);
    CHECK(bounds.top == 20.0f);
    CHECK(bounds.width == 30.0f);
    CHECK(bounds.height == 40.0f);
}

TEST_CASE("updates active state") {
    DummyEntity entity(0, 0, 10, 10);

    entity.setActive(false);
    CHECK_FALSE(entity.isActive());

    entity.setActive(true);
    CHECK(entity.isActive());
}

TEST_CASE("updates position coordinates") {
    DummyEntity entity(0, 0, 10, 10);

    entity.setPosition(Vector2(100.0f, 150.5f));
    CHECK(entity.getPosition().x == 100.0f);
    CHECK(entity.getPosition().y == 150.5f);

    Rect bounds = entity.getBounds();
    CHECK(bounds.left == 100.0f);
    CHECK(bounds.top == 150.5f);
}

TEST_CASE("detects collision with overlapping bounding boxes") {
    DummyEntity e1(0.0f, 0.0f, 20.0f, 20.0f);
    DummyEntity e2(10.0f, 10.0f, 20.0f, 20.0f);
    DummyEntity e3(19.0f, 19.0f, 5.0f, 5.0f);

    CHECK(e1.checkCollision(e2));
    CHECK(e2.checkCollision(e1));
    CHECK(e1.checkCollision(e3));
}

TEST_CASE("does not detect collision when bounding boxes are separated") {
    DummyEntity e1(0.0f, 0.0f, 10.0f, 10.0f);
    DummyEntity e2(50.0f, 50.0f, 10.0f, 10.0f);
    DummyEntity e3(10.1f, 0.0f, 10.0f, 10.0f);

    CHECK_FALSE(e1.checkCollision(e2));
    CHECK_FALSE(e2.checkCollision(e1));
    CHECK_FALSE(e1.checkCollision(e3));
}

} // TEST_SUITE("Entity")