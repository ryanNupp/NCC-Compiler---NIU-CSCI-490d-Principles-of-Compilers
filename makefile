INC_DIR   := ./include
SRC_DIR   := ./src
OBJ_DIR   := ./obj

SRCS := $(wildcard $(SRC_DIR)/*.cc)
OBJS := $(patsubst $(SRC_DIR)/%.cc, $(OBJ_DIR)/%.o , $(SRCS))

TARGET_EXEC := ncc

CC     := g++
CFLAGS := -I$(INC_DIR) -std=c++20 -Wall -Werror -g
LIBS   := 


# Rules

$(TARGET_EXEC): $(OBJ_DIR) $(OBJS) 
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

clean:
	rm -r $(OBJ_DIR) $(TARGET_EXEC)