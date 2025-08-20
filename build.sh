#!/usr/bin/env sh

CFLAGS="-Wall -Wextra -std=c11 -ggdb -fsanitize=address -fsanitize=undefined"
SRC="ivec.c symtable.c tag.c constructor.c sys.c main.c"
OUT="alisp.out"

set -xe

cc $CFLAGS -o $OUT $SRC;
