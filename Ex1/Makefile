
#  *  @file makefile
#  *  AUTHORS: Niv Kotek - 208236315
# 
#To run, type in the command line "make all" and then type 
# (encryption)- ./coder key -e < my_original_file > encrypted_file 
# (decryption)- ./coder key -d < my_descripter_file > my_original_file 

# You can run test.txt with 100KB as an example


CC=gcc
Wall=-Wall -pthread  -lpthread
OBJ=thpool.o job.o binary_semaphore.o
all: coder

coder: codec.h main.c $(OBJ)
	$(CC) $(Wall) main.c -L. -l Codec -o coder $(OBJ)

binary_semaphore.o: binary_semaphore.c binary_semaphore.h
	$(CC) -c binary_semaphore.c

job.o: job.c job.h
	$(CC) -c job.c

thpool.o: thpool.c thpool.h
	$(CC) -c thpool.c

.PHONY: clean all
clean:
	rm -f coder *.o
