# Ferramentas e opções de compilação
CXX ?= g++
CPPFLAGS := -Iinclude -Itests/third_party
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Werror -g
LDFLAGS :=

BUILD_DIR := build
TEST_RUNNER := $(BUILD_DIR)/tests
TEST_SOURCES := $(wildcard tests/test_*.cpp)
SOURCE_FILES := $(filter-out src/main.cpp,$(wildcard src/*.cpp))

ifeq ($(OS),Windows_NT)
REMOVE_BUILD_DIR := cmd /C "if exist $(BUILD_DIR) rmdir /S /Q $(BUILD_DIR)"
REMOVE_COVERAGE_DATA := cmd /C "if exist tests\*.gcda del /Q tests\*.gcda & if exist tests\*.gcno del /Q tests\*.gcno & if exist src\*.gcda del /Q src\*.gcda & if exist src\*.gcno del /Q src\*.gcno"
else
REMOVE_BUILD_DIR := $(RM) -r $(BUILD_DIR)
REMOVE_COVERAGE_DATA := $(RM) tests/*.gcda tests/*.gcno src/*.gcda src/*.gcno
endif

.PHONY: test test-map coverage clean help

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TEST_RUNNER): $(TEST_SOURCES) $(SOURCE_FILES) | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(TEST_SOURCES) $(SOURCE_FILES) $(LDFLAGS) -o $@

test: $(TEST_RUNNER)
	./$(TEST_RUNNER)

test-map: test

coverage: CXXFLAGS += --coverage -O0
coverage: LDFLAGS += --coverage
coverage: clean test
	gcovr --root . --filter src --exclude src/main.cpp --fail-under-line 60

clean:
	$(REMOVE_BUILD_DIR)
	$(REMOVE_COVERAGE_DATA)

help:
	@echo "make test       Build and run all unit tests"
	@echo "make test-map   Compatibility alias for make test"
	@echo "make coverage   Run all tests and require at least 60% line coverage"
	@echo "make clean      Remove generated test and coverage files"
