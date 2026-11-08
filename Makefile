# ROOT DEFENSE - GNU Make + g++ no Linux, C++11
CXX := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -pedantic -g
CPPFLAGS := -Iinclude
TEST_CPPFLAGS := $(CPPFLAGS) -Itests/third_party

SRC_DIR := src
TEST_DIR := tests
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin

TARGET := $(BIN_DIR)/root_defense
TEST_TARGET := $(BIN_DIR)/tests

SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
LIB_SOURCES := $(filter-out $(SRC_DIR)/main.cpp,$(SOURCES))
TEST_SOURCES := $(wildcard $(TEST_DIR)/*.cpp)

OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/src/%.o,$(SOURCES))
LIB_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/src/%.o,$(LIB_SOURCES))
TEST_OBJECTS := $(patsubst $(TEST_DIR)/%.cpp,$(OBJ_DIR)/tests/%.o,$(TEST_SOURCES))

.PHONY: all run test coverage docs clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_TARGET): $(LIB_OBJECTS) $(TEST_OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ_DIR)/src/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR)/tests/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(TEST_CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

coverage: clean
	$(MAKE) test CXXFLAGS="$(CXXFLAGS) --coverage -O0"
	gcovr -r . --filter src/ --exclude src/main.cpp --print-summary --fail-under-line 60

docs:
	doxygen Doxyfile

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(BUILD_DIR)/docs

-include $(OBJECTS:.o=.d) $(TEST_OBJECTS:.o=.d)
