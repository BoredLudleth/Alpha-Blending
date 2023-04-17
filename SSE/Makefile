all: link

link: main.cpp alphablender.cpp
	g++ main.cpp alphablender.cpp  -o main -Ofast -mavx2 -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

test: test.cpp
	gcc test.cpp -o test -Ofast -mavx2 -msse

clean:
	rm *.o *.out *.lst main