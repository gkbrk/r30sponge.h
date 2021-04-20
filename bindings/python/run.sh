#!/bin/sh
c89 -DR30S_NONSTATIC -x c -shared -o r30sponge.so r30sponge.h &&
    python3 ./expected_values.py
