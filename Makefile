all:
	g++ -I include -L lib -o Game Main.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_mixer
	./Game