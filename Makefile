all:
	g++ -framework OpenGL -lGLEW -lSDL2main -lSDL2 *.cpp -o hello 
