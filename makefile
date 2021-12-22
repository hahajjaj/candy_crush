FLAGS=-Wall -pedantic

all: candy

candy: candy3.cpp cpp_files/cell.hpp
	g++ candy3.cpp -o candy3 -lfltk -lfltk_images
clean:
	rm -f candy3