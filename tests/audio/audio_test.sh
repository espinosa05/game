#!/bin/bash
set -x

LIB_BASE_DIR=../../lib

CORE_BASE_DIR=$LIB_BASE_DIR/libcore
KIEK_BASE_DIR=$LIB_BASE_DIR/libkiek

CORE_INC_DIR=$CORE_BASE_DIR/include
KIEK_INC_DIR=$KIEK_BASE_DIR/include

CORE_LIB_DIR=$CORE_BASE_DIR/core
KIEK_LIB_DIR=$KIEK_BASE_DIR/kiek

CORE_OUT_DIR=$CORE_LIB_DIR/out
KIEK_OUT_DIR=$KIEK_LIB_DIR/out

gcc -c audio_test.c -I $CORE_INC_DIR -I $KIEK_INC_DIR $(pkg-config --cflags --libs libpipewire-0.3)
gcc audio_test.o $CORE_OUT_DIR/libcore.a $KIEK_OUT_DIR/libkiek.a -o audio_test -lvulkan -lxcb -lxcb-icccm -lxcb-keysyms -lpipewire-0.3 -lm
./audio_test
