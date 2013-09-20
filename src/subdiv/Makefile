
SYSTEM ?= $(shell uname | tr '[:upper:]' '[:lower:]')

all: subdiv

subdiv: main.cpp
	g++ -o subdiv main.cpp -I../../regal/include -I../../r3/code -L../../regal/lib/$(SYSTEM) -lRegal -lRegalGLU -lRegalGLUT -lX11

clean:
	rm subdiv

