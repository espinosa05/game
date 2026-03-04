#!/bin/bash
set -x

cc nob.c -o nob
cd lib/
    cd libcore/
        cd core/
            cc nob.c -o nob
        cd ../
    cd ../
    cd libkiek/
        cd kiek/
            cc nob.c -o nob
        cd ../
    cd ../
    cd libbe/
        cd be/
            cc nob.c -o nob
        cd ../
    cd ../

cd ../

