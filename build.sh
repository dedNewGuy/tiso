#!/usr/bin/env sh

set -xe

clang -Wall -Wextra -o tiso tiso.c term.c
