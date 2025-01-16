# Compiler and flags
CXX_COMPILER = g++
COMPILER_FLAGS = -Wall -Wextra -pedantic -std=c++17

# Directories
SOURCE_DIRECTORY = src
AST_NODES_SOURCE_DIRECTORY = src/AbstractSyntaxTreeExpressionNodes

# Flex
FLEX_TOOL = flex
FLEX_SOURCE = $(SOURCE_DIRECTORY)/lexer.l
FLEX_OUTPUT_C = lex.yy.c

# Bison
BISON_TOOL = bison
BISON_SOURCE = $(SOURCE_DIRECTORY)/parser.y
BISON_OUTPUT_C = parser.tab.c
BISON_OUTPUT_H = parser.tab.h

# AST
AST_SOURCE = $(SOURCE_DIRECTORY)/AbstractSyntaxTree.cpp
AST_NODE_SOURCES = $(wildcard $(AST_NODES_SOURCE_DIRECTORY)/*.cpp)

# Main
MAIN_SOURCE = $(SOURCE_DIRECTORY)/main.cpp

# Output
TARGET_EXECUTABLE = compiler

# Default target
all: $(TARGET_EXECUTABLE)

# Compile the target
$(TARGET_EXECUTABLE): $(BISON_OUTPUT_C) $(FLEX_OUTPUT_C)
	$(CXX_COMPILER) $(COMPILER_FLAGS) -I $(SOURCE_DIRECTORY) $(BISON_OUTPUT_C) $(FLEX_OUTPUT_C) $(AST_SOURCE) $(AST_NODE_SOURCES) $(MAIN_SOURCE) -o $(TARGET_EXECUTABLE)

# Generate parser
$(BISON_OUTPUT_C) $(BISON_OUTPUT_H): $(BISON_SOURCE)
	$(BISON_TOOL) -d -o $(BISON_OUTPUT_C) $(BISON_SOURCE)

# Generate lexer
$(FLEX_OUTPUT_C): $(FLEX_SOURCE)
	$(FLEX_TOOL) -o $(FLEX_OUTPUT_C) $(FLEX_SOURCE)

# Clean up
clean:
	rm -f $(TARGET_EXECUTABLE) $(FLEX_OUTPUT_C) $(BISON_OUTPUT_C) $(BISON_OUTPUT_H)

.PHONY: all clean

