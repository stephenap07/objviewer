all:
	g++ -framework OpenGL -lGLEW -lSDL2main -lSDL2 -lSDL2_image *.cpp -o hello 
run:
	./hello
