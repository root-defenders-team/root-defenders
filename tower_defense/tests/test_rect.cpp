#include <doctest/doctest.h>

#include "Rect.hpp"

TEST_SUITE("Rect") {

TEST_CASE("retangulos sobrepostos se intersectam") {
    Rect a = {0.0f, 0.0f, 2.0f, 2.0f};
    Rect b = {1.0f, 1.0f, 2.0f, 2.0f};
    CHECK(a.intersects(b));
    CHECK(b.intersects(a));
}

TEST_CASE("retangulo contido no outro se intersecta") {
    Rect outer = {0.0f, 0.0f, 10.0f, 10.0f};
    Rect inner = {4.0f, 4.0f, 1.0f, 1.0f};
    CHECK(outer.intersects(inner));
    CHECK(inner.intersects(outer));
}

TEST_CASE("retangulos separados nao se intersectam") {
    Rect a = {0.0f, 0.0f, 1.0f, 1.0f};
    CHECK_FALSE(a.intersects(Rect{5.0f, 0.0f, 1.0f, 1.0f}));
    CHECK_FALSE(a.intersects(Rect{0.0f, 5.0f, 1.0f, 1.0f}));
}

TEST_CASE("retangulos que so encostam na borda nao se intersectam") {
    Rect a = {0.0f, 0.0f, 1.0f, 1.0f};
    CHECK_FALSE(a.intersects(Rect{1.0f, 0.0f, 1.0f, 1.0f}));
    CHECK_FALSE(a.intersects(Rect{0.0f, 1.0f, 1.0f, 1.0f}));
}

}
