CXX    = g++
FLAGS  = -std=c++11 -g -Wall -Wextra -Werror
LFLAGS = -lssl -lcrypto

BINCPP = $(wildcard problem*.cpp)
BIN    = $(BINCPP:problem%.cpp=p%) 

LIBCPP = bytevector.cpp cookie.cpp
LIBOBJ = $(LIBCPP:%.cpp=build/%.o) 

CPP    = $(BINCPP) $(LIBCPP)
OBJ    = $(CPP:%.cpp=build/%.o)

ifdef ASAN
  FLAGS += -fsanitize=address
  LFLAGS += -fsanitize=address
endif

all: $(BIN)

p% : build/problem%.o $(LIBOBJ)
	@ echo "Linking $@"
	@ $(CXX) $(FLAGS)  $^ -o $@ $(LFLAGS)

build/%.o: %.cpp
	@ mkdir -p $(@D)
	@ echo "Compiling $@"
	@ $(CXX) $(FLAGS) -c $< -o $@

clean :
	@ echo "Removing build files"
	@ rm -f $(BIN)
	@ rm -rf build

.PRECIOUS: $(OBJ)
