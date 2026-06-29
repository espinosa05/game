#!/bin/bash
set -x
gcc -ggdb -I../../lib/libcore/include string_builder_test.c ../../lib/libcore/core/out/libcore.a -o string_builder_test
