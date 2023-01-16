
#  *  @file makefile
#  *  AUTHORS: Niv Kotek - 208236315
# 
#To run, type in the command line "make all" and then type 
# ./page
# make clean

CC = gcc
CFLAGS = -g -Wall

all: page

page: page.o
	$(CC) $(CFLAGS) -o page page.o

page.o: page.c 
	$(CC) $(CFLAGS) -c page.c

.PHONY: clean all

clean:
	rm -f *.o  page