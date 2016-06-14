length : length.o
	g++ length.o -std=c++11 -o length

length.o : length.cpp curry.hpp
	g++ -c length.cpp -std=c++11

