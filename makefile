# Compiler Flags
CC = gcc
CFLAGS = -Wall -ansi -pedantic -std=c89 -Iinclude -g

# Directories
OBJ_DIR = obj
SRC_DIR = src
OUT_DIR = output
INCLUDE_DIR = include
DEP_FILE = dependencies.d  # Dependency file

# Find all source files and corresponding object files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

# The target executable
assembler: $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $@

# Pattern rule for creating .o files from .c files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Include the dependency file
-include $(DEP_FILE)

# Generate dependencies for each source file
$(DEP_FILE): $(SRC_DIR)/*.c
	$(CC) $(CFLAGS) -MM $^ > $(DEP_FILE)

# Clean up build files
clean:
	rm -rf $(OBJ_DIR)/*.o $(OUT_DIR)/*.am $(OUT_DIR)/*.ob $(OUT_DIR)/*.ent $(OUT_DIR)/*.ext assembler $(DEP_FILE)
