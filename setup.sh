#!/bin/bash
set -x
cc nob.c -o nob
cd lib/core/
cc nob.c -o nob
cd ../../
cd game
cc nob.c -o nob
cd ..
