#include <doctest/doctest.h>

#include "Vector2.hpp"

TEST_SUITE("Vector2") {

TEST_CASE("construtor padrao cria o vetor nulo") {
    Vector2 v;
    CHECK(v.x == doctest::Approx(0.0f));
    CHECK(v.y == doctest::Approx(0.0f));
}

TEST_CASE("construtor com componentes guarda os valores") {
    Vector2 v(1.5f, -2.0f);
    CHECK(v.x == doctest::Approx(1.5f));
    CHECK(v.y == doctest::Approx(-2.0f));
}

TEST_CASE("length calcula a norma euclidiana") {
    CHECK(Vector2(3.0f, 4.0f).length() == doctest::Approx(5.0f));
    CHECK(Vector2().length() == doctest::Approx(0.0f));
}

TEST_CASE("distanceTo e simetrica") {
    Vector2 a(1.0f, 1.0f);
    Vector2 b(4.0f, 5.0f);
    CHECK(a.distanceTo(b) == doctest::Approx(5.0f));
    CHECK(b.distanceTo(a) == doctest::Approx(5.0f));
    CHECK(a.distanceTo(a) == doctest::Approx(0.0f));
}

TEST_CASE("operadores de soma, subtracao e escala") {
    Vector2 a(1.0f, 2.0f);
    Vector2 b(3.0f, -1.0f);

    Vector2 sum = a + b;
    CHECK(sum.x == doctest::Approx(4.0f));
    CHECK(sum.y == doctest::Approx(1.0f));

    Vector2 diff = a - b;
    CHECK(diff.x == doctest::Approx(-2.0f));
    CHECK(diff.y == doctest::Approx(3.0f));

    Vector2 scaled = a * 2.5f;
    CHECK(scaled.x == doctest::Approx(2.5f));
    CHECK(scaled.y == doctest::Approx(5.0f));
}

}
