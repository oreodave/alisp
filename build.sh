#!/usr/bin/env sh

set -xe

CFLAGS="-Wall -Wextra -std=c11 -ggdb -fsanitize=address -fsanitize=undefined -Wswitch -Wswitch-enum"
LDFLAGS="-lc"

LIB=$(find "./runtime" -type 'f')
OUT="build/alisp.out"

build() {
    mkdir -p build;
    cc $CFLAGS -o $OUT $LIB main.c $LDFLAGS;
    cc $CFLAGS -o build/test.out $LIB test/test.c $LDFLAGS;
}

clean() {
    rm -v $OUT test.out;
}

run() {
    ./$OUT;
}

test() {
    ./build/test.out
}

build

if [ "$1" = "run" ]
then
    run
elif [ "$1" = "test" ]
then
    test
elif [ "$1" = "clean" ]
then
    clean
fi
