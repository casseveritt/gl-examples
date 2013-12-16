
SYSTEM ?= $(shell uname | tr '[:upper:]' '[:lower:]')

all: es2_read_depth

es2_read_depth: main.cpp
	g++ -o es2_read_depth main.cpp -I../../regal/include -L../../regal/lib/$(SYSTEM) -lRegal -lRegalGLU -lRegalGLUT -lX11

clean:
	rm es2_read_depth

