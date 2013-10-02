
SYSTEM ?= $(shell uname | tr '[:upper:]' '[:lower:]')

all: osdsimple

INC = -I../../regal/include -I../../r3/code -I../../osd/opensubdiv
LIB = -L../../regal/lib/$(SYSTEM) -lRegal -lRegalGLU -lRegalGLUT -lX11

SRC = $(wildcard *.cpp)

OBJ = $(patsubst %.cpp,%.o, $(SRC))

.cpp.o:
	g++ -c $(INC) $?

osdsimple: $(OBJ)
	g++ -o osdsimple $(OBJ) -I../../regal/include -I../../r3/code -L../../regal/lib/$(SYSTEM) -lRegal -lRegalGLU -lRegalGLUT -lX11

clean:
	rm osdsimple
	rm $(OBJ)

