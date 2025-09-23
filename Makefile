# Makefile for SimpleLang Compiler

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g
LDFLAGS = 

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
TEST_DIR = tests

# Source files
LEXER_SOURCES = $(wildcard $(SRC_DIR)/lexer/*.cpp)
PARSER_SOURCES = $(wildcard $(SRC_DIR)/parser/*.cpp)
AST_SOURCES = $(wildcard $(SRC_DIR)/ast/*.cpp)
SEMANTIC_SOURCES = $(wildcard $(SRC_DIR)/semantic/*.cpp)
CODEGEN_SOURCES = $(wildcard $(SRC_DIR)/codegen/*.cpp)
RUNTIME_SOURCES = $(wildcard $(SRC_DIR)/runtime/*.cpp)
UTILS_SOURCES = $(wildcard $(SRC_DIR)/utils/*.cpp)
MAIN_SOURCE = $(SRC_DIR)/main.cpp

ALL_SOURCES = $(LEXER_SOURCES) $(PARSER_SOURCES) $(AST_SOURCES) $(SEMANTIC_SOURCES) $(CODEGEN_SOURCES) $(RUNTIME_SOURCES) $(UTILS_SOURCES) $(MAIN_SOURCE)

# Object files
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(ALL_SOURCES))

# Test sources
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJECTS = $(patsubst $(TEST_DIR)/%.cpp,$(BUILD_DIR)/test/%.o,$(TEST_SOURCES))

# Target executable
TARGET = $(BIN_DIR)/compiler
TEST_TARGET = $(BIN_DIR)/test_runner

# Default target
all: $(TARGET)

# Create directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/lexer
	mkdir -p $(BUILD_DIR)/parser
	mkdir -p $(BUILD_DIR)/ast
	mkdir -p $(BUILD_DIR)/semantic
	mkdir -p $(BUILD_DIR)/codegen
	mkdir -p $(BUILD_DIR)/runtime
	mkdir -p $(BUILD_DIR)/utils
	mkdir -p $(BUILD_DIR)/test

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -c $< -o $@

# Compile test files
$(BUILD_DIR)/test/%.o: $(TEST_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -c $< -o $@

# Link main executable
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

# Link test executable
$(TEST_TARGET): $(filter-out $(BUILD_DIR)/main.o,$(OBJECTS)) $(TEST_OBJECTS) | $(BIN_DIR)
	$(CXX) $(filter-out $(BUILD_DIR)/main.o,$(OBJECTS)) $(TEST_OBJECTS) $(LDFLAGS) -o $@

# Build target
build: $(TARGET)
	@echo "Build complete: $(TARGET)"

# Test target
test: $(TEST_TARGET)
	@echo "Running tests..."
	./$(TEST_TARGET)
	@echo "Running integration tests..."
	@cd tests && bash integration_tests.sh

# Clean target
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Install target (optional)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Debug build
debug: CXXFLAGS += -DDEBUG -g3
debug: $(TARGET)

# Release build
release: CXXFLAGS += -DNDEBUG -O3
release: clean $(TARGET)

# Run with example
run-example: $(TARGET)
	./$(TARGET) examples/factorial.sl -o examples/factorial
	./examples/factorial

# Format code
format:
	find $(SRC_DIR) -name "*.cpp" -o -name "*.h" | xargs clang-format -i
	find $(TEST_DIR) -name "*.cpp" -o -name "*.h" | xargs clang-format -i

# Static analysis
analyze:
	cppcheck --enable=all --std=c++17 $(SRC_DIR)

# Help
help:
	@echo "Available targets:"
	@echo "  all       - Build the compiler (default)"
	@echo "  build     - Build the compiler"
	@echo "  test      - Build and run tests"
	@echo "  clean     - Remove build files"
	@echo "  debug     - Build with debug symbols"
	@echo "  release   - Build optimized release version"
	@echo "  install   - Install to /usr/local/bin"
	@echo "  format    - Format source code"
	@echo "  analyze   - Run static analysis"
	@echo "  help      - Show this help"

.PHONY: all build test clean debug release install format analyze help run-example