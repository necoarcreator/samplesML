
CC = g++
CFLAGS = -Wall 
All: execute

execute: main.o readerEncoder.o KohonenMap.o Normalizer.o 
	$(CC) $(CFLAGS) main.o readerEncoder.o KohonenMap.o Normalizer.o -o execute

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp -o main.o

readerEncoder.o: readerEncoder.cpp
	$(CC) $(CFLAGS) -c readerEncoder.cpp -o readerEncoder.o

KohonenMap.o: KohonenMap.cpp
	$(CC) $(CFLAGS) -c KohonenMap.cpp -o KohonenMap.o

Normalizer.o: Normalizer.cpp
	$(CC) $(CFLAGS) -c Normalizer.cpp -o Normalizer.o

clean: 
	@echo "Cleaning up..."
	rm execute
	rm -rf *.o execute 
	

rebuild: clean execute

run: execute
	./execute
