CC = gcc

#Name of our library
LIB_NAME = libBeMa

#Library
lib: saveMain.o
	$(CC) saveMain.o $(COMPILER_FLAGS) -shared -o $(LIB_NAME).so

#Object files
saveMain.o: saveMain.c
	${CC} saveMain.c $(COMPILER_FLAGS) -fPIC -c

#gcc -c -fPIC -Wall -Wextra saveMain.c -o saveMain.o
#gcc saveMain.o -shared -o libBeMa.so