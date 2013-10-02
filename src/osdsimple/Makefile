
SYSTEM ?= $(shell uname | tr '[:upper:]' '[:lower:]')

all: osdsimple

INC = -I../../regal/include -I../../r3/code -I../../osd/opensubdiv
LIB = -L../../regal/lib/$(SYSTEM) -lRegal -lRegalGLU -lRegalGLUT -lX11

SRC = $(wildcard *.cpp)

OSDSRC  = ../../osd/opensubdiv/osd/cpuComputeContext.cpp
OSDSRC += ../../osd/opensubdiv/osd/cpuComputeController.cpp
OSDSRC += ../../osd/opensubdiv/osd/cpuEvalLimitContext.cpp
OSDSRC += ../../osd/opensubdiv/osd/cpuEvalLimitController.cpp
OSDSRC += ../../osd/opensubdiv/osd/cpuEvalLimitKernel.cpp
OSDSRC += ../../osd/opensubdiv/osd/cpuEvalStencilsContext.cpp
OSDSRC += ../../osd/opensubdiv/osd/cpuEvalStencilsController.cpp
OSDSRC += ../../osd/opensubdiv/osd/cpuGLVertexBuffer.cpp
OSDSRC += ../../osd/opensubdiv/osd/cpuKernel.cpp
OSDSRC += ../../osd/opensubdiv/osd/cpuVertexBuffer.cpp
OSDSRC += ../../osd/opensubdiv/osd/drawContext.cpp
OSDSRC += ../../osd/opensubdiv/osd/drawRegistry.cpp
OSDSRC += ../../osd/opensubdiv/osd/error.cpp
OSDSRC += ../../osd/opensubdiv/osd/evalLimitContext.cpp
OSDSRC += ../../osd/opensubdiv/osd/glDrawContext.cpp

SRC += $(OSDSRC)


OBJ = $(patsubst %.cpp,%.o, $(SRC))

.cpp.o:
	g++ -c $(INC) $? -o $(patsubst %.cpp,%.o,$?)

osdsimple: $(OBJ)
	g++ -o osdsimple $(OBJ) -I../../regal/include -I../../r3/code -L../../regal/lib/$(SYSTEM) -lRegal -lRegalGLU -lRegalGLUT -lX11

clean:
	rm osdsimple
	rm $(OBJ)

