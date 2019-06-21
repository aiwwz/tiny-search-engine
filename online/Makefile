INC_DIR := ./include ./include/net ./include/base ./include/cppjieba
SRC_DIR := ./src ./src/net ./src/base
EXE_DIR := ./bin

CC := g++
CPPFLAGS := -std=c++11 -g -Wall
LIBS := -ljson -lpthread
INC_FILE := $(addprefix -l, $(INC_DIR))
CPP_FILE := $(wildcard ./src/*.cc) $(wildcard ./src/base/*.cc) $(wildcard ./src/net/*.cc)
TARGET := tinyse

$(TARGET) : $(CPP_FILE)
			$(CC) $(CPPFLAGS) -o $@ $(CPP_FILE) $(INC_FILE) $(LIBS)

clean: rm -rf $(TARGET)
