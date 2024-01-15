#!/bin/sh

set -e

build() {
    # build the cpp project
    ninja -C builddir/
}

run() {
    ./builddir/porkchop
}

plot(){
    python3 scripts/plot.py
}

if [ "$#" -ne 1 ]; then
    echo "not enough arguments"
    exit 1
fi

if [ "$1" = "build" ]; then
    build
elif [ "$1" = "run" ]; then
    run
    plot
elif [ "$1" = "all" ]; then
    build
    echo "building the porkchop plot"
    run
    echo "plotting"
    plot
fi
