#!/bin/bash
set -x

cc nob.c -o nob
cd lib/
    cd core/
        cc nob.c -o nob
    cd ../
    cd kiek/
        cc nob.c -o nob
    cd ../
cd ../

cd game
    cc nob.c -o nob
cd ../
