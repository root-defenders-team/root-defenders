#include <doctest/doctest.h>

#include <string>

#include "Cell.hpp"
#include "Command.hpp"
#include "CommandParser.hpp"
#include "GameException.hpp"
#include "InvalidCommandException.hpp"
#include "TargetPriority.hpp"

namespace {

/// Devolve a mensagem da InvalidCommandException lançada ao interpretar a linha.
std::string parseError(const std::string& line) {
    try {
        CommandParser::parse(line);
    } catch (const InvalidCommandException& error) {
        return error.what();
    }
    return "";
}

} // namespace

TEST_SUITE("CommandParser") {

TEST_CASE("splits a command into name and arguments") {
    Command command = CommandParser::parse("install firewall 5 3");

    CHECK(command.name == "install");
    REQUIRE(command.args.size() == 3);
    CHECK(command.args[0] == "firewall");
    CHECK(command.args[1] == "5");
    CHECK(command.args[2] == "3");
}

TEST_CASE("ignores extra spaces") {
    Command command = CommandParser::parse("   upgrade    2   4   ");

    CHECK(command.name == "upgrade");
    REQUIRE(command.args.size() == 2);
    CHECK(command.args[0] == "2");
    CHECK(command.args[1] == "4");
}

TEST_CASE("accepts every command of the game") {
    CHECK(CommandParser::parse("help").args.empty());
    CHECK(CommandParser::parse("rm 1 2").args.size() == 2);
    CHECK(CommandParser::parse("target 1 2 weak").args.size() == 3);
    CHECK(CommandParser::parse("ls").args.empty());
    CHECK(CommandParser::parse("status").args.empty());
    CHECK(CommandParser::parse("map").args.empty());
    CHECK(CommandParser::parse("man rootkit").args.size() == 1);
    CHECK(CommandParser::parse("run").args.empty());
    CHECK(CommandParser::parse("ranking").args.empty());
    CHECK(CommandParser::parse("rank").args.empty());
    CHECK(CommandParser::parse("exit").args.empty());
}

TEST_CASE("rejects an empty line and an unknown command") {
    CHECK_THROWS_AS(CommandParser::parse(""), InvalidCommandException);
    CHECK_THROWS_AS(CommandParser::parse("    "), InvalidCommandException);
    CHECK_THROWS_AS(CommandParser::parse("hack 1 2"), InvalidCommandException);
    CHECK_THROWS_AS(CommandParser::parse("INSTALL firewall 1 1"), InvalidCommandException);
    CHECK(parseError("hack").find("comando desconhecido: hack") != std::string::npos);
}

TEST_CASE("rejects the wrong number of arguments and shows the usage") {
    CHECK_THROWS_AS(CommandParser::parse("install firewall 5"), InvalidCommandException);
    CHECK_THROWS_AS(CommandParser::parse("run now"), InvalidCommandException);
    CHECK_THROWS_AS(CommandParser::parse("rm 1"), InvalidCommandException);
    CHECK(parseError("install firewall 5").find("install <tipo> <x> <y>") != std::string::npos);
}

TEST_CASE("errors can be caught as GameException") {
    CHECK_THROWS_AS(CommandParser::parse("hack"), GameException);
}

TEST_CASE("parseInt reads integers") {
    CHECK(CommandParser::parseInt("42") == 42);
    CHECK(CommandParser::parseInt("0") == 0);
    CHECK(CommandParser::parseInt("-3") == -3);
}

TEST_CASE("parseInt rejects anything that is not an integer") {
    CHECK_THROWS_AS(CommandParser::parseInt(""), InvalidCommandException);
    CHECK_THROWS_AS(CommandParser::parseInt("abc"), InvalidCommandException);
    CHECK_THROWS_AS(CommandParser::parseInt("3a"), InvalidCommandException);
    CHECK_THROWS_AS(CommandParser::parseInt("1.5"), InvalidCommandException);
    CHECK_THROWS_AS(CommandParser::parseInt("99999999999"), InvalidCommandException);
}

TEST_CASE("parseCell reads the column and the line") {
    Cell cell = CommandParser::parseCell("5", "3");

    CHECK(cell.x == 5);
    CHECK(cell.y == 3);
    CHECK_THROWS_AS(CommandParser::parseCell("x", "3"), InvalidCommandException);
    CHECK_THROWS_AS(CommandParser::parseCell("5", ""), InvalidCommandException);
}

TEST_CASE("parsePriority reads the four priorities") {
    CHECK(CommandParser::parsePriority("first") == TargetPriority::First);
    CHECK(CommandParser::parsePriority("strong") == TargetPriority::Strongest);
    CHECK(CommandParser::parsePriority("weak") == TargetPriority::Weakest);
    CHECK(CommandParser::parsePriority("close") == TargetPriority::Closest);
}

TEST_CASE("parsePriority rejects an unknown priority") {
    CHECK_THROWS_AS(CommandParser::parsePriority("fastest"), InvalidCommandException);
    CHECK_THROWS_AS(CommandParser::parsePriority("First"), InvalidCommandException);
}

} // TEST_SUITE("CommandParser")
