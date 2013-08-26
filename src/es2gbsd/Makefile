
SYSTEM ?= $(shell uname | tr '[:upper:]' '[:lower:]')

all: es2gbsd

es2gbsd: main.cpp
	g++ -o es2gbsd main.cpp -I../../regal/include -L../../regal/lib/$(SYSTEM) -lRegal -lRegalGLU -lRegalGLUT -lX11

clean:
	rm es2gbsd

