#include <doctest/doctest.h>

#include <cstdio>
#include <fstream>
#include <stdexcept>
#include <string>

#include "GameException.hpp"
#include "ScoreBoard.hpp"
#include "ScoreEntry.hpp"

namespace {

const char* RANKING_PATH = "build/test_score_board.txt"; ///< Apagado no fim de cada teste.

/// Cria uma entrada do ranking em uma linha só.
ScoreEntry makeEntry(const std::string& name, int score, int wave) {
    ScoreEntry entry;
    entry.name = name;
    entry.score = score;
    entry.wave = wave;
    return entry;
}

/// Garante que cada teste começa e termina sem o arquivo do ranking.
class RankingFileTest {
protected:
    RankingFileTest() {
        std::remove(RANKING_PATH);
    }

    ~RankingFileTest() {
        std::remove(RANKING_PATH);
    }

    /// Escreve o conteúdo no arquivo do ranking.
    void writeRanking(const std::string& content) {
        std::ofstream file(RANKING_PATH);
        file << content;
    }
};

} // namespace

TEST_SUITE("ScoreBoard") {

TEST_CASE_FIXTURE(RankingFileTest, "a missing file is an empty ranking") {
    ScoreBoard board(RANKING_PATH);

    board.load();

    CHECK(board.getEntries().empty());
    CHECK(board.getHighScore() == 0);
    CHECK(board.getIgnoredLines() == 0);
}

TEST_CASE_FIXTURE(RankingFileTest, "load reads the entries from the highest score") {
    writeRanking("300;2;Ana\n4820;8;Thiago Arthur\n");
    ScoreBoard board(RANKING_PATH);

    board.load();

    REQUIRE(board.getEntries().size() == 2);
    CHECK(board.getEntries()[0].name == "Thiago Arthur");
    CHECK(board.getEntries()[0].score == 4820);
    CHECK(board.getEntries()[0].wave == 8);
    CHECK(board.getEntries()[1].name == "Ana");
    CHECK(board.getHighScore() == 4820);
}

TEST_CASE_FIXTURE(RankingFileTest, "corrupted lines are skipped and counted") {
    writeRanking("abc\n100;1;Bia\n;;\n\n50;x;Caio\n-5;1;Duda\n200;2;\n");
    ScoreBoard board(RANKING_PATH);

    board.load();

    REQUIRE(board.getEntries().size() == 1);
    CHECK(board.getEntries()[0].name == "Bia");
    CHECK(board.getIgnoredLines() == 5);
}

TEST_CASE_FIXTURE(RankingFileTest, "lines saved on Windows are read without the '\\r'") {
    writeRanking("100;1;Bia\r\n");
    ScoreBoard board(RANKING_PATH);

    board.load();

    REQUIRE(board.getEntries().size() == 1);
    CHECK(board.getEntries()[0].name == "Bia");
}

TEST_CASE("addEntry keeps only the 5 best scores in order") {
    ScoreBoard board(RANKING_PATH);
    int scores[] = {300, 100, 700, 500, 200, 600, 400};
    for (int i = 0; i < 7; ++i) {
        board.addEntry(makeEntry("jogador", scores[i], 1));
    }

    REQUIRE(board.getEntries().size() == ScoreBoard::MAX_ENTRIES);
    CHECK(board.getEntries()[0].score == 700);
    CHECK(board.getEntries()[4].score == 300);
    CHECK(board.getHighScore() == 700);
}

TEST_CASE("a tie keeps the oldest entry in front") {
    ScoreBoard board(RANKING_PATH);

    board.addEntry(makeEntry("primeiro", 500, 3));
    board.addEntry(makeEntry("segundo", 500, 3));

    CHECK(board.getEntries()[0].name == "primeiro");
    CHECK(board.getEntries()[1].name == "segundo");
}

TEST_CASE("qualifies while there is room or the score beats the lowest") {
    ScoreBoard board(RANKING_PATH);
    CHECK(board.qualifies(0));

    for (int score = 100; score <= 500; score += 100) {
        board.addEntry(makeEntry("jogador", score, 1));
    }

    CHECK_FALSE(board.qualifies(100));
    CHECK(board.qualifies(101));
}

TEST_CASE("addEntry rejects an invalid entry") {
    ScoreBoard board(RANKING_PATH);

    CHECK_THROWS_AS(board.addEntry(makeEntry("", 100, 1)), std::invalid_argument);
    CHECK_THROWS_AS(board.addEntry(makeEntry("a;b", 100, 1)), std::invalid_argument);
    CHECK_THROWS_AS(board.addEntry(makeEntry("Ana", -1, 1)), std::invalid_argument);
    CHECK_THROWS_AS(board.addEntry(makeEntry("Ana", 100, -1)), std::invalid_argument);
    CHECK(board.getEntries().empty());
}

TEST_CASE_FIXTURE(RankingFileTest, "save and load again keep the same ranking") {
    ScoreBoard board(RANKING_PATH);
    board.addEntry(makeEntry("Thiago", 4820, 8));
    board.addEntry(makeEntry("Lucas", 3150, 6));

    board.save();
    ScoreBoard loaded(RANKING_PATH);
    loaded.load();

    REQUIRE(loaded.getEntries().size() == 2);
    CHECK(loaded.getEntries()[0].name == "Thiago");
    CHECK(loaded.getEntries()[1].score == 3150);
    CHECK(loaded.getIgnoredLines() == 0);
}

TEST_CASE("save throws GameException when the file cannot be written") {
    ScoreBoard board("build/pasta_que_nao_existe/ranking.txt");
    board.addEntry(makeEntry("Ana", 100, 1));

    CHECK_THROWS_AS(board.save(), GameException);
}

} // TEST_SUITE("ScoreBoard")
