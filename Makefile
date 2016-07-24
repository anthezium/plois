INCLUDES := -I.
TARGETS := length length.o curry.o

length : length.o curry.o
	$(CXX) $^ -std=c++11 -o length

length.o : length.cpp curry.hpp
	$(CXX) -c $< -std=c++11 $(INCLUDES)

curry.o : curry.cpp curry.hpp
	$(CXX) -c $< -std=c++11 $(INCLUDES)

clean :
	rm $(TARGETS)
