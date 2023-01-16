
#  *  @file makefile
#  *  AUTHORS: Niv Kotek - 208236315
# 
#To run, type in the command line "make all" and then type 
# ./shell
# make clean

CC = gcc
CFLAGS = -g -Wall

all: shell 

shell: shell.o
	$(CC) $(CFLAGS) -o shell shell.o

shell.o: shell.c 
	$(CC) $(CFLAGS) -c shell.c

.PHONY: clean all

clean:
	rm -f *.o  shell history.txt