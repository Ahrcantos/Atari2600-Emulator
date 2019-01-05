cpaste: src/main.cpp src/cpu.cpp
	gcc -o bin/emu src/main.cpp src/cpu.cpp  `pkg-config --cflags --libs sdl2` -lstdc++
