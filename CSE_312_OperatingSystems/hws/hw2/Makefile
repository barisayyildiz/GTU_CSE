CC = g++
CFLAGS = -std=c++11 -Iinclude

part1: part1.o MemoryManagement.o mylib.o
	$(CC) $(CFLAGS) part1.o MemoryManagement.o mylib.o -o part1

part1.o: part1.cpp include/MemoryManagement.h
	$(CC) $(CFLAGS) -c part1.cpp -o part1.o

part2: part2.o MemoryManagement.o mylib.o
	$(CC) $(CFLAGS) part2.o MemoryManagement.o mylib.o -o part2

part2.o: part2.cpp include/MemoryManagement.h
	$(CC) $(CFLAGS) -c part2.cpp -o part2.o

part3: part3.o MemoryManagement.o mylib.o
	$(CC) $(CFLAGS) part3.o MemoryManagement.o mylib.o -o part3

part3.o: part3.cpp include/MemoryManagement.h
	$(CC) $(CFLAGS) -c part3.cpp -o part3.o

MemoryManagement.o: include/MemoryManagement.cpp include/MemoryManagement.h
	$(CC) $(CFLAGS) -c include/MemoryManagement.cpp -o MemoryManagement.o

mylib.o: include/mylib.cpp include/MemoryManagement.h
	$(CC) $(CFLAGS) -c include/mylib.cpp -o mylib.o

clean:
	rm -f *.o part1 part2 part3 *.bin *.dat
