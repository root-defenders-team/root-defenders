#include <doctest/doctest.h>

#include <sstream>
#include <stdexcept>

#include "CellType.hpp"
#include "MapLoader.hpp"

TEST_SUITE("MapLoader") {

TEST_CASE("creates a map from a valid text input") {
    std::stringstream input("E#@\n...\nX..\n");

    Map map = MapLoader::parse(input, "mapa_teste");

    CHECK(map.getWidth() == 3);
    CHECK(map.getHeight() == 3);
    CHECK(map.getCellType({0, 0}) == CellType::Path);
    CHECK(map.getCellType({1, 0}) == CellType::Path);
    CHECK(map.getCellType({1, 1}) == CellType::Buildable);
    CHECK(map.getCellType({0, 2}) == CellType::Blocked);
    REQUIRE(map.getWaypoints().size() == 3);
    CHECK(map.getWaypoints()[0] == Cell{0, 0});
    CHECK(map.getWaypoints()[2] == Cell{2, 0});
    CHECK(map.hasPath());
}

TEST_CASE("rejects maps with inconsistent line sizes") {
    std::stringstream input("E#@\n..\n");

    CHECK_THROWS_AS(MapLoader::parse(input, "mapa_teste"), std::runtime_error);
}

TEST_CASE("rejects invalid characters and missing entry") {
    std::stringstream invalidCharacter("E?@\n");
    std::stringstream missingEntry(".#@\n");

    CHECK_THROWS_AS(MapLoader::parse(invalidCharacter, "mapa_teste"),
                    std::runtime_error);
    CHECK_THROWS_AS(MapLoader::parse(missingEntry, "mapa_teste"),
                    std::runtime_error);
}

TEST_CASE("rejects interrupted or forked paths") {
    std::stringstream interruptedPath("E.X\n..@\n");
    std::stringstream forkedPath("E#@\n.#.\n");

    CHECK_THROWS_AS(MapLoader::parse(interruptedPath, "mapa_teste"),
                    std::runtime_error);
    CHECK_THROWS_AS(MapLoader::parse(forkedPath, "mapa_teste"),
                    std::runtime_error);
}

} // TEST_SUITE("MapLoader")
