CC=g++

all:
	
	$(CC) sokoban.cpp -o sokoban.exe -Wall `sdl2-config --libs` -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

clean:
	rm -rf sokoban.exe
