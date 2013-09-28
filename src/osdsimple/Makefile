
SYSTEM ?= $(shell uname | tr '[:upper:]' '[:lower:]')

all: osdsimple

osdsimple: main.cpp
	g++ -o osdsimple main.cpp -I../../regal/include -I../../r3/code -L../../regal/lib/$(SYSTEM) -lRegal -lRegalGLU -lRegalGLUT -lX11

clean:
	rm osdsimple

