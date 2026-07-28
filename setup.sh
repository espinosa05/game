#!/bin/bash

build_nob() {
    pushd $1
    gcc nob.c -o nob
    popd
}

PROJECT="sample/"
LIBCORE_SRCD="lib/libcore/core/"
LIBBE_SRCD="lib/libbe/be/"
PROJECT_SRCD="projects/$PROJECT"

if [ $# -ne 2 ]; then
    echo "no project selected!"
else
    PROJECT=$1
fi

echo "continuing with $PROJECT..."

set -x
build_nob .
build_nob $LIBCORE_SRCD
build_nob $LIBBE_SRCD
build_nob $PROJECT_SRCD
