#include <doctest/doctest.h>

#include "Cell.hpp"

TEST_SUITE("Cell") {

TEST_CASE("aceita inicializacao por chaves") {
    Cell c = {3, 4};
    CHECK(c.x == 3);
    CHECK(c.y == 4);
}

TEST_CASE("operator== compara coluna e linha") {
    Cell a = {2, 5};
    Cell same = {2, 5};
    Cell otherColumn = {3, 5};
    Cell otherRow = {2, 6};

    CHECK(a == same);
    CHECK_FALSE(a == otherColumn);
    CHECK_FALSE(a == otherRow);
}

TEST_CASE("toVector2 converte para as mesmas coordenadas em float") {
    Cell c = {7, 1};
    Vector2 v = c.toVector2();
    CHECK(v.x == doctest::Approx(7.0f));
    CHECK(v.y == doctest::Approx(1.0f));
}

}
