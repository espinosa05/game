CC=gcc
CFLAGS=-Wall -Werror -Wextra -pedantic -std=gnu99
INC_DIRS=-I../../include
LIB_LINKS=-lcore
OUT=out/

all: net

net: $(OUT) $(OUT)net.so

$(OUT)net.so: networking.c
	$(CC) -shared -fPIC $(INC_DIRS) $(CFLAGS) $^ -o $@

$(OUT):
	mkdir $@
