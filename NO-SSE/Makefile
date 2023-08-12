all: link compile

link: main.cpp alphablender.cpp
	gcc -c main.cpp -o main.o
	gcc -c alphablender.cpp -o alphablender.o 

compile: main.o alphablender.o
	g++ main.o alphablender.o -O2 -o main -lsfml-graphics -lsfml-window -lsfml-system

clean:
	rm *.o *.out *.lst main