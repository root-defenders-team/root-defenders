CXX ?= g++
CPPFLAGS := -Iinclude -Itests/third_party
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Werror -g
LDFLAGS :=
SFML_LIBS := -lsfml-graphics -lsfml-window -lsfml-system
LDLIBS := $(SFML_LIBS)

BUILD_DIR := build
TEST_RUNNER := $(BUILD_DIR)/tests
TEST_SOURCES := $(wildcard tests/test_*.cpp)
SOURCE_FILES := $(filter-out src/main.cpp,$(wildcard src/*.cpp))

GAME_MANAGER_RUNNER := $(BUILD_DIR)/test_game_manager
GAME_MANAGER_TEST_SOURCES := tests/test_main.cpp tests/test_game_manager.cpp

ENTITIES_RUNNER := $(BUILD_DIR)/test_entities
ENTITIES_TEST_SOURCES := tests/test_main.cpp tests/test_enemy.cpp tests/test_tower.cpp

MAP_LOADER_RUNNER := $(BUILD_DIR)/test_map_loader
MAP_LOADER_TEST_SOURCES := tests/test_main.cpp tests/test_map.cpp tests/test_map_loader.cpp
MAP_LOADER_SOURCES := src/Vector2.cpp src/Cell.cpp src/Map.cpp src/MapLoader.cpp

.PHONY: test test-map test-game-manager test-entities test-map-loader coverage coverage-game-manager coverage-entities clean help

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TEST_RUNNER): $(TEST_SOURCES) $(SOURCE_FILES) | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(TEST_SOURCES) $(SOURCE_FILES) $(LDFLAGS) $(LDLIBS) -o $@

$(GAME_MANAGER_RUNNER): $(GAME_MANAGER_TEST_SOURCES) $(SOURCE_FILES) | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(GAME_MANAGER_TEST_SOURCES) $(SOURCE_FILES) $(LDFLAGS) $(LDLIBS) -o $@

$(ENTITIES_RUNNER): $(ENTITIES_TEST_SOURCES) $(SOURCE_FILES) | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(ENTITIES_TEST_SOURCES) $(SOURCE_FILES) $(LDFLAGS) $(LDLIBS) -o $@

$(MAP_LOADER_RUNNER): $(MAP_LOADER_TEST_SOURCES) $(MAP_LOADER_SOURCES) | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(MAP_LOADER_TEST_SOURCES) $(MAP_LOADER_SOURCES) $(LDFLAGS) -o $@

test: $(TEST_RUNNER)
	./$(TEST_RUNNER)

test-map: test

test-game-manager: $(GAME_MANAGER_RUNNER)
	./$(GAME_MANAGER_RUNNER) --test-suite=GameManager

test-entities: $(ENTITIES_RUNNER)
	./$(ENTITIES_RUNNER)

test-map-loader: $(MAP_LOADER_RUNNER)
	./$(MAP_LOADER_RUNNER)

coverage: CXXFLAGS += --coverage -O0
coverage: LDFLAGS += --coverage
coverage: clean test
	gcovr --root . --filter src --exclude src/main.cpp --fail-under-line 60

coverage-game-manager: CXXFLAGS += --coverage -O0
coverage-game-manager: LDFLAGS += --coverage
coverage-game-manager: clean $(GAME_MANAGER_RUNNER)
	-./$(GAME_MANAGER_RUNNER) --test-suite=GameManager
	gcovr --root . --filter src/GameManager.cpp --fail-under-line 60

coverage-entities: CXXFLAGS += --coverage -O0
coverage-entities: LDFLAGS += --coverage
coverage-entities: clean $(ENTITIES_RUNNER)
	-./$(ENTITIES_RUNNER)
	gcovr --root . --filter src/Enemy.cpp --filter src/Tower.cpp --fail-under-line 60

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) *.gcda *.gcno tests/*.gcda tests/*.gcno src/*.gcda src/*.gcno

help:
	@echo "make test                  Build and run all unit tests"
	@echo "make test-map              Compatibility alias for make test"
	@echo "make test-game-manager     Build and run the GameManager unit tests"
	@echo "make test-entities         Build and run the Tower and Enemy unit tests"
	@echo "make test-map-loader       Build and run the Map and MapLoader unit tests"
	@echo "make coverage              Run all tests and require at least 60% line coverage"
	@echo "make coverage-game-manager Run the GameManager tests and require at least 60% line coverage of GameManager.cpp"
	@echo "make coverage-entities     Run the entities tests and require at least 60% line coverage of Tower and Enemy"
	@echo "make clean                 Remove generated test and coverage files"
