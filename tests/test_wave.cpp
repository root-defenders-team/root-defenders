#include <doctest/doctest.h>

#include <limits>
#include <stdexcept>
#include <vector>

#include "SpawnGroup.hpp"
#include "Wave.hpp"

TEST_SUITE("Wave") {

TEST_CASE("rejects empty groups and invalid group values") {
    CHECK_THROWS_AS(Wave(std::vector<SpawnGroup>()), std::invalid_argument);

    CHECK_THROWS_AS(
        Wave(std::vector<SpawnGroup>{{EnemyType::Virus, 0, 1.0f}}),
        std::invalid_argument);
    CHECK_THROWS_AS(
        Wave(std::vector<SpawnGroup>{{EnemyType::Virus, -1, 1.0f}}),
        std::invalid_argument);
    CHECK_THROWS_AS(
        Wave(std::vector<SpawnGroup>{{EnemyType::Virus, 1, -1.0f}}),
        std::invalid_argument);
    CHECK_THROWS_AS(
        Wave(std::vector<SpawnGroup>{
            {EnemyType::Virus, 1, std::numeric_limits<float>::infinity()}}),
        std::invalid_argument);
    CHECK_THROWS_AS(
        Wave(std::vector<SpawnGroup>{
            {EnemyType::Virus, 1, std::numeric_limits<float>::quiet_NaN()}}),
        std::invalid_argument);
}

TEST_CASE("rejects a total enemy count that overflows int") {
    CHECK_THROWS_AS(
        Wave(std::vector<SpawnGroup>{
            {EnemyType::Virus, std::numeric_limits<int>::max(), 0.0f},
            {EnemyType::Worm, 1, 0.0f}}),
        std::overflow_error);
}

TEST_CASE("spawns the first enemy immediately and respects the interval") {
    Wave wave(std::vector<SpawnGroup>{{EnemyType::Virus, 3, 1.0f}});

    CHECK(wave.getTotalEnemies() == 3);
    CHECK_FALSE(wave.isFinishedSpawning());
    CHECK(wave.update(0.0f) == std::vector<EnemyType>{EnemyType::Virus});
    CHECK(wave.update(0.5f).empty());
    CHECK(wave.update(0.5f) == std::vector<EnemyType>{EnemyType::Virus});
    CHECK(wave.update(1.0f) == std::vector<EnemyType>{EnemyType::Virus});
    CHECK(wave.isFinishedSpawning());
    CHECK(wave.update(1.0f).empty());
}

TEST_CASE("spawns zero-interval groups in the same tick") {
    Wave wave(std::vector<SpawnGroup>{{EnemyType::Worm, 3, 0.0f}});

    CHECK(wave.update(0.0f) == std::vector<EnemyType>(3, EnemyType::Worm));
    CHECK(wave.isFinishedSpawning());
}

TEST_CASE("spawns groups sequentially and reset restarts the wave") {
    Wave wave(std::vector<SpawnGroup>{
        {EnemyType::Virus, 2, 1.0f},
        {EnemyType::Rootkit, 1, 0.5f}});

    CHECK(wave.update(0.1f) == std::vector<EnemyType>{EnemyType::Virus});
    CHECK(wave.update(1.0f) == std::vector<EnemyType>{EnemyType::Virus});
    CHECK_FALSE(wave.isFinishedSpawning());

    CHECK(wave.update(0.0f) == std::vector<EnemyType>{EnemyType::Rootkit});
    CHECK(wave.isFinishedSpawning());

    wave.reset();
    CHECK_FALSE(wave.isFinishedSpawning());
    CHECK(wave.getTotalEnemies() == 3);
    CHECK(wave.update(0.0f) == std::vector<EnemyType>{EnemyType::Virus});
}

TEST_CASE("rejects negative and non-finite tick durations") {
    Wave wave(std::vector<SpawnGroup>{{EnemyType::Virus, 1, 1.0f}});

    CHECK_THROWS_AS(wave.update(-0.1f), std::invalid_argument);
    CHECK_THROWS_AS(
        wave.update(std::numeric_limits<float>::infinity()),
        std::invalid_argument);
    CHECK_THROWS_AS(
        wave.update(std::numeric_limits<float>::quiet_NaN()),
        std::invalid_argument);
    CHECK(wave.update(0.0f) == std::vector<EnemyType>{EnemyType::Virus});
}

} // TEST_SUITE("Wave")
