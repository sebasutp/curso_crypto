
FLAGS=-std=c++11 -g
LIBS=-lboost_program_options

all: vigenere.o

vigenere.o: vigenere.cpp ../encoding.h
	g++ vigenere.cpp -o vigenere.o $(FLAGS) $(LIBS)

clean:
	rm *.o
