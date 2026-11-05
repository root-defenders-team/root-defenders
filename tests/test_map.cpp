#include <doctest/doctest.h>

#include <stdexcept>

#include "Cell.hpp"
#include "CellType.hpp"
#include "Map.hpp"

TEST_SUITE("Map") {

TEST_CASE("rejects maps without positive dimensions") {
    CHECK_THROWS_AS(Map(0, 5), std::invalid_argument);
    CHECK_THROWS_AS(Map(5, 0), std::invalid_argument);
    CHECK_NOTHROW(Map(1, 1));
}

TEST_CASE("reports dimensions and map limits") {
    Map map(4, 3);

    CHECK(map.getWidth() == 4);
    CHECK(map.getHeight() == 3);
    CHECK(map.isInside({0, 0}));
    CHECK(map.isInside({3, 2}));
    CHECK_FALSE(map.isInside({-1, 0}));
    CHECK_FALSE(map.isInside({0, -1}));
    CHECK_FALSE(map.isInside({4, 2}));
    CHECK_FALSE(map.isInside({3, 3}));
}

TEST_CASE("starts with buildable cells") {
    Map map(3, 3);

    CHECK(map.getCellType({1, 1}) == CellType::Buildable);
    CHECK(map.isBuildable({1, 1}));
    CHECK_FALSE(map.isPath({1, 1}));
    CHECK_FALSE(map.isBlocked({1, 1}));
    CHECK(map.isBlocked({-1, 0}));
}

TEST_CASE("changes cell types and protects invalid positions") {
    Map map(3, 3);

    map.setCellType({1, 1}, CellType::Path);
    CHECK(map.isPath({1, 1}));
    CHECK_FALSE(map.isBuildable({1, 1}));

    map.setCellType({2, 2}, CellType::Blocked);
    CHECK(map.isBlocked({2, 2}));
    CHECK_FALSE(map.isBuildable({2, 2}));

    CHECK_THROWS_AS(map.getCellType({3, 0}), std::out_of_range);
    CHECK_THROWS_AS(map.setCellType({0, -1}, CellType::Path), std::out_of_range);
}

TEST_CASE("occupies and releases only buildable cells") {
    Map map(3, 3);

    CHECK(map.occupyCell({1, 1}));
    CHECK_FALSE(map.isBuildable({1, 1}));
    CHECK_FALSE(map.occupyCell({1, 1}));
    CHECK(map.releaseCell({1, 1}));
    CHECK(map.isBuildable({1, 1}));
    CHECK_FALSE(map.releaseCell({1, 1}));

    map.setCellType({0, 0}, CellType::Path);
    map.setCellType({2, 2}, CellType::Blocked);
    CHECK_FALSE(map.occupyCell({0, 0}));
    CHECK_FALSE(map.occupyCell({2, 2}));
    CHECK_FALSE(map.occupyCell({3, 0}));
}

TEST_CASE("stores valid waypoints in insertion order") {
    Map map(4, 4);

    CHECK_FALSE(map.hasPath());
    map.addWaypoint({0, 0});
    CHECK_FALSE(map.hasPath());
    map.addWaypoint({3, 3});

    REQUIRE(map.getWaypoints().size() == 2);
    CHECK(map.getWaypoints()[0] == Cell{0, 0});
    CHECK(map.getWaypoints()[1] == Cell{3, 3});
    CHECK(map.hasPath());
    CHECK_THROWS_AS(map.addWaypoint({4, 0}), std::out_of_range);
}

TEST_CASE("converts waypoints to an enemy path") {
    Map map(4, 4);
    map.addWaypoint({0, 1});
    map.addWaypoint({2, 1});
    map.addWaypoint({2, 3});

    const std::vector<Vector2>& path = map.getPath();

    REQUIRE(path.size() == 3);
    CHECK(path[0].x == 0.0f);
    CHECK(path[0].y == 1.0f);
    CHECK(path[1].x == 2.0f);
    CHECK(path[1].y == 1.0f);
    CHECK(path[2].x == 2.0f);
    CHECK(path[2].y == 3.0f);

    map.addWaypoint({1, 3});
    REQUIRE(path.size() == 4);
    CHECK(path[3].x == 1.0f);
    CHECK(path[3].y == 3.0f);
}

} // TEST_SUITE("Map")
