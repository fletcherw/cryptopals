CC=g++
FLAGS=-std=c++11 -g -Wall -Wextra -Werror
LFLAGS=-lcrypto

CPP=$(wildcard problem*.cpp)
BIN=$(CPP:problem%.cpp=p%.out)

all: $(BIN)

p%.out : problem%.cpp
	@ echo "Compiling $@"
	@ $(CC) $(FLAGS) bytevector.cpp $< $(LFLAGS) -o $@

clean :
	rm $(BIN) 
