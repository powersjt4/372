###
###@file - Make file for chatclient 
###@author - Jacob Powers
###@description - Make file to compile chatclient

CC = gcc
CFLAGS = -std=gnu99
CFLAGS += -Wall 
CFLAGS += -pedantic-errors
LDFLAGS = -lm
OBJS = ftserver.o
SRCS = ftserver.c
HEADERS = 

default: all 

 all: 
	$(CC) ${CFLAGS} ${SRCS} -o ftserver $(LDFLAGS)

test: ftserver.c
	${CC} -g -v ftserver.c -o test 

clean:
	${RM} -rf ftserver test  *.o 
