# Directories
OBJ_DIR      := obj
INC_DIR      := include
SRC_DIR      := src
SRC_SUB_DIRS := $(shell find src -mindepth 1 -maxdepth 1 -type d -exec basename {} \;)

# Source files, header files, object files
MAIN_SRC  := $(SRC_DIR)/ncc.cpp
SRCS      := $(foreach dir, $(SRC_SUB_DIRS), $(wildcard $(SRC_DIR)/$(dir)/*.cpp))

# Object files
OBJS      := $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(SRCS)))
MAIN_OBJ  := $(OBJ_DIR)/$(notdir $(MAIN_SRC:.cpp=.o))

# Compiler, flags, libraries
CXX      := g++
CXXFLAGS := -I$(INC_DIR) -std=c++20 -Wall -g # -Werror
LIBS     :=

TARGET_EXEC := ncc


# Rules

all: $(OBJ_DIR) $(TARGET_EXEC)

$(TARGET_EXEC): $(OBJS) $(MAIN_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/*/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(MAIN_OBJ): $(MAIN_SRC)
	$(CXX) $(CXXFLAGS) -c $(MAIN_SRC) -o $(MAIN_OBJ)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(TARGET_EXEC)

# Phony targets
.PHONY: all clean