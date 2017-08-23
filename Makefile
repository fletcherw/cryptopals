CXX     = g++
FLAGS   = -std=c++11 -g -Wall -Wextra -Werror
LFLAGS  = -lssl -lcrypto

BINCPP  = $(wildcard src/problem*.cpp)
BIN     = $(BINCPP:src/problem%.cpp=p%) 

LIBCPP  = src/bytevector.cpp \
          src/cookie.cpp \
          src/MT19937.cpp \
          src/Crypto.cpp

LIBOBJ  = $(LIBCPP:src/%.cpp=build/%.o) 
LIBHEAD = $(LIBCPP:%.cpp=%.h) 

CPP     = $(BINCPP) $(LIBCPP)
OBJ     = $(CPP:src/%.cpp=build/%.o)

ifdef ASAN
  FLAGS += -fsanitize=address
  LFLAGS += -fsanitize=address
endif

all: $(BIN)

p% : build/problem%.o $(LIBOBJ) $(LIBHEAD)
	@ echo "Linking $@"
	@ $(CXX) $(FLAGS) $< $(LIBOBJ) -o $@ $(LFLAGS)

build/%.o: src/%.cpp
	@ mkdir -p $(@D)
	@ echo "Compiling $@"
	@ $(CXX) $(FLAGS) -c $< -o $@

clean:
	@ echo "Removing build files"
	@ rm -f $(BIN)
	@ rm -rf build

.PRECIOUS: $(OBJ)
