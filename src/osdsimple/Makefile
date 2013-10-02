
SYSTEM ?= $(shell uname | tr '[:upper:]' '[:lower:]')

all: osdsimple

INC = -I../../regal/include -I../../r3/code -I../../osd/opensubdiv
LIB = -L../../regal/lib/$(SYSTEM) -lRegal -lRegalGLU -lRegalGLUT -lX11

SRC = $(wildcard *.cpp)

OSDSRC  = ../../osd/cpuComputeContext.cpp
OSDSRC += ../../osd/cpuComputeController.cpp
OSDSRC += ../../osd/cpuEvalLimitContext.cpp
OSDSRC += ../../osd/cpuEvalLimitController.cpp
OSDSRC += ../../osd/cpuEvalLimitKernel.cpp
OSDSRC += ../../osd/cpuEvalStencilsContext.cpp
OSDSRC += ../../osd/cpuEvalStencilsController.cpp
OSDSRC += ../../osd/cpuKernel.cpp
OSDSRC += ../../osd/cpuVertexBuffer.cpp
OSDSRC += ../../osd/drawContext.cpp
OSDSRC += ../../osd/drawRegistry.cpp
OSDSRC += ../../osd/error.cpp
OSDSRC += ../../osd/evalLimitContext.cpp
OSDSRC += ../../osd/gcdComputeController.cpp
OSDSRC += ../../osd/gcdKernel.cpp
OSDSRC += ../../osd/glDrawContext.cpp

SRC += $(OSDSRC)


OBJ = $(patsubst %.cpp,%.o, $(SRC))

.cpp.o:
	g++ -c $(INC) $?

osdsimple: $(OBJ)
	g++ -o osdsimple $(OBJ) -I../../regal/include -I../../r3/code -L../../regal/lib/$(SYSTEM) -lRegal -lRegalGLU -lRegalGLUT -lX11

clean:
	rm osdsimple
	rm $(OBJ)

