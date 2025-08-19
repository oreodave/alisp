#!/usr/bin/env sh

CFLAGS="-Wall -Wextra -std=c11 -ggdb -fsanitize=address -fsanitize=undefined"
SRC="vec.c tag.c symtable.c main.c"
OUT="alisp.out"

set -xe

cc $CFLAGS -o $OUT $SRC;
