CC=g++
FLAGS=-std=c++11 -g -Wall -Wextra -Werror
LFLAGS=-lcrypto

CPP=$(wildcard problem*.cpp)
BIN=$(CPP:problem%.cpp=p%)

ifdef ASAN
  FLAGS += -fsanitize=address
  LFLAGS += -fsanitize=address
endif

all: $(BIN)

p% : problem%.cpp bytevector.h bytevector.cpp
	@ echo "Compiling $@"
	@ $(CC) $(FLAGS) bytevector.cpp $< $(LFLAGS) -o $@

clean :
	rm -f $(BIN)
