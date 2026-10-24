.PHONY: all clean

CC := gcc
CFLAGS := -Wall -Wextra -Iinclude 
LDLIBS := -lgdi32

SRC_DIR := src
BUILD_DIR := build
TARGET := $(BUILD_DIR)/screen.exe

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

all: $(TARGET)

$(TARGET) : $(OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $a $(LDLIBS)

$(BUILD_DIR)/%.o : $(SRC_DIR)%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR): 
	mkdir -p $(BUILD_DIR)

clean: 
	rm -rf $(BUILD_DIR)
