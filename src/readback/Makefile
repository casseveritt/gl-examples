all: readback

readback: main.cpp
	g++ -o readback main.cpp -I../../regal/include -L../../regal/lib/linux -lRegal -lRegalGLU -lRegalGLUT -lX11

clean:
	rm readback
