#!/bin/sh
c89 -DR30S_NONSTATIC -x c -c r30sponge.h && \
    ar rcs libr30sponge.a r30sponge.o && \
    rustc -L . main.rs && \
    ./main
