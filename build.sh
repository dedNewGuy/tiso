#!/usr/bin/env sh

set -xe

clang -Wall -Wextra -o tiso tiso.c term.c
# clang -Wall -Wextra -o tiso_debug tiso.c term.c
