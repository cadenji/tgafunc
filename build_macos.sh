#!/bin/bash

OPTS="-o test.out -std=c99 -Wall -I ./"
SRCS="tgafunc.c test/test.c"

clang $OPTS $SRCS