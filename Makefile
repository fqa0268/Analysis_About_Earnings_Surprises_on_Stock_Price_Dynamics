CXX := g++
CXXFLAGS := -Wall -g -std=c++14 -Iinclude
LDFLAGS := -pthread
LIBS := -lcurl
GTEST_DIR := ./googletest
GTEST_LIB_DIR := $(GTEST_DIR)/build/lib
GTEST_INCLUDE := -isystem $(GTEST_DIR)/include

# Define paths
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
INCLUDE_DIR := include
TESTS_DIR := tests

# Find all cpp files in src and test directories
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
TEST_SRCS := $(wildcard $(TESTS_DIR)/*.cpp)

# Define object files for main application and test
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
TEST_OBJS := $(patsubst $(TESTS_DIR)/%.cpp, $(OBJ_DIR)/%_test.o, $(TEST_SRCS))

# Name of your main application and test executable
MAIN_EXEC := $(BIN_DIR)/main
TEST_EXEC := $(BIN_DIR)/test_suite

# Default target
all: $(MAIN_EXEC)

# Target for main application
$(MAIN_EXEC): $(OBJS)
	$(CXX) $(LDFLAGS) $^ $(LIBS) -o $@

# Pattern rule for object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Target for tests
test: $(TEST_EXEC)

$(TEST_EXEC): $(TEST_OBJS) $(filter-out $(OBJ_DIR)/main.o, $(OBJS))
	$(CXX) $(LDFLAGS) $^ -L$(GTEST_LIB_DIR) -lgtest -lgtest_main $(LIBS) -o $@

$(OBJ_DIR)/%_test.o: $(TESTS_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(GTEST_INCLUDE) -c $< -o $@

# Clean target
clean:
	rm -f $(OBJ_DIR)/*.o $(BIN_DIR)/* $(TESTS_DIR)/*_test

.PHONY: all clean test

