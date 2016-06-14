INCLUDES := -I.

length : length.o curry.o
	g++ $^ -std=c++11 -o length

length.o : length.cpp curry.hpp
	g++ -c $< -std=c++11 $(INCLUDES)
curry.o : curry.cpp curry.hpp
	g++ -c $< -std=c++11 $(INCLUDES)


