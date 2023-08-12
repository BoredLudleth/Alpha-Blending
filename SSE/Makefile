all: link

link: main.cpp alphablender.cpp
	g++ main.cpp alphablender.cpp  -o main -mavx2 -O1 -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

clean:
	rm *.o *.out *.lst main