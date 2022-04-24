PROJECT_NAME := kcalc

INC_DIR   := ./include
SRC_DIR   := ./src
BUILD_DIR := ./build
OBJ_DIR	  := $(BUILD_DIR)/obj
BIN_DIR	  := $(BUILD_DIR)/bin

INC := $(wildcard $(INC_DIR)/*.h)
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))
DEP := $(patsubst %.o,%.d,$(OBJ))

CC       := gcc
CFLAGS 	 := -Wall -Wextra -std=c11
LDFLAGS  :=

TARGET := $(BIN_DIR)/kcalc

.PHONY: all
all: $(TARGET)
	@$(info [$(PROJECT_NAME)] build done)

$(TARGET): $(OBJ)
	$(info [$(PROJECT_NAME)] linking $(notdir $@))
	@$(CC) $(LDFLAGS) -o $@ $^

$(OBJ): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(info [$(PROJECT_NAME)] compiling $(notdir $<) -> $(notdir $@))
	@$(CC) -MMD -I$(INC_DIR) $(CFLAGS) -c -o $@ $<

.PHONY: debug
debug: CFLAGS += -DDEBUG -g
debug: all

.PHONY: build
build:
	@[ -d $(OBJ_DIR) ] || mkdir -p $(OBJ_DIR)
	@[ -d $(BIN_DIR) ] || mkdir -p $(BIN_DIR)

.PHONY: run
run:
	@$(TARGET)

.PHONY: clean
clean:
	$(info [$(PROJECT_NAME)] $@)
	@$(RM) $(TARGET) $(OBJ) $(DEP)

-include $(DEP)