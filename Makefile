CC   = clang++
SRC  = $(wildcard *.cpp) $(wildcard src/*/*.cpp)
OBJ  = $(addprefix obj/,$(notdir $(SRC:.cpp=.o)))  
DEPS = $(SRC:%.cpp=obj/%.d)

LDFLAGS = -L/usr/local/lib -lGLEW -lSDL2main -lSDL2 
CFLAGS  = -std=c++1y -Wall -fPIC -O3

EXE = objviewer

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
	LDFLAGS += -lGL
endif
ifeq ($(UNAME), Darwin)
	LDFLAGS += -framework OpenGL
	CFLAGS += -I/usr/local/include
endif

.PHONY: all clean run

$(EXE): $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) $(LDFLAGS) -o $@

obj/%.o: %.cpp | obj
	$(CC) -MMD -MP -c $< $(CFLAGS) -o $@ 

obj:
	mkdir obj

run:
	./$(EXE)

clean:
	rm -f $(EXE) $(OBJ) $(DEPS)

-include $(DEPS)
