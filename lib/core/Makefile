CC=gcc
CFLAGS=-Wall -Werror -Wpedantic -std=gnu99 -Wextra
SHARED_FLAGS=-fPIC -shared

TEST_DIR=tests/

INC_DIRS=-I ../../include/
CORE_SRC=os_linux.c error_report.c

OUT=out/

all: core
core: $(OUT)core.so

$(OUT)core.so: $(OUT) $(CORE_SRC)
	$(CC) $(CFLAGS) $(INC_DIRS) $(SHARED_FLAGS) $(CORE_SRC) -o $@

$(OUT):
	mkdir $@
