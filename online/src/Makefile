SRCS:=$(wildcard *.cc)
OBJS:=$(patsubst %.cc,%.o,$(SRCS))
ELF:=a.out
CC:=g++
CXXFLAGS:=-std=c++11 -g -Wall
$(ELF):$(OBJS)
	g++ $^ -o $@ -lpthread -llog4cpp $(CXXFLAGS)
.PHONY:clean
clean:
	rm -rf $(ELF) $(OBJS)
