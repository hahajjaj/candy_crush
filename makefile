FLAGS=-Wall -pedantic

all: candy

candy: candy.cpp
	g++ candy.cpp -o candy -lfltk -lfltk_images
clean:
	rm -f candy