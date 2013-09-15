
SYSTEM ?= $(shell uname | tr '[:upper:]' '[:lower:]')

all: tmpl

tmpl: main.cpp
	g++ -o tmpl main.cpp -I../../regal/include -L../../regal/lib/$(SYSTEM) -lRegal -lRegalGLU -lRegalGLUT -lX11

clean:
	rm tmpl

