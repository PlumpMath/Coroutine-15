all:
	g++ -std=c++11 -o test main.cpp Coroutine.hpp
clean:
	rm -rf test
