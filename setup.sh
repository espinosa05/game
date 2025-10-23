#!/bin/bash
set -x

cc nob.c -o nob
cd lib/
    cd libcore/core
        cc nob.c -o nob
    cd ../
    cd libkiek/kiek
        cc nob.c -o nob
    cd ../
cd ../

cd game
    cc nob.c -o nob
cd ../
