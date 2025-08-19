#!/usr/bin/env sh

CFLAGS="-Wall -Wextra -std=c11 -ggdb"
SRC="vec.c symtable.c main.c"
OUT="alisp.out"

set -xe

cc $CFLAGS -o $OUT $SRC;
