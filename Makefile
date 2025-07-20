
CC = gcc
CFLAGS = -g

SRC_FILES = $(wildcard src/*.c) 
OBJ_FILES = $(SRC_FILES:.c=.o)

LIB_NAME = drings

all: lib/lib$(LIB_NAME).a

lib/lib$(LIB_NAME).a: $(OBJ_FILES)
	mkdir -p lib 
	ar cr lib/lib$(LIB_NAME).a $(OBJ_FILES)
	rm -f $(OBJ_FILES)

%.o: %.c 
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf lib $(OBJ_FILES)

install: lib/lib$(LIB_NAME).a
	cp lib/lib$(LIB_NAME).a /usr/local/lib/ 
	cp -r include/$(LIB_NAME) /usr/local/include/ 

uninstall:
	rm -rf /usr/local/lib/lib$(LIB_NAME).a 
	rm -rf /usr/local/include/$(LIB_NAME)/

example: lib/lib$(LIB_NAME).a install
	$(CC) $(CFLAGS) example.c -o main -l:lib$(LIB_NAME).a

.PHONY: all clean install uninstall 

