cpaste: src/main.cpp
	gcc -o bin/emu src/main.cpp  `pkg-config --cflags --libs sdl2` -lstdc++
