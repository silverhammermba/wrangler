all: wrangle

wrangle: wrangle.o cow.o helpers.o
	g++ -o launch wrangle.o cow.o helpers.o -lsfml-graphics -lsfml-window -lsfml-system

wrangle.o: wrangle.cpp helpers.hpp
	g++ -c wrangle.cpp -std=c++11

cow.o: cow.cpp cow.hpp helpers.hpp
	g++ -c cow.cpp -std=c++11

helpers.o: helpers.cpp helpers.hpp
	g++ -c helpers.cpp -std=c++11

clean:
	rm -f *~ *.o *.out launch
