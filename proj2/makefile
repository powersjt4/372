###
###@file - Make file for chatclient 
###@author - Jacob Powers
###@description - Make file to compile chatclient

CC = gcc
CFLAGS = -std=gnu99
CFLAGS += -Wall 
CFLAGS += -pedantic-errors
LDFLAGS = -lm
OBJS = charclient.o
SRCS = chatclient.c
HEADERS = 

default: all 

 all: 
	$(CC) ${CFLAGS} ${SRCS} -o chatclient $(LDFLAGS)

test: chatclient.c
	${CC} -g -v chatclient.c -o test 

clean:
	${RM} -rf chatclient test  *.o 
