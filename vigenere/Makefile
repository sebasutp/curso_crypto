
FLAGS=-std=c++11 -O2
LIBS=-lboost_program_options
CC=g++

all: vigenere.o lang_model.o breaker.o 

vigenere.o: vigenere.cpp ../encoding.h
	$(CC) vigenere.cpp -o vigenere.o $(FLAGS) $(LIBS)

lang_model.o: lang_model.cpp ../encoding.h
	$(CC) lang_model.cpp -o lang_model.o $(FLAGS) $(LIBS)

breaker.o: breaker.cpp ../encoding.h
	$(CC) breaker.cpp -o breaker.o $(FLAGS) $(LIBS)

clean:
	rm *.o
