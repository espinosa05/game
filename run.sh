#!/bin/bash

ARGC=$#

project="sample"

if [[ $ARGC -ne 2 ]]
then
    echo "no project selected!"
    echo "continuing with first default sample..."
else
    project=$1
fi

cp -r projects/$project lib/

LIB_NTT_INCLUDE_DIR=$(pwd)/lib/libntt/include LIB_CORE_INCLUDE_DIR=$(pwd)/lib/libcore/include LIB_BE_INCLUDE_DIR=$(pwd)/lib/libbe/include ./nob $project run
