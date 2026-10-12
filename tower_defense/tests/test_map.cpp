#include <doctest/doctest.h>

#include <stdexcept>

#include "Map.hpp"

TEST_SUITE("Map") {

TEST_CASE("rejects maps without positive dimensions") {
    CHECK_THROWS_AS(Map(0, 5), std::invalid_argument);
    CHECK_THROWS_AS(Map(5, 0), std::invalid_argument);
    CHECK_THROWS_AS(Map(0, 0), std::invalid_argument);

    CHECK_NOTHROW(Map(1, 1));
}

TEST_CASE("reports dimensions and valid coordinates") {
    Map map(4, 3);

    CHECK(map.width() == 4);
    CHECK(map.height() == 3);
    CHECK(map.isInside(0, 0));
    CHECK(map.isInside(0, 2));
    CHECK(map.isInside(3, 0));
    CHECK(map.isInside(3, 2));
    CHECK_FALSE(map.isInside(-1, 0));
    CHECK_FALSE(map.isInside(0, -1));
    CHECK_FALSE(map.isInside(4, 2));
    CHECK_FALSE(map.isInside(3, 3));
    CHECK_FALSE(map.isInside(4, 3));
}

TEST_CASE("starts with traversable terrain and rejects invalid cells") {
    Map map(3, 3);

    CHECK(map.isTraversable(1, 1));
    CHECK_FALSE(map.isBlocked(1, 1));
    CHECK(map.isBlocked(-1, 0));
    CHECK_FALSE(map.isTraversable(-1, 0));
    CHECK_FALSE(map.isCellFree(-1, 0));
    CHECK_FALSE(map.setTraversable(-1, 0, false));
    CHECK_FALSE(map.setTraversable(-1, 0, true));
    CHECK_FALSE(map.setTraversable(3, 1, false));
}

TEST_CASE("changes traversability without affecting construction availability") {
    Map map(3, 3);

    CHECK(map.setTraversable(2, 2, false));
    CHECK(map.isBlocked(2, 2));
    CHECK_FALSE(map.isTraversable(2, 2));
    CHECK(map.isCellFree(2, 2));

    CHECK(map.setTraversable(2, 2, true));
    CHECK(map.isTraversable(2, 2));
    CHECK_FALSE(map.isBlocked(2, 2));
}

TEST_CASE("allows construction on blocked terrain") {
    Map map(3, 3);

    REQUIRE(map.setTraversable(1, 1, false));
    CHECK(map.isCellFree(1, 1));
    CHECK(map.occupyCell(1, 1));
    CHECK_FALSE(map.isCellFree(1, 1));
    CHECK(map.isBlocked(1, 1));
}

TEST_CASE("occupies and releases only valid free cells") {
    Map map(3, 3);

    CHECK_FALSE(map.occupyCell(-1, 0));
    CHECK_FALSE(map.occupyCell(3, 0));
    CHECK(map.isCellFree(1, 1));
    CHECK(map.occupyCell(1, 1));
    CHECK_FALSE(map.isCellFree(1, 1));
    CHECK_FALSE(map.occupyCell(1, 1));
    CHECK(map.isTraversable(1, 1));

    CHECK_FALSE(map.releaseCell(-1, 0));
    CHECK(map.releaseCell(1, 1));
    CHECK(map.isCellFree(1, 1));
    CHECK_FALSE(map.releaseCell(1, 1));
}

TEST_CASE("does not block a cell occupied by a tower") {
    Map map(3, 3);

    REQUIRE(map.occupyCell(1, 1));
    CHECK_FALSE(map.setTraversable(1, 1, false));
    CHECK(map.isTraversable(1, 1));
    CHECK_FALSE(map.isBlocked(1, 1));
}

TEST_CASE("stores only valid waypoints in insertion order") {
    Map map(4, 4);

    CHECK(map.waypoints().empty());
    map.addWaypoint({0, 0});
    map.addWaypoint({3, 3});
    map.addWaypoint({3, 2});

    REQUIRE(map.waypoints().size() == 3);
    CHECK(map.waypoints()[0].x == 0);
    CHECK(map.waypoints()[0].y == 0);
    CHECK(map.waypoints()[1].x == 3);
    CHECK(map.waypoints()[1].y == 3);
    CHECK(map.waypoints()[2].x == 3);
    CHECK(map.waypoints()[2].y == 2);
    CHECK_THROWS_AS(map.addWaypoint({4, 0}), std::out_of_range);
    CHECK_THROWS_AS(map.addWaypoint({-1, 0}), std::out_of_range);
    CHECK(map.waypoints().size() == 3);
}

} // TEST_SUITE("Map")
