# Ferramentas e opções de compilação
CXX ?= g++
CPPFLAGS := -Iinclude -Itests/third_party
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Werror -g
LDFLAGS :=

BUILD_DIR := build
MAP_TEST_RUNNER := $(BUILD_DIR)/map_tests
MAP_TEST_SOURCES := tests/test_main.cpp tests/test_map.cpp
MAP_SOURCE := $(wildcard src/Map.cpp)

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

$(MAP_TEST_RUNNER): $(MAP_TEST_SOURCES) $(MAP_SOURCE) | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(MAP_TEST_SOURCES) $(MAP_SOURCE) $(LDFLAGS) -o $@

test: test-map

test-map: $(MAP_TEST_RUNNER)
	./$(MAP_TEST_RUNNER)

coverage: CXXFLAGS += --coverage -O0
coverage: LDFLAGS += --coverage
coverage: clean test-map
	gcovr --root . --filter src/Map.cpp --fail-under-line 60

clean:
	$(REMOVE_BUILD_DIR)
	$(REMOVE_COVERAGE_DATA)

help:
	@echo "make test-map   Build and run Map unit tests"
	@echo "make test       Alias for test-map"
	@echo "make coverage   Run Map tests and require at least 60% line coverage"
	@echo "make clean      Remove the Map test executable"
