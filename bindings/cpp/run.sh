#!/bin/sh
c89 -DR30S_NONSTATIC -x c -c r30sponge.h && \
    g++ main.cpp r30sponge.o && \
    ./a.out
