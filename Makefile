all: compile
	clang++-6.0 -o bin/RobotsWar -s  -lfparser -lsfml-network -lsfml-graphics -lsfml-window -lsfml-system -std=c++17 *.o -lstdc++fs
	rm -f *.o
	
compile:
	clang++-6.0 -Wall -std=c++1z -std=c++14 -fexceptions -std=c++17 -O2 -c *.cpp
