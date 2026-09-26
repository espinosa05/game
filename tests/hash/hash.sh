#!/bin/bash
set -x

LIB_BASE_DIR=../../lib

CORE_BASE_DIR=$LIB_BASE_DIR/libcore

CORE_INC_DIR=$CORE_BASE_DIR/include

CORE_LIB_DIR=$CORE_BASE_DIR/core

CORE_OUT_DIR=$CORE_LIB_DIR/out

gcc -c hash.c -I $CORE_INC_DIR
gcc hash.o $CORE_OUT_DIR/libcore.a -o hash
./hash
