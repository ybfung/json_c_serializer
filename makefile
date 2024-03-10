
srcdir = .

AR = ar
CC = gcc
#CFLAGS = -m32 -g -O2 -Wall -Wno-format-extra-args
CFLAGS = -g -O2 -Wall
LDFLAGS = 
OBJ_PATH = ./obj/
SRCS = $(wildcard ./src/*.c)
OBJS = $(SRCS:./src/%.c=$(OBJ_PATH)%.o)
	
OBJ := $(patsubst %,$(OBJ_PATH)%,$(OBJS))

all: libjson_serializer.a libjson_serializer.so

libjson_serializer.so: $(OBJS)
	$(CC) $(CFLAGS) -shared -o libjson_serializer.so $^

libjson_serializer.a: $(OBJS)
	$(AR) rcs libjson_serializer.a $^

$(OBJ_PATH)%.o: ./src/%.c
	$(CC) $(CFLAGS) -I./inc/ -o $@ -c $^

clean:
	rm -f $(OBJ_PATH)json.o $(OBJ_PATH)json-builder.o $(OBJ_PATH)json_serializer.o libjson_serializer.a libjson_serializer.so

.PHONY: all clean
