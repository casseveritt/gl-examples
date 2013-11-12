
SYSTEM ?= $(shell uname | tr '[:upper:]' '[:lower:]')

all: proginstance

proginstance: main.cpp
	g++ -o proginstance main.cpp -I../../regal/include -L../../regal/lib/$(SYSTEM) -lRegal -lRegalGLU -lRegalGLUT -lX11

clean:
	rm proginstance

