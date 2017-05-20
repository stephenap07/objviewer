all:
	g++ -std=c++1z -I/usr/local/include -framework OpenGL -L/usr/local/lib -lGLEW -lSDL2main -lSDL2 *.cpp -o hello 
run:
	./hello
