#include <doctest/doctest.h>

#include <memory>
#include <stdexcept>

#include "Cell.hpp"
#include "Tower.hpp"
#include "TowerFactory.hpp"
#include "TowerType.hpp"

TEST_SUITE("TowerFactory") {

TEST_CASE("parseType converte nomes validos para TowerType") {
    CHECK(TowerFactory::parseType("firewall") == TowerType::Firewall);
    CHECK(TowerFactory::parseType("antivirus") == TowerType::Antivirus);
    CHECK(TowerFactory::parseType("scanner") == TowerType::Scanner);
}

TEST_CASE("parseType lanca invalid_argument para nome desconhecido") {
    CHECK_THROWS_AS(TowerFactory::parseType("laser"), std::invalid_argument);
    CHECK_THROWS_AS(TowerFactory::parseType("FIREWALL"), std::invalid_argument);
    CHECK_THROWS_AS(TowerFactory::parseType(""), std::invalid_argument);
}

TEST_CASE("create instancia a subclasse correta para cada TowerType") {
    Cell cell = {2, 3};

    std::unique_ptr<Tower> firewall = TowerFactory::create(TowerType::Firewall, cell);
    REQUIRE(firewall != nullptr);
    CHECK(firewall->getSymbol() == 'F');
    CHECK(firewall->getCost() == 50);
    CHECK(firewall->getRange() == doctest::Approx(2.5f));
    CHECK(firewall->getDamage() == doctest::Approx(6.0f));

    std::unique_ptr<Tower> antivirus = TowerFactory::create(TowerType::Antivirus, cell);
    REQUIRE(antivirus != nullptr);
    CHECK(antivirus->getSymbol() == 'A');
    CHECK(antivirus->getCost() == 100);
    CHECK(antivirus->getRange() == doctest::Approx(4.0f));
    CHECK(antivirus->getDamage() == doctest::Approx(30.0f));

    std::unique_ptr<Tower> scanner = TowerFactory::create(TowerType::Scanner, cell);
    REQUIRE(scanner != nullptr);
    CHECK(scanner->getSymbol() == 'S');
    CHECK(scanner->getCost() == 120);
    CHECK(scanner->getRange() == doctest::Approx(2.0f));
    CHECK(scanner->getDamage() == doctest::Approx(8.0f));
}

} // TEST_SUITE("TowerFactory")
