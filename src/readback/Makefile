
SYSTEM ?= $(shell uname | tr '[:upper:]' '[:lower:]')

all: readback

readback: main.cpp
	g++ -o readback main.cpp -I../../regal/include -L../../regal/lib/$(SYSTEM) -lRegal -lRegalGLU -lRegalGLUT -lX11

clean:
	rm readback
