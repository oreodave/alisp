#!/usr/bin/env sh

CFLAGS="-Wall -Wextra -std=c11 -ggdb"
SRC="main.c"
OUT="main.out"

set -xe

cc $CFLAGS -o $OUT $SRC;
