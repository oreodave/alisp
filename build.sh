#!/usr/bin/env sh

set -xe

CFLAGS="-Wall -Wextra -std=c11 -ggdb -fsanitize=address -fsanitize=undefined"
SRC="vec.c symtable.c tag.c constructor.c sys.c main.c"
OUT="alisp.out"

build() {
    cc $CFLAGS -o $OUT $SRC;
}

run() {
    ./$OUT;
}

build

if [ "$1" = "run" ]
then
    run
fi
