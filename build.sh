#!/usr/bin/env sh

set -xe

CFLAGS="-Wall -Wextra -std=c11 -ggdb -fsanitize=address -fsanitize=undefined -Wswitch -Wswitch-enum"
LINK="-I."
LIB=$(find "./" -name "*.c" -not -name "main.c" -not -name "test.c")
OUT="alisp.out"

build() {
    cc $LINK $CFLAGS -o $OUT $LIB main.c;
    cc $LINK $CFLAGS -o test.out $LIB test.c;
}

clean() {
    rm -v $OUT test.out;
}

run() {
    ./$OUT;
}

test() {
    ./test.out
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
