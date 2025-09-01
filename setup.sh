#!/bin/bash
set -x
cc nob.c -o nob
cc lib/core/nob.c -o lib/core/nob
cc game/nob.c -o game/nob
